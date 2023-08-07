#pragma once

#include <atomic>
#include <memory> // destroy_at

#include "Exception.hpp"
#include "memory.hpp" // construct_at
#include "type_traits.hpp"

#ifndef SOUP_DEBUG_SHAREDPTR 
#define SOUP_DEBUG_SHAREDPTR false
#endif

#if SOUP_DEBUG_SHAREDPTR
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
#if SOUP_DEBUG_SHAREDPTR
		inline static std::unordered_set<T*> managed_instances{};
#endif

		struct Data
		{
			T* inst;
			std::atomic_uint refcount;
			bool was_created_with_make_shared = false;

			Data(T* inst)
				: inst(inst), refcount(1)
			{
			}

			void incref()
			{
#if SOUP_DEBUG_SHAREDPTR
				const auto preinc = refcount.load();
				if (preinc == 0)
				{
					__debugbreak(); // Attempt to revive the dead
				}
#endif
				++refcount;
#if SOUP_DEBUG_SHAREDPTR
				std::cout << (void*)inst << " :: Increment to " << refcount.load() << " from " << preinc << "\n";
#endif
			}

			void decref()
			{
#if SOUP_DEBUG_SHAREDPTR
				const auto predec = refcount.load();
#endif
				if (--refcount == 0)
				{
#if SOUP_DEBUG_SHAREDPTR
					std::cout << (void*)inst << " :: No more references\n";
					managed_instances.erase(inst);
#endif
					if (was_created_with_make_shared)
					{
						const auto inst = this->inst;
						std::destroy_at<>(inst);
						std::destroy_at<>(this);
						::operator delete(reinterpret_cast<void*>(inst)/*, sizeof(T) + sizeof(typename SharedPtr<T>::Data)*/);
					}
					else
					{
						delete inst;
						delete this;
					}
				}
#if SOUP_DEBUG_SHAREDPTR
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

		SharedPtr(Data* data)
			: data(data)
		{
		}

		SharedPtr(T* inst)
			: data(new Data(inst))
		{
#if SOUP_DEBUG_SHAREDPTR
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
			if (data != nullptr)
			{
				data->incref();
			}
		}

		SharedPtr(SharedPtr<T>&& b)
			: data(b.data)
		{
			b.data = nullptr;
		}

		// reinterpret_cast<Data*> is okay because it contains a pointer to the actual instance, so it's all the same regardless of T.

		template <typename T2, SOUP_RESTRICT(std::is_base_of_v<T, T2> || std::is_base_of_v<T2, T>)>
		SharedPtr(const SharedPtr<T2>& b) noexcept
			: data(reinterpret_cast<Data*>(b.data))
		{
			if (data != nullptr)
			{
				data->incref();
			}
		}

		template <typename T2, SOUP_RESTRICT(std::is_base_of_v<T, T2> || std::is_base_of_v<T2, T>)>
		SharedPtr(SharedPtr<T2>&& b) noexcept
			: data(reinterpret_cast<Data*>(b.data))
		{
			b.data = nullptr;
		}

		void operator=(const SharedPtr<T>& b)
		{
			const auto prev_data = this->data;
			this->data = b.data;
			if (this->data != nullptr)
			{
				this->data->incref();
			}
			if (prev_data != nullptr)
			{
				prev_data->decref();
			}
		}

		void operator=(SharedPtr<T>&& b)
		{
			const auto prev_data = this->data;
			this->data = b.data;
			b.data = nullptr;
			if (prev_data != nullptr)
			{
				prev_data->decref();
			}
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
			const auto data = this->data;
			if (data != nullptr)
			{
				this->data = nullptr;
				data->decref();
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
			if (data)
			{
				return data->inst;
			}
			return nullptr;
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
			const auto data = this->data;
			this->data = nullptr;
			if (data->refcount.load() != 1)
			{
				this->data = data;
				throw Exception("Attempt to release SharedPtr with more than 1 reference");
			}
			T* const inst = data->inst;
			const auto was_created_with_make_shared = data->was_created_with_make_shared;
			std::destroy_at<>(data);
			if (was_created_with_make_shared)
			{
				// data will continue to be allocated behind the instance, but once the instance is free'd, data is also free'd.
			}
			else
			{
				::operator delete(reinterpret_cast<void*>(data));
			}
			return inst;
		}
	};

	template <typename T, typename...Args, SOUP_RESTRICT(!std::is_array_v<T>)>
	[[nodiscard]] SharedPtr<T> make_shared(Args&&...args)
	{
		void* const b = ::operator new(sizeof(T) + sizeof(typename SharedPtr<T>::Data));
		typename SharedPtr<T>::Data* data;
		try
		{
			auto inst = soup::construct_at<>(reinterpret_cast<T*>(b), std::forward<Args>(args)...);
			data = soup::construct_at<>(reinterpret_cast<typename SharedPtr<T>::Data*>(reinterpret_cast<uintptr_t>(b) + sizeof(T)), inst);
		}
		catch (...)
		{
			::operator delete(b);
			std::rethrow_exception(std::current_exception());
		}
		data->was_created_with_make_shared = true;
		return SharedPtr<T>(data);
	}
}
