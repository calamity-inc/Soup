#pragma once

#include "alloc_raii.hpp"
#include "module.hpp"

namespace soup
{
	struct bytepatch_external
	{
		std::shared_ptr<module> mod;
		alloc_raii og;
		pointer p;
		size_t size;

		~bytepatch_external()
		{
			unpatch();
		}

		void patch(soup::pointer p, void* data, size_t size)
		{
			if (!og || this->size != size)
			{
				og = alloc_raii(size);
				mod->externalRead(p, og, size);
			}
			this->p = p;
			this->size = mod->externalWrite(p, data, size);
		}

		void unpatch()
		{
			if (og)
			{
				mod->externalWrite(p, og, size);
				forget();
			}
		}

		void forget()
		{
			og.release();
		}
	};
}
