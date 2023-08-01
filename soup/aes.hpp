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
		[[nodiscard]] static std::vector<uint8_t> expandKey(const uint8_t* key, size_t key_len);
		[[nodiscard]] static int getNk(size_t key_len);
		[[nodiscard]] static int getNr(size_t key_len);
		[[nodiscard]] static int getNr(const int Nk);

		static void subBytes(uint8_t** state);
		static void shiftRow(uint8_t** state, int i, int n);    // shift row i on n positions
		static void shiftRows(uint8_t** state);
		static uint8_t xtime(uint8_t b);    // multiply on x
		static void mixColumns(uint8_t** state);
		static void addRoundKey(uint8_t** state, uint8_t* key);
		static void subWord(uint8_t* a);
		static void rotWord(uint8_t* a);
		static void xorWords(uint8_t* a, uint8_t* b, uint8_t* c);
		static void getRoundConstant(uint8_t* a, int n);
		static void invSubBytes(uint8_t** state);
		static void invMixColumns(uint8_t** state);
		static void invShiftRows(uint8_t** state);
		static void encryptBlock(const uint8_t in[], uint8_t out[], uint8_t key[], const int Nr);
		static void decryptBlock(const uint8_t in[], uint8_t out[], uint8_t key[], const int Nr);
		static void xorBlocks(const uint8_t* a, const uint8_t* b, uint8_t* c, unsigned int len);
		static void incCounter(uint8_t* counter);
	};
}
