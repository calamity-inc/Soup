#include "aglTranspiler.hpp"

namespace soup
{
    std::string aglTranspiler::getOutput() const
    {
        std::string out;
        if (!includes.empty())
        {
            for (const auto& include : includes)
            {
                out.append("#include ");
                out.append(include);
                out.push_back('\n');
            }
			out.push_back('\n');
		}
        out.append(body);
        return out;
    }

    void aglTranspiler::indent()
    {
        body.append(indentation, '\t');
    }
}
