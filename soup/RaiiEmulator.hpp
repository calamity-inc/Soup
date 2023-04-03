#pragma once

#include <deque>
#include <vector>

#include "deleter.hpp"
#include "Exception.hpp"

namespace soup
{
	// For when your "favourite" web scripting language is missing important OOP features.
	struct RaiiEmulator
	{
		struct Object
		{
			void* addr;
			deleter_t deleter;

			template <typename T>
			Object(T* inst)
				: addr(inst), deleter(&deleter_impl<T>)
			{
			}

			Object(Object&& b) noexcept
				: addr(b.addr), deleter(b.deleter)
			{
				b.addr = nullptr;
			}

			void operator=(Object&& b) noexcept
			{
				addr = b.addr;
				deleter = b.deleter;
				b.addr = nullptr;
			}

			~Object()
			{
				if (addr != nullptr)
				{
					deleter(addr);
				}
			}
		};

		struct Scope
		{
			std::vector<Object> objects{};

			template <typename T>
			T* add(T* inst)
			{
				objects.emplace_back(inst);
				return inst;
			}

			[[nodiscard]] std::vector<Object>::iterator find(void* inst) noexcept
			{
				auto it = objects.begin();
				for (; it != objects.end(); ++it)
				{
					if (it->addr == inst)
					{
						break;
					}
				}
				return it;
			}
		};

		std::deque<Scope> scopes{};

		RaiiEmulator()
		{			
			beginScope(); // Global scope
		}

		void beginScope()
		{
			scopes.push_front(Scope{});
		}

		void endScope()
		{
			SOUP_IF_UNLIKELY (scopes.size() == 1) // Only the global scope active?
			{
				throw Exception("endScope called while no scope is active");
			}
			scopes.pop_front();
		}

		[[nodiscard]] Scope& currentScope() noexcept // Can't throw because global scope is always valid
		{
			return scopes.front();
		}

		template <typename T>
		[[nodiscard]] T* add(T&& inst)
		{
			return add(new T(std::move(inst)));
		}

		template <typename T>
		T* add(T* inst)
		{
			return currentScope().add(inst);
		}

		void broadenScope(void* inst)
		{
			for (size_t i = 0; i != scopes.size(); ++i)
			{
				if (auto e = scopes.at(i).find(inst); e != scopes.at(i).objects.end())
				{
					SOUP_IF_UNLIKELY (i + 1 >= scopes.size())
					{
						throw Exception("Attempt to broaden scope beyond global scope.");
					}
					scopes.at(i + 1).objects.emplace_back(std::move(*e));
					scopes.at(i).objects.erase(e);
					return;
				}
			}
			throw Exception("Can't broaden scope of an instance I don't manage");
		}

		void free(void* inst)
		{
			for (auto& scope : scopes)
			{
				if (auto e = scope.find(inst); e != scope.objects.end())
				{
					scope.objects.erase(e);
					return;
				}
			}
			throw Exception("Can't free an instance I don't manage (double-free?)");
		}
	};
}
