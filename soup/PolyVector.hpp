#pragma once

#include "Poly.hpp"
#include <vector>

NAMESPACE_SOUP
{
	struct PolyVector : public std::vector<Poly>
	{
		void subdivide()
		{
			PolyVector tmp{};
			tmp.reserve(size() * 2);
			for (const auto& p : *this)
			{
				const Vector3* edge1;
				const Vector3* edge2;
				const Vector3* edge3;

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
				Vector3 d = ((*edge1 + *edge2) / 2.0f);
				const auto normal = p.getSurfaceNormal();
				Poly p1{ *edge3, *edge1, d };
				if (p1.getSurfaceNormal().distance(normal) < 0.1f)
				{
					tmp.emplace_back(p1);
				}
				else
				{
					tmp.emplace_back(Poly{ *edge3, *edge2, d });
				}
				Poly p2{ *edge3, d, *edge2 };
				if (p2.getSurfaceNormal().distance(normal) < 0.1f)
				{
					tmp.emplace_back(p2);
				}
				else
				{
					tmp.emplace_back(Poly{ *edge3, d, *edge1 });
				}
			}
			*this = std::move(tmp);
		}
	};
}
