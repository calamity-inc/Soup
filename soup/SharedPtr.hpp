#pragma once

#include <atomic>

#include "Exception.hpp"
#include "type_traits.hpp"

#define DEBUG_SHAREDPTR false

#if DEBUG_SHAREDPTR
#include <iostream>
#include <unordered_set>
#endif

namespace soup
{
	// Why not std::shared_ptr?
	// - Stores a deleter despite being templated
	// - Produces RTTI even with RTTI off
	// - No release method
	template <typename T>
	class SharedPtr
	{
	public:
#if DEBUG_SHAREDPTR
		inline static std::unordered_set<T*> managed_instances{};
#endif

		struct Data
		{
			T* inst;
			std::atomic_size_t refcount;

			Data(T* inst)
				: inst(inst), refcount(1)
			{
			}

			void incref()
			{
#if DEBUG_SHAREDPTR
				const auto preinc = refcount.load();
				if (preinc == 0)
				{
					__debugbreak(); // Attempt to revive the dead
				}
#endif
				++refcount;
#if DEBUG_SHAREDPTR
				std::cout << (void*)inst << " :: Increment to " << refcount.load() << " from " << preinc << "\n";
#endif
			}

			void decref()
			{
#if DEBUG_SHAREDPTR
				const auto predec = refcount.load();
#endif
				if (--refcount == 0)
				{
#if DEBUG_SHAREDPTR
					std::cout << (void*)inst << " :: No more references\n";
					managed_instances.erase(inst);
#endif
					delete inst;
					delete this;
				}
#if DEBUG_SHAREDPTR
				else
				{
					std::cout << (void*)inst << " :: Decrement to " << refcount.load() << " from " << predec << "\n";
				}
#endif
			}
		};

		Data* data;

		SharedPtr()
			: data(nullptr)
		{
		}

		SharedPtr(T* inst)
			: data(new Data(inst))
		{
#if DEBUG_SHAREDPTR
			if (managed_instances.contains(data->inst))
			{
				__debugbreak(); // Already managed by another SharedPtr instance
			}
			managed_instances.emplace(data->inst);
			std::cout << (void*)data->inst << " :: New SharedPtr\n";
#endif
		}

		SharedPtr(const SharedPtr<T>& b)
			: data(b.data)
		{
			data->incref();
		}

		SharedPtr(SharedPtr<T>&& b)
			: data(b.data)
		{
			b.data = nullptr;
		}

		void operator=(const SharedPtr<T>& b)
		{
			reset();
			data = b.data;
			data->incref();
		}

		void operator=(SharedPtr<T>&& b)
		{
			reset();
			data = b.data;
			b.data = nullptr;
		}

		~SharedPtr()
		{
			if (data != nullptr)
			{
				data->decref();
			}
		}

		void reset()
		{
			if (data != nullptr)
			{
				data->decref();
				data = nullptr;
			}
		}

		[[nodiscard]] operator bool() const noexcept
		{
			return data != nullptr;
		}

		[[nodiscard]] operator T* () const noexcept
		{
			return get();
		}

		[[nodiscard]] T* get() const noexcept
		{
			return data->inst;
		}

		[[nodiscard]] T& operator*() const noexcept
		{
			return *get();
		}

		[[nodiscard]] T* operator->() const noexcept
		{
			return get();
		}

		[[nodiscard]] size_t getRefCount() const noexcept
		{
			return data->refcount.load();
		}

		[[nodiscard]] T* release()
		{
			if (getRefCount() != 1)
			{
				throw Exception("Attempt to release SharedPtr with more than 1 reference");
			}
			T* res = get();
			delete data;
			data = nullptr;
			return res;
		}
	};

	template <typename T, typename...Args, SOUP_RESTRICT(!std::is_array_v<T>)>
	[[nodiscard]] SharedPtr<T> make_shared(Args&&...args)
	{
		return SharedPtr<T>(new T(std::forward<Args>(args)...));
	}
}
