#pragma once

#include <string>

namespace soup
{
	// Adapted from https://github.com/SergeyBel/AES

	class aes
	{
	public:
		// Input size must be a multiple of 16 bytes. You may use a padding scheme such as PKCS#7 to ensure this.
		// Key size must be 16 bytes, 24 bytes, or 32 bytes.
		// IV size must be 16 bytes.

		static void pkcs7Pad(std::string& encrypted);
		static void pkcs7Unpad(std::string& decrypted);

		static void cbcEncrypt(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t iv[16]);
		static void cbcDecrypt(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t iv[16]);
		static void cfbEncrypt(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t iv[16]);
		static void cfbDecrypt(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t iv[16]);
		static void ecbEncrypt(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len);
		static void ecbDecrypt(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len);

	private:
		static void expandKey(uint8_t w[240], const uint8_t* key, size_t key_len);
		[[nodiscard]] static int getNk(size_t key_len);
		[[nodiscard]] static int getNr(size_t key_len);
		[[nodiscard]] static int getNr(const int Nk);

		static void subBytes(uint8_t** state);
		static void shiftRow(uint8_t** state, int i, int n);    // shift row i on n positions
		static void shiftRows(uint8_t** state);
		static uint8_t xtime(uint8_t b);    // multiply on x
		static void mixColumns(uint8_t** state);
		static void addRoundKey(uint8_t** state, const uint8_t* key);
		static void subWord(uint8_t* a);
		static void rotWord(uint8_t* a);
		static void xorWords(uint8_t* a, uint8_t* b, uint8_t* c);
		[[nodiscard]] static uint8_t getRoundConstant(int n);
		static void invSubBytes(uint8_t** state);
		static void invMixColumns(uint8_t** state);
		static void invShiftRows(uint8_t** state);
		static void encryptBlock(const uint8_t in[16], uint8_t out[16], const uint8_t roundKeys[240], const int Nr);
		static void decryptBlock(const uint8_t in[16], uint8_t out[16], const uint8_t roundKeys[240], const int Nr);
		static void xorBlocks(const uint8_t* a, const uint8_t* b, uint8_t* c, unsigned int len);
	};
}
