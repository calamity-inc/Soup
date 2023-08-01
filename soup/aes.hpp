#pragma once

#include <string>
#include <vector>

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

		[[nodiscard]] static std::string encryptCBC(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string decryptCBC(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string encryptCFB(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string decryptCFB(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string encryptECB(const std::string& in, const std::string& key);
		[[nodiscard]] static std::string decryptECB(const std::string& in, const std::string& key);

		[[nodiscard]] static std::vector<uint8_t> encryptCBC(const std::vector<uint8_t>& in, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
		static void encryptCBCInplace(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t iv[16]);
		[[nodiscard]] static std::vector<uint8_t> decryptCBC(const std::vector<uint8_t>& in, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
		static void decryptCBCInplace(std::vector<uint8_t>& data, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
		static void decryptCBCInplace(uint8_t* data, size_t data_len, const uint8_t* key, size_t key_len, const uint8_t iv[16]);
		[[nodiscard]] static std::vector<uint8_t> encryptCFB(const std::vector<uint8_t>& in, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
		[[nodiscard]] static std::vector<uint8_t> decryptCFB(const std::vector<uint8_t>& in, const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv);
		[[nodiscard]] static std::vector<uint8_t> encryptECB(const std::vector<uint8_t>& in, const std::vector<uint8_t>& key);
		[[nodiscard]] static std::vector<uint8_t> decryptECB(const std::vector<uint8_t>& in, const std::vector<uint8_t>& key);

	private:
		[[nodiscard]] static std::vector<uint8_t> KeyExpansion(const uint8_t* key, size_t key_len);
		[[nodiscard]] static int getNk(size_t key_len);
		[[nodiscard]] static int getNr(size_t key_len);
		[[nodiscard]] static int getNr(const int Nk);

		static void SubBytes(uint8_t** state);
		static void ShiftRow(uint8_t** state, int i, int n);    // shift row i on n positions
		static void ShiftRows(uint8_t** state);
		static uint8_t xtime(uint8_t b);    // multiply on x
		static void MixColumns(uint8_t** state);
		static void AddRoundKey(uint8_t** state, uint8_t* key);
		static void SubWord(uint8_t* a);
		static void RotWord(uint8_t* a);
		static void XorWords(uint8_t* a, uint8_t* b, uint8_t* c);
		static void Rcon(uint8_t* a, int n);
		static void InvSubBytes(uint8_t** state);
		static void InvMixColumns(uint8_t** state);
		static void InvShiftRows(uint8_t** state);
		static void EncryptBlock(const uint8_t in[], uint8_t out[], uint8_t key[], const int Nr);
		static void DecryptBlock(const uint8_t in[], uint8_t out[], uint8_t key[], const int Nr);
		static void XorBlocks(const uint8_t* a, const uint8_t* b, uint8_t* c, unsigned int len);
		static void IncCounter(uint8_t* counter);
	};
}
