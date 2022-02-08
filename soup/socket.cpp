#include "socket.hpp"

// TLS Support
// - Windows: Adapted from https://github.com/david-maw/StreamSSL
// - Linux: Not yet! Maybe I'll use OpenSSL, but then any .so requiring soup would be stuck loaded, because OpenSSL loves to leak. Maybe LibreSSL?

#include "dns.hpp"
#include "rand.hpp"

#if SOUP_PLATFORM_WINDOWS
#include <schannel.h>
#endif

namespace soup
{
#if SOUP_PLATFORM_WINDOWS
	PSecurityFunctionTableA socket::sft = nullptr;
#endif

	bool socket::connectReliable(const char* host, uint16_t port) noexcept
	{
		auto res = dns::lookupIPv4(host);
		if (!res.empty() && connect(rand(res), port))
		{
			return true;
		}
		res = dns::lookupIPv6(host);
		if (!res.empty() && connect(rand(res), port))
		{
			return true;
		}
		return false;
	}

	bool socket::encrypt(const char* server_name) noexcept
	{
#if SOUP_PLATFORM_WINDOWS
		if (sft == nullptr)
		{
			sft = InitSecurityInterface();
			if (sft == nullptr)
			{
				return false;
			}
		}

		if (!SecIsValidHandle(&cred_h))
		{
			SCHANNEL_CRED SchannelCred{};
			SchannelCred.dwVersion = SCHANNEL_CRED_VERSION;
			SchannelCred.grbitEnabledProtocols = SP_PROT_TLS1_2_CLIENT | SP_PROT_TLS1_3_CLIENT;
			SchannelCred.dwFlags = SCH_CRED_MANUAL_CRED_VALIDATION | SCH_CRED_NO_DEFAULT_CREDS | SCH_USE_STRONG_CRYPTO;
			TimeStamp ts;
			if (sft->AcquireCredentialsHandle(nullptr, (SEC_CHAR*)UNISP_NAME, SECPKG_CRED_OUTBOUND, nullptr, &SchannelCred, nullptr, nullptr, &cred_h, &ts) != SEC_E_OK)
			{
				return false;
			}
		}

		DWORD dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT
			| ISC_REQ_REPLAY_DETECT
			| ISC_REQ_CONFIDENTIALITY
			| ISC_REQ_EXTENDED_ERROR
			| ISC_REQ_ALLOCATE_MEMORY
			| ISC_REQ_STREAM;

		SecBuffer OutBuffers[1];
		OutBuffers[0].pvBuffer = nullptr;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;

		SecBufferDesc OutBuffer;
		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		TimeStamp ts;
		SECURITY_STATUS scRet = sft->InitializeSecurityContext(&cred_h, nullptr, (SEC_CHAR*)server_name, dwSSPIFlags, 0, SECURITY_NATIVE_DREP, nullptr, 0, &ctx_h, &OutBuffer, &dwSSPIFlags, &ts);
		if (scRet != SEC_I_CONTINUE_NEEDED)
		{
			return false;
		}
		if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != nullptr)
		{
			if (!sendUnencrypted(OutBuffers[0].pvBuffer, (int)OutBuffers[0].cbBuffer))
			{
				sft->FreeContextBuffer(OutBuffers[0].pvBuffer);
				return false;
			}
			sft->FreeContextBuffer(OutBuffers[0].pvBuffer);
		}

		DWORD dwSSPIOutFlags;
		DWORD cbIoBuffer = 0;
		BOOL fDoRead = TRUE;
		int cbData;
		SecBufferDesc InBuffer;
		SecBuffer InBuffers[2];

		do
		{
			if (0 == cbIoBuffer || scRet == SEC_E_INCOMPLETE_MESSAGE)
			{
				if (fDoRead)
				{
					cbData = recvUnencrypted(readBuffer + cbIoBuffer, sizeof(readBuffer) - cbIoBuffer);
					if (cbData == SOCKET_ERROR)
					{
						scRet = SEC_E_INTERNAL_ERROR;
						break;
					}
					else if (cbData == 0)
					{
						scRet = SEC_E_INTERNAL_ERROR;
						break;
					}

					cbIoBuffer += cbData;
				}
				else
				{
					fDoRead = TRUE;
				}
			}

			InBuffers[0].pvBuffer = readBuffer;
			InBuffers[0].cbBuffer = cbIoBuffer;
			InBuffers[0].BufferType = SECBUFFER_TOKEN;

			InBuffers[1].pvBuffer = nullptr;
			InBuffers[1].cbBuffer = 0;
			InBuffers[1].BufferType = SECBUFFER_EMPTY;

			InBuffer.cBuffers = 2;
			InBuffer.pBuffers = InBuffers;
			InBuffer.ulVersion = SECBUFFER_VERSION;

			OutBuffers[0].pvBuffer = nullptr;
			OutBuffers[0].BufferType = SECBUFFER_TOKEN;
			OutBuffers[0].cbBuffer = 0;

			OutBuffer.cBuffers = 1;
			OutBuffer.pBuffers = OutBuffers;
			OutBuffer.ulVersion = SECBUFFER_VERSION;

			scRet = sft->InitializeSecurityContext(&cred_h,
				&ctx_h,
				nullptr,
				dwSSPIFlags,
				0,
				SECURITY_NATIVE_DREP,
				&InBuffer,
				0,
				nullptr,
				&OutBuffer,
				&dwSSPIOutFlags,
				&ts);

			if (scRet == SEC_E_OK ||
				scRet == SEC_I_CONTINUE_NEEDED ||
				(FAILED(scRet) && (dwSSPIOutFlags & ISC_RET_EXTENDED_ERROR)))
			{
				if (OutBuffers[0].cbBuffer != 0 && OutBuffers[0].pvBuffer != nullptr)
				{
					if (!sendUnencrypted(OutBuffers[0].pvBuffer, OutBuffers[0].cbBuffer))
					{
						sft->FreeContextBuffer(OutBuffers[0].pvBuffer);
						scRet = SEC_E_INTERNAL_ERROR;
						break;
					}

					sft->FreeContextBuffer(OutBuffers[0].pvBuffer);
					OutBuffers[0].pvBuffer = nullptr;
				}
			}

			if (scRet == SEC_E_INCOMPLETE_MESSAGE)
			{
				continue;
			}

			if (scRet == SEC_E_OK)
			{
				if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
				{
					MoveMemory(readBuffer,
						readBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
						InBuffers[1].cbBuffer);

					readBufferBytes = InBuffers[1].cbBuffer;
				}
				else
				{
					readBufferBytes = 0;
				}
				break;
			}

			if (FAILED(scRet))
			{
				break;
			}

			if (InBuffers[1].BufferType == SECBUFFER_EXTRA)
			{
				MoveMemory(readBuffer,
					readBuffer + (cbIoBuffer - InBuffers[1].cbBuffer),
					InBuffers[1].cbBuffer);

				cbIoBuffer = InBuffers[1].cbBuffer;
			}
			else
			{
				cbIoBuffer = 0;
			}
		}
		while (scRet == SEC_I_CONTINUE_NEEDED || scRet == SEC_E_INCOMPLETE_MESSAGE);

		if (FAILED(scRet)
			|| FAILED(sft->QueryContextAttributes(&ctx_h, SECPKG_ATTR_STREAM_SIZES, &Sizes))
			)
		{
			releaseContext();
			return false;
		}

		encrypted = true;
		readPtr = readBuffer;

		return true;
#else
		return false;
#endif
	}

	bool socket::send(const void* data, int size) noexcept
	{
#if SOUP_PLATFORM_WINDOWS
		if (!encrypted)
		{
			return sendUnencrypted(data, size);
		}

		if (size > MaxMsgSize)
		{
			return false;
		}

		SecBufferDesc   Message;
		SecBuffer       Buffers[4];

		Message.ulVersion = SECBUFFER_VERSION;
		Message.cBuffers = 4;
		Message.pBuffers = Buffers;

		Buffers[0].BufferType = SECBUFFER_EMPTY;
		Buffers[1].BufferType = SECBUFFER_EMPTY;
		Buffers[2].BufferType = SECBUFFER_EMPTY;
		Buffers[3].BufferType = SECBUFFER_EMPTY;

		memcpy_s(writeBuffer + Sizes.cbHeader, sizeof(writeBuffer) - Sizes.cbHeader - Sizes.cbTrailer, data, size);

		Buffers[0].pvBuffer = writeBuffer;
		Buffers[0].cbBuffer = Sizes.cbHeader;
		Buffers[0].BufferType = SECBUFFER_STREAM_HEADER;

		Buffers[1].pvBuffer = writeBuffer + Sizes.cbHeader;
		Buffers[1].cbBuffer = static_cast<unsigned long>(size);
		Buffers[1].BufferType = SECBUFFER_DATA;

		Buffers[2].pvBuffer = writeBuffer + Sizes.cbHeader + size;
		Buffers[2].cbBuffer = Sizes.cbTrailer;
		Buffers[2].BufferType = SECBUFFER_STREAM_TRAILER;

		Buffers[3].BufferType = SECBUFFER_EMPTY;

		return !FAILED(sft->EncryptMessage(&ctx_h, 0, &Message, 0))
			&& sendUnencrypted(writeBuffer, static_cast<size_t>(Buffers[0].cbBuffer) + Buffers[1].cbBuffer + Buffers[2].cbBuffer);
#else
		return sendUnencrypted(data, size);
#endif
	}

	int socket::recv(void* outData, int size) noexcept
	{
#if SOUP_PLATFORM_WINDOWS
		if (!encrypted)
		{
			return recvUnencrypted(outData, size);
		}

		if (plainTextBytes > 0)
		{
			if (size >= plainTextBytes)
			{
				auto bytesReturned = plainTextBytes;
				memcpy_s(outData, size, plainTextPtr, plainTextBytes);
				plainTextBytes = 0;
				return static_cast<int>(bytesReturned);
			}
			else
			{
				memcpy_s(outData, size, plainTextPtr, size);
				plainTextPtr += size;
				plainTextBytes -= size;
				return static_cast<int>(size);
			}
		}

		SecBufferDesc   Message;
		SecBuffer       Buffers[4];
		SECURITY_STATUS scRet;

		Message.ulVersion = SECBUFFER_VERSION;
		Message.cBuffers = 4;
		Message.pBuffers = Buffers;

		Buffers[0].BufferType = SECBUFFER_EMPTY;
		Buffers[1].BufferType = SECBUFFER_EMPTY;
		Buffers[2].BufferType = SECBUFFER_EMPTY;
		Buffers[3].BufferType = SECBUFFER_EMPTY;

		if (readBufferBytes == 0)
		{
			scRet = SEC_E_INCOMPLETE_MESSAGE;
		}
		else
		{
			Buffers[0].pvBuffer = readPtr;
			Buffers[0].cbBuffer = static_cast<unsigned long>(readBufferBytes);
			Buffers[0].BufferType = SECBUFFER_DATA;
			scRet = sft->DecryptMessage(&ctx_h, &Message, 0, nullptr);
		}

		while (scRet == SEC_E_INCOMPLETE_MESSAGE)
		{
			size_t freeBytesAtStart = static_cast<int>((CHAR*)readPtr - &readBuffer[0]);
			size_t freeBytesAtEnd = static_cast<int>(sizeof(readBuffer)) - readBufferBytes - freeBytesAtStart;
			if (freeBytesAtEnd == 0)
			{
				if (freeBytesAtStart <= 0)
				{
					return SOCKET_ERROR;
				}
				memmove_s(readBuffer, sizeof(readBuffer), readPtr, sizeof(readBuffer) - freeBytesAtStart);
				freeBytesAtEnd = freeBytesAtStart;
				readPtr = readBuffer;
			}
			const int err = recvUnencrypted((CHAR*)readPtr + readBufferBytes, freeBytesAtEnd);
			if (err == SOCKET_ERROR || err == 0)
			{
				return SOCKET_ERROR;
			}
			readBufferBytes += err;

			Buffers[0].pvBuffer = readPtr;
			Buffers[0].cbBuffer = static_cast<unsigned long>(readBufferBytes);
			Buffers[0].BufferType = SECBUFFER_DATA;

			Buffers[1].BufferType = SECBUFFER_EMPTY;
			Buffers[2].BufferType = SECBUFFER_EMPTY;
			Buffers[3].BufferType = SECBUFFER_EMPTY;

			scRet = sft->DecryptMessage(&ctx_h, &Message, 0, nullptr);
		}


		if (scRet != SEC_E_OK)
		{
			if (scRet == SEC_I_CONTEXT_EXPIRED)
			{
				encrypted = false;
			}
			// BUG: SEC_I_RENEGOTIATE is not handled
			return SOCKET_ERROR;
		}

		PSecBuffer pDataBuffer(nullptr);

		for (int i = 1; i < 4; i++)
		{
			if (Buffers[i].BufferType == SECBUFFER_DATA)
			{
				pDataBuffer = &Buffers[i];
				break;
			}
		}

		if (!pDataBuffer)
		{
			return SOCKET_ERROR;
		}

		if (size >= int(pDataBuffer->cbBuffer))
		{
			memcpy_s(outData, size, pDataBuffer->pvBuffer, pDataBuffer->cbBuffer);
		}
		else
		{
			memcpy_s(outData, size, pDataBuffer->pvBuffer, size);
			plainTextBytes = pDataBuffer->cbBuffer - size;
			plainTextPtr = plainText;
			if (memcpy_s(plainText, sizeof(plainText), (char*)pDataBuffer->pvBuffer + size, plainTextBytes))
			{
				return SOCKET_ERROR;
			}
			pDataBuffer->cbBuffer = static_cast<unsigned long>(size);
		}

		PSecBuffer pExtraDataBuffer(nullptr);

		for (int i = 1; i < 4; i++)
		{
			if (Buffers[i].BufferType == SECBUFFER_EXTRA)
			{
				pExtraDataBuffer = &Buffers[i];
				break;
			}
		}

		if (pExtraDataBuffer)
		{
			readBufferBytes = pExtraDataBuffer->cbBuffer;
			readPtr = pExtraDataBuffer->pvBuffer;
		}
		else
		{
			readBufferBytes = 0;
			readPtr = readBuffer;
		}

		return pDataBuffer->cbBuffer;
#else
		return recvUnencrypted(outData, size);
#endif
	}

	void socket::sendCloseNotify() noexcept
	{
#if SOUP_PLATFORM_WINDOWS
		DWORD           dwType = SCHANNEL_SHUTDOWN;
		PBYTE           pbMessage;
		DWORD           cbMessage;

		SecBufferDesc   OutBuffer;
		SecBuffer       OutBuffers[1];
		DWORD           dwSSPIFlags;
		TimeStamp       tsExpiry;

		OutBuffers[0].pvBuffer = &dwType;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = sizeof(dwType);

		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		if (FAILED(sft->ApplyControlToken(&ctx_h, &OutBuffer)))
		{
			return;
		}

		dwSSPIFlags = ISC_REQ_SEQUENCE_DETECT |
			ISC_REQ_REPLAY_DETECT |
			ISC_REQ_CONFIDENTIALITY |
			ISC_RET_EXTENDED_ERROR |
			ISC_REQ_ALLOCATE_MEMORY |
			ISC_REQ_STREAM;

		OutBuffers[0].pvBuffer = nullptr;
		OutBuffers[0].BufferType = SECBUFFER_TOKEN;
		OutBuffers[0].cbBuffer = 0;

		OutBuffer.cBuffers = 1;
		OutBuffer.pBuffers = OutBuffers;
		OutBuffer.ulVersion = SECBUFFER_VERSION;

		if (FAILED(sft->InitializeSecurityContext(
			&cred_h,
			&ctx_h,
			nullptr,
			dwSSPIFlags,
			0,
			SECURITY_NATIVE_DREP,
			&OutBuffer,
			0,
			nullptr,
			&OutBuffer,
			&dwSSPIFlags,
			&tsExpiry)))
		{
			return;
		}

		pbMessage = (PBYTE)OutBuffers[0].pvBuffer;
		cbMessage = OutBuffers[0].cbBuffer;

		if (pbMessage != nullptr && cbMessage != 0)
		{
			sendUnencrypted(pbMessage, cbMessage);

			sft->FreeContextBuffer(pbMessage);
		}
#endif
	}
}
