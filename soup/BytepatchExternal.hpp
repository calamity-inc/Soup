#pragma once

#include "AllocRaii.hpp"
#include "Module.hpp"

NAMESPACE_SOUP
{
	struct BytepatchExternal
	{
		std::shared_ptr<Module> mod;
		AllocRaii og;
		Pointer p;
		size_t size;

		~BytepatchExternal()
		{
			unpatch();
		}

		void patch(Pointer p, void* data, size_t size)
		{
			if (!og || this->size != size)
			{
				og = AllocRaii(size);
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
