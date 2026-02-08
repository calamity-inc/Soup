#pragma once

#include <stack>

#include "Exception.hpp"

NAMESPACE_SOUP
{
	template <typename T>
	struct Stack : public std::stack<T>
	{
		T& top()
		{
			SOUP_IF_UNLIKELY (this->empty())
			{
				SOUP_THROW(Exception("attempt to get top of an empty stack"));
			}
			return std::stack<T>::top();
		}

		const T& top() const
		{
			SOUP_IF_UNLIKELY (this->empty())
			{
				SOUP_THROW(Exception("attempt to get top of an empty stack"));
			}
			return std::stack<T>::top();
		}
	};
}
