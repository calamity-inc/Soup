#pragma once

#include "base.hpp"
#if SOUP_EXCEPTIONS

#include <string>

#include "code_tree.hpp"
#include "SourceLocation.hpp"

#define unit(name) SOUP_CODE_TREE_NODE(::soup::TestUnit, name)
extern void test(const char* name, void(*test)());
#undef assert
#define assert(...) assert_impl((__VA_ARGS__), SOUP_CAPTURE_SOURCE_LOCATION);

extern void assert_impl(bool b, soup::SourceLocation sl);

NAMESPACE_SOUP
{
	struct AssertionFailed { };

	struct TestUnit : public CodeTreeNode<TestUnit>
	{
		const char* const name;
		const bool is_test;

		inline static thread_local size_t failed_tests;
		inline static thread_local size_t total_tests;

		TestUnit(const char* name, bool is_test = false)
			: CodeTreeNode(), name(name), is_test(is_test)
		{
		}

		void onFinishedBuildingTree();
		void runTests();
		void truncateSuccessfulTests();
		void printout(std::string prefix = {}) const;
	};

	struct Test : public TestUnit
	{
		void(*test)();
		SourceLocation last_successful_test{};
		std::string err{};

		inline static thread_local Test* currently_running = nullptr;

		Test(const char* name, void(*test)())
			: TestUnit(name, true), test(test)
		{
		}

		void setException(std::string&& msg);
	};
}
#endif
