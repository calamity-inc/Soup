#pragma once

#include <string>
#include <vector>

namespace soup
{
	// Adapted from https://github.com/SergeyBel/AES

	class aes
	{
	public:
		// Input and IV size must be a multiple of 16 bytes. Key size must be 16 bytes, 24 bytes, or 32 bytes.

		[[nodiscard]] static std::vector<unsigned char> encryptECB(const std::vector<unsigned char>& in, const std::vector<unsigned char>& key);
		[[nodiscard]] static std::vector<unsigned char> decryptECB(const std::vector<unsigned char>& in, const std::vector<unsigned char>& key);
		[[nodiscard]] static std::vector<unsigned char> encryptCBC(const std::vector<unsigned char>& in, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
		[[nodiscard]] static std::vector<unsigned char> decryptCBC(const std::vector<unsigned char>& in, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
		[[nodiscard]] static std::vector<unsigned char> encryptCFB(const std::vector<unsigned char>& in, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);
		[[nodiscard]] static std::vector<unsigned char> decryptCFB(const std::vector<unsigned char>& in, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv);

		[[nodiscard]] static std::string encryptECB(const std::string& in, const std::string& key);
		[[nodiscard]] static std::string decryptECB(const std::string& in, const std::string& key);
		[[nodiscard]] static std::string encryptCBC(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string decryptCBC(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string encryptCFB(const std::string& in, const std::string& key, const std::string& iv);
		[[nodiscard]] static std::string decryptCFB(const std::string& in, const std::string& key, const std::string& iv);

	private:
		[[nodiscard]] static std::vector<unsigned char> KeyExpansion(const std::vector<unsigned char>& key);
		[[nodiscard]] static int getNk(const std::vector<unsigned char>& key);
		[[nodiscard]] static int getNr(const std::vector<unsigned char>& key);
		[[nodiscard]] static int getNr(const int Nk);

		static void SubBytes(unsigned char** state);
		static void ShiftRow(unsigned char** state, int i, int n);    // shift row i on n positions
		static void ShiftRows(unsigned char** state);
		static unsigned char xtime(unsigned char b);    // multiply on x
		static void MixColumns(unsigned char** state);
		static void AddRoundKey(unsigned char** state, unsigned char* key);
		static void SubWord(unsigned char* a);
		static void RotWord(unsigned char* a);
		static void XorWords(unsigned char* a, unsigned char* b, unsigned char* c);
		static void Rcon(unsigned char* a, int n);
		static void InvSubBytes(unsigned char** state);
		static void InvMixColumns(unsigned char** state);
		static void InvShiftRows(unsigned char** state);
		static void EncryptBlock(const unsigned char in[], unsigned char out[], unsigned char key[], const int Nr);
		static void DecryptBlock(const unsigned char in[], unsigned char out[], unsigned char key[], const int Nr);
		static void XorBlocks(const unsigned char* a, const unsigned char* b, unsigned char* c, unsigned int len);
	};
}
