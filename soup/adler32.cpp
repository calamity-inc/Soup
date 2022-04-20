#include "adler32.hpp"

#define BASE 65521U
#define NMAX 5552

#define DO1(buf,i)  {adler += (buf)[i]; sum2 += adler;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
#define MOD(a) a %= BASE
#define MOD28(a) a %= BASE
#define MOD63(a) a %= BASE

namespace soup
{
	uint32_t adler32::hash(const std::string& data)
	{
		return hash(data.data(), data.size());
	}

	uint32_t adler32::hash(const char* buf, size_t len)
	{
		return hash((const uint8_t*)buf, len);
	}

	uint32_t adler32::hash(const uint8_t* buf, size_t len, uint32_t init)
	{
		/* split Adler-32 into component sums */
		uint32_t sum2 = ((init >> 16) & 0xffff);
		uint32_t adler = (init & 0xffff);

		/* in case user likes doing a byte at a time, keep it fast */
		if (len == 1)
		{
			adler += buf[0];
			if (adler >= BASE)
			{
				adler -= BASE;
			}
			sum2 += adler;
			if (sum2 >= BASE)
			{
				sum2 -= BASE;
			}
			return adler | (sum2 << 16);
		}

		/* in case short lengths are provided, keep it somewhat fast */
		if (len < 16)
		{
			while (len--)
			{
				adler += *buf++;
				sum2 += adler;
			}
			if (adler >= BASE)
			{
				adler -= BASE;
			}
			MOD28(sum2);            /* only added so many BASE's */
			return adler | (sum2 << 16);
		}

		/* do length NMAX blocks -- requires just one modulo operation */
		while (len >= NMAX)
		{
			len -= NMAX;
			uint32_t n = NMAX / 16;          /* NMAX is divisible by 16 */
			do
			{
				DO16(buf);          /* 16 sums unrolled */
				buf += 16;
			} while (--n);
			MOD(adler);
			MOD(sum2);
		}

		/* do remaining bytes (less than NMAX, still just one modulo) */
		if (len) /* avoid modulos if none remaining */
		{
			while (len >= 16)
			{
				len -= 16;
				DO16(buf);
				buf += 16;
			}
			while (len--)
			{
				adler += *buf++;
				sum2 += adler;
			}
			MOD(adler);
			MOD(sum2);
		}

		/* return recombined sums */
		return adler | (sum2 << 16);
	}
}
