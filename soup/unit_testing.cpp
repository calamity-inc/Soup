#include "unit_testing.hpp"

#if SOUP_EXCEPTIONS

#include <iostream>
#include <stdexcept>

using namespace soup;

void test(const char* name, void(*test)())
{
	new soup::Test(name, test);
	soup::Test::finishCurrentNode();
}

void assert_impl(bool b, soup::SourceLocation sl)
{
	if (Test::currently_running == nullptr)
	{
		throw std::runtime_error("Can't use assert outside of a test");
	}
	if (!b)
	{
		Test::currently_running->err = "Assertion in ";
		Test::currently_running->err.append(sl.file_name);
		Test::currently_running->err.append(" on line ");
		Test::currently_running->err.append(std::to_string(sl.line));
		Test::currently_running->err.append(" failed");
		throw AssertionFailed();
	}
	Test::currently_running->last_successful_test = std::move(sl);
}

namespace soup
{
	void TestUnit::onFinishedBuildingTree()
	{
		// Note: Tree should be free'd by this function, but we exit, so who cares
		failed_tests = 0;
		total_tests = 0;
		runTests();
		if (failed_tests == 0)
		{
			std::cout << "All " << total_tests << " tests ran successfully!";
			exit(0);
		}
		std::cout << failed_tests << " / " << total_tests << " tests have failed.\n\n";
		truncateSuccessfulTests();
		printout();
		exit(1);
	}

	void TestUnit::runTests()
	{
		if (is_test)
		{
			try
			{
				Test::currently_running = static_cast<Test*>(this);
				Test::currently_running->test();
			}
			catch (const AssertionFailed&)
			{
				++failed_tests;
			}
			catch (const std::exception& e)
			{
				++failed_tests;
				static_cast<Test*>(this)->setException(e.what());
			}
			catch (...)
			{
				++failed_tests;
				static_cast<Test*>(this)->setException("exception occurred");
			}
			++total_tests;
			Test::currently_running = nullptr;
		}
		for (const auto& child : children)
		{
			child->runTests();
		}
	}

	void TestUnit::truncateSuccessfulTests()
	{
		for (auto i = children.begin(); i != children.end(); )
		{
			if ((*i)->is_test)
			{
				if (static_cast<Test*>(*i)->err.empty())
				{
					//delete *i; // who cares? we're gonna exit soon anyway
					i = children.erase(i);
				}
				else
				{
					++i;
				}
			}
			else
			{
				(*i)->truncateSuccessfulTests();
				if ((*i)->children.empty())
				{
					i = children.erase(i);
				}
				else
				{
					++i;
				}
			}
		}
	}

	void TestUnit::printout(std::string prefix) const
	{
		std::cout << prefix << name;
		if (is_test)
		{
			std::cout << ": " << static_cast<const Test*>(this)->err;
		}
		std::cout << "\n";
		prefix.append("  ");
		for (const auto& child : children)
		{
			child->printout(prefix);
		}
	}

	void Test::setException(std::string&& msg)
	{
		err = std::move(msg);
		if (last_successful_test.isValid())
		{
			err.append(" after line ");
			err.append(std::to_string(last_successful_test.line));
		}
	}
}

#endif
