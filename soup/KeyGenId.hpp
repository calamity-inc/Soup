#pragma once

#include <string>

#include "BigintId.hpp"

namespace soup
{
	// KeyGenId: A neat way to derive keypairs from 128 seed bits for more convenient storage.
	//
	// This is based on original work, so it has not been subject to any peer review, which is
	// critical to determine the actual security of any cryptographic system, as per Schneier's Law.
	// Still, here is my analysis of the security of KeyGenId.
	//
	// Firstly, I have no reason to believe that a keypair derived with this method would be any less secure than
	// a keypair generated with more direct and less restricted RNG.
	//
	// Similarly, I see no problem with 128 random bits as a security token.
	//
	// So, the input & output of this algorithm seem fine, so what happens when they're combined?
	// KeyGenId splits the 128-bit into two 64-bit seeds, each of which is used to generate at least 1 prime number.
	// This means that the keypair's N and one of the 64-bit seeds would be sufficient to generate a few primes and compute N / P = Q.
	// Similarly, if N and, say, 60 bits of one of the seeds is known, it would be well within reason to bruteforce the remaining bits.
	//
	// To put it another way: If the public key will be public and a good part of the seed is revealed, it would be possible to derive the rest.
	// What "a good part of the seed" means exactly is hard to define, but it's at most 50% and approaching 0% with increasing complexity.
	struct KeyGenId
	{
		unsigned int bits;
		BigintId p;
		BigintId q;

		KeyGenId() {}
		KeyGenId(unsigned int bits, const std::string& seeds_export);

		[[nodiscard]] static KeyGenId generate(unsigned int bits);

		[[nodiscard]] std::string toSeedsExport() const;

		[[nodiscard]] RsaKeypair getKeypair() const;
	};
}
