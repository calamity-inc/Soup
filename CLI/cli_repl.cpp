#include "cli.hpp"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "os.hpp"
#include "string.hpp"
#include "Tempfile.hpp"

struct AppendableStringTracker
{
	size_t prev_len = 0;

	[[nodiscard]] std::string getNew(const std::string& str)
	{
		auto len = str.size();
		auto _new_len = (len - prev_len);
		if (_new_len != 0)
		{
			std::string _new = str.substr(prev_len, len - prev_len);
			prev_len = len;
			return _new;
		}
		return {};
	}
};

struct ReplState
{
	std::string code{};
	size_t line_offset = 8;
	AppendableStringTracker compile_output_tracker{};
	AppendableStringTracker execute_output_tracker{};
};

bool compile_and_run(ReplState& state)
{
	std::string program = R"EOC(
#include <iostream>
#include <string>
#include <vector>

int main()
{
)EOC";
	program.append(state.code);
	program.append("\nreturn 0;\n}");

	soup::Tempfile code_file("cpp");
	{
		std::ofstream f{ code_file };
		f << std::move(program);
	}

	soup::Tempfile exe_file("exe");

	bool success = true;
	auto compile_output = state.compile_output_tracker.getNew(soup::os::execute("clang", {
#if SOUP_WINDOWS
		"-std=c++20",
#else
		"-std=c++17",
		"-lstdc++",
		"-lstdc++fs",
#endif
		"-o", exe_file, code_file
	}));
	if (!compile_output.empty()) // BUG: Compile output is truncated too much when producing an error if a previous line causes a warning.
	{
		std::string line_name = code_file;
		line_name.push_back(':');
		line_name.append(std::to_string(state.line_offset));
		soup::string::replace_all(compile_output, line_name, "[code]");
		soup::string::replace_all(compile_output, code_file, "[repl]");
		std::cout << compile_output;
		success = (compile_output.find(": error: ") == std::string::npos);
	}
	if (success)
	{
		auto execute_output = state.execute_output_tracker.getNew(soup::os::execute(exe_file));
		if (!execute_output.empty())
		{
			std::cout << execute_output << std::endl;
		}
		return true;
	}
	return false;
}

void cli_repl()
{
	std::cout << "C++ Read-Eval-Print Loop (REPL)\n$ " << std::flush;

	ReplState stable_state{};

	std::string line;
	while (std::getline(std::cin, line))
	{
		ReplState state = stable_state;
		state.code.append(line);
		state.code.push_back('\n');
		if (compile_and_run(state))
		{
			++state.line_offset;
			stable_state = std::move(state);
		}
		std::cout << "$ " << std::flush;
	}
}
