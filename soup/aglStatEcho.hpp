#pragma once

#include "aglStat.hpp"

#include <string>

namespace soup
{
	struct aglStatEcho : public aglStat
	{
		std::string arg;

		aglStatEcho(std::string arg)
			: arg(std::move(arg))
		{
		}

		void transpile(aglTranspiler& t) const final
		{
			t.indent();
			if (t.target == AGL_CPP)
			{
				t.includes.emplace("<iostream>");
				t.body.append("std::cout << ");
			}
			else
			{
				t.body.append("echo ");
			}
			t.body.append(arg);
			t.body.append(";\n");
		}
	};
}
