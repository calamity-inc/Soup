#pragma once

#include "poly.hpp"
#include <vector>

namespace soup
{
	struct poly_vector : public std::vector<poly>
	{
		void subdivide()
		{
			poly_vector tmp{};
			tmp.reserve(size() * 2);
			for (const auto& p : *this)
			{
				const vector3* edge1;
				const vector3* edge2;
				const vector3* edge3;

				float a_b_len = p.a.distance(p.b);
				float a_c_len = p.a.distance(p.c);
				float b_c_len = p.b.distance(p.c);

				if (a_b_len > a_c_len && a_b_len > b_c_len)
				{
					edge1 = &p.a;
					edge2 = &p.b;
					edge3 = &p.c;
				}
				else if (a_c_len > a_b_len && a_b_len > b_c_len)
				{
					edge1 = &p.a;
					edge2 = &p.c;
					edge3 = &p.b;
				}
				else // assuming b..c
				{
					edge1 = &p.b;
					edge2 = &p.c;
					edge3 = &p.a;
				}
				vector3 d = ((*edge1 + *edge2) / 2.0f);
				const auto normal = p.getSurfaceNormal();
				poly p1{ *edge3, *edge1, d };
				if (p1.getSurfaceNormal().distance(normal) < 0.1f)
				{
					tmp.emplace_back(p1);
				}
				else
				{
					tmp.emplace_back(poly{ *edge3, *edge2, d });
				}
				poly p2{ *edge3, d, *edge2 };
				if (p2.getSurfaceNormal().distance(normal) < 0.1f)
				{
					tmp.emplace_back(p2);
				}
				else
				{
					tmp.emplace_back(poly{ *edge3, d, *edge1 });
				}
			}
			*this = std::move(tmp);
		}
	};
}
