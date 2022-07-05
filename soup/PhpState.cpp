#include "PhpState.hpp"

#include "LangDesc.hpp"
#include "LangVm.hpp"
#include "ParseError.hpp"
#include "ParserState.hpp"
#include "parse_tree.hpp"
#include "string.hpp"
#include "StringReader.hpp"
#include "StringWriter.hpp"
#include "UniquePtr.hpp"

namespace soup
{
	enum PhpOpCodes : int
	{
		OP_CONCAT = 0,
		OP_INDEX,
		OP_ASSIGN,
		OP_EQ,
		OP_IF,
		OP_IF_ELSE,
		OP_CALL,
		OP_REQUIRE,
		OP_ECHO,
	};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wstring-compare"
	[[nodiscard]] static LangDesc getLangDescImpl()
	{
		LangDesc ld;
		ld.addToken("<?php");
		ld.addToken("?>");
		ld.addBlock("{", "}");
		ld.addToken("function", Rgb::BLUE, [](ParserState& ps)
		{
			auto node = ps.popRighthand();
			if (node->type != ast::Node::LEXEME)
			{
				std::string err = "'function' expected righthand '(' or name, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}
			UniquePtr<ast::Node> var_name_literal{};
			if (reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.token_keyword == Lexeme::LITERAL)
			{
				var_name_literal = std::move(node);
				node = ps.popRighthand();
			}
			if (reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.token_keyword != "(")
			{
				std::string err = "'function' expected righthand '(', found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}

			node = ps.popRighthand();
			if (node->type != ast::Node::LEXEME
				|| reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.token_keyword != Lexeme::LITERAL
				)
			{
				std::string err = "'function' expected righthand ')' or parameter list, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}
			std::vector<UniquePtr<ast::Node>> param_literals{};
			if (reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.val.getString() != ")")
			{
				while (true)
				{
					param_literals.emplace_back(std::move(node));

					node = ps.popRighthand();
					if (node->type != ast::Node::LEXEME
						|| !reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral(",")
						)
					{
						if (node->type == ast::Node::LEXEME
							&& reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral(")")
							)
						{
							break;
						}
						std::string err = "Parameter list expected righthand ',', found ";
						err.append(node->toString());
						throw ParseError(std::move(err));
					}

					node = ps.popRighthand();
					if (node->type != ast::Node::LEXEME
						|| reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.token_keyword != Lexeme::LITERAL
						)
					{
						std::string err = "Parameter list expected righthand parameter after ',', found ";
						err.append(node->toString());
						throw ParseError(std::move(err));
					}
				}
			}

			node = ps.popRighthand();
			if (node->type != ast::Node::BLOCK)
			{
				std::string err = "'function' expected righthand block, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}

			reinterpret_cast<ast::Block*>(node.get())->param_literals = std::move(param_literals);
			if (var_name_literal)
			{
				ps.setOp(OP_ASSIGN);
				ps.pushArgNode(std::move(var_name_literal));
				ps.pushArg(reinterpret_cast<ast::Block*>(node.release()));
			}
			else
			{
				ps.pushLefthand(reinterpret_cast<ast::Block*>(node.release()));
			}
		});
		ld.addToken("[", [](ParserState& ps)
		{
			ps.setOp(OP_INDEX);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();

			if (auto node = ps.popRighthand();
				node->type != ast::Node::LEXEME
				|| !reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral("]")
				)
			{
				std::string err = "'[' expected ']' after index/key, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}
		});
		ld.addToken(".", Rgb::RED, [](ParserState& ps)
		{
			ps.setOp(OP_CONCAT);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});
		ld.addToken("=", Rgb::RED, [](ParserState& ps)
		{
			ps.setOp(OP_ASSIGN);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});
		ld.addToken("==", Rgb::RED, [](ParserState& ps)
		{
			ps.setOp(OP_EQ);
			ps.consumeLefthandValue();
			ps.consumeRighthandValue();
		});
		ld.addToken("if", Rgb::RED, [](ParserState& ps)
		{
			ps.setOp(OP_IF);
			auto node = ps.popRighthand();
			if (node->type != ast::Node::LEXEME
				|| reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.token_keyword != "("
				)
			{
				std::string err = "'if' expected righthand '(', found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}

			ps.consumeRighthandValue(); // condition

			node = ps.popRighthand();
			if (node->type != ast::Node::LEXEME
				|| !reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral(")")
				)
			{
				std::string err = "'if(cond' expected righthand ')', found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}

			node = ps.popRighthand();
			if (node->type != ast::Node::BLOCK)
			{
				std::string err = "'if(cond)' expected righthand block, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}
			ps.pushArg(reinterpret_cast<ast::Block*>(node.release()));
		});
		ld.addToken("else", Rgb::RED, [](ParserState& ps)
		{
			auto node = ps.popLefthand();
			if (node->type != ast::Node::OP
				|| reinterpret_cast<ast::OpNode*>(node.get())->op.type != OP_IF
				)
			{
				std::string err = "'else' expected lefthand OP_IF, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}
			ps.setOp(OP_IF_ELSE);
			ps.setArgs(std::move(reinterpret_cast<ast::OpNode*>(node.get())->op.args));

			node = ps.popRighthand();
			if (node->type != ast::Node::BLOCK)
			{
				std::string err = "'else' expected righthand block, found ";
				err.append(node->toString());
				throw ParseError(std::move(err));
			}
			ps.pushArg(reinterpret_cast<ast::Block*>(node.release()));
		});
		ld.addToken("(", [](ParserState& ps)
		{
			ps.setOp(OP_CALL);
			ps.consumeLefthandValue();

			std::vector<UniquePtr<ast::Node>> arg_nodes{};

			auto node = ps.popRighthand();
			if (node->type != ast::Node::LEXEME
				|| !reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral(")")
				)
			{
				while (true)
				{
					if (!node->isValue())
					{
						std::string err = "Argument list expected value, found ";
						err.append(node->toString());
						throw ParseError(std::move(err));
					}
					arg_nodes.emplace_back(std::move(node));

					node = ps.popRighthand();
					if (node->type != ast::Node::LEXEME
						|| !reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral(",")
						)
					{
						if (node->type == ast::Node::LEXEME
							&& reinterpret_cast<ast::LexemeNode*>(node.get())->lexeme.isLiteral(")")
							)
						{
							break;
						}
						std::string err = "Argument list expected righthand ',', found ";
						err.append(node->toString());
						throw ParseError(std::move(err));
					}

					node = ps.popRighthand();
				}
			}
			
			ps.pushArg(arg_nodes.size());
			for (auto& node : arg_nodes)
			{
				ps.pushArgNode(std::move(node));
			}
		});
		ld.addToken("require", Rgb::RED, [](ParserState& ps)
		{
			ps.setOp(OP_REQUIRE);
			ps.consumeRighthandValue();
		});
		ld.addToken("echo", Rgb::BLUE, [](ParserState& ps)
		{
			ps.setOp(OP_ECHO);
			ps.consumeRighthandValue();
		});
		return ld;
	}

	const LangDesc& PhpState::getLangDesc()
	{
		static LangDesc ld = getLangDescImpl();
		return ld;
	}

	static void processNonPhpmodeBuffer(std::vector<Lexeme>& ls, std::vector<Lexeme>::iterator& i, std::string& non_phpmode_buffer)
	{
		if (!non_phpmode_buffer.empty())
		{
			i = ls.insert(i, Lexeme{ Lexeme::VAL, std::move(non_phpmode_buffer) });
			i = ls.insert(i, Lexeme{ "echo" });
			non_phpmode_buffer.clear();
		}
	}

	void PhpState::processPhpmode(std::vector<Lexeme>& ls)
	{
		std::string non_phpmode_buffer{};
		auto i = ls.begin();
		for (; i != ls.end(); )
		{
			if (i->token_keyword == "<?php")
			{
				i = ls.erase(i);
				processNonPhpmodeBuffer(ls, i, non_phpmode_buffer);
				for (; i != ls.end(); )
				{
					if (i->token_keyword == "?>")
					{
						i = ls.erase(i);
						break;
					}
					++i;
				}
			}
			else
			{
				non_phpmode_buffer.append(i->getSourceString());
				i = ls.erase(i);
			}
		}
		processNonPhpmodeBuffer(ls, i, non_phpmode_buffer);
	}
#pragma clang diagnostic pop

	std::string PhpState::evaluate(const std::string& code, unsigned int max_require_depth) const
	{
		std::string output{};
		try
		{
			auto ld = getLangDesc();
			auto ls = ld.tokenise(code);
			processPhpmode(ls);
			ld.eraseNlTerminatedComments(ls, "//");
			ld.eraseSpace(ls);
			auto b = ld.parseImpl(ls);

			StringWriter w;
			b.compile(w);

			StringReader r{ std::move(w.str) };
			execute(output, r, max_require_depth);
		}
		catch (const std::runtime_error& e)
		{
			if (!output.empty())
			{
				output.push_back('\n');
			}
			output.append("ERROR: ");
			output.append(e.what());
		}
		return output;
	}

	void PhpState::execute(std::string& output, Reader& r, unsigned int max_require_depth, std::stack<std::shared_ptr<Mixed>>&& stack) const
	{
		LangVm vm(&r, std::move(stack));

		std::unordered_map<Mixed, std::shared_ptr<Mixed>> _SERVER{};
		_SERVER.emplace("REQUEST_URI", std::make_shared<Mixed>(request_uri));

		vm.vars.emplace("$_SERVER", std::make_shared<Mixed>(std::move(_SERVER)));

		for (uint8_t op; vm.getNextOp(op); )
		{
			switch (op)
			{
			case OP_CONCAT:
				{
					std::string str = vm.pop()->toString();
					str.append(vm.pop()->toString());
					vm.push(std::move(str));
				}
				break;

			case OP_INDEX:
				{
					auto arr = vm.pop();
					auto key = vm.pop();
					if (auto e = arr->getMixedSpMixedMap().find(*key); e != arr->getMixedSpMixedMap().end())
					{
						vm.push(e->second);
					}
					else
					{
						std::string err = "Array has no entry with key ";
						err.append(key->toString());
						throw ParseError(std::move(err));
					}
				}
				break;

			case OP_ASSIGN:
				{
					auto& var = vm.popVarRef();
					var = vm.pop();
				}
				break;

			case OP_EQ:
				vm.push(vm.pop()->toString() == vm.pop()->toString());
				break;

			case OP_IF:
				{
					auto cond_val = vm.pop()->getInt();
					auto true_sr = vm.popFunc();
					if (cond_val)
					{
						execute(output, true_sr, max_require_depth);
					}
				}
				break;

			case OP_IF_ELSE:
				{
					auto cond_val = vm.pop()->getInt();
					auto true_sr = vm.popFunc();
					auto false_sr = vm.popFunc();
					if (cond_val)
					{
						execute(output, true_sr, max_require_depth);
					}
					else
					{
						execute(output, false_sr, max_require_depth);
					}
				}
				break;

			case OP_CALL:
				{
					auto sr = vm.popFunc();
					auto num_args = vm.popRaw()->getUInt();
					std::vector<std::shared_ptr<Mixed>> args{};
					args.reserve(num_args);
					while (num_args--)
					{
						args.emplace_back(vm.pop());
					}
					std::stack<std::shared_ptr<Mixed>> handover_stack{};
					for (auto i = args.rbegin(); i != args.rend(); ++i)
					{
						handover_stack.emplace(std::move(*i));
					}
					execute(output, sr, max_require_depth, std::move(handover_stack));
				}
				break;

			case OP_REQUIRE:
				{
					if (max_require_depth == 0)
					{
						throw std::runtime_error("Max require depth exceeded");
					}
					std::filesystem::path file = cwd;
					file /= vm.popString();
					if (!std::filesystem::exists(file))
					{
						std::string err = "Required file doesn't exist: ";
						err.append(file.string());
						throw std::runtime_error(std::move(err));
					}
					output.append(evaluate(string::fromFile(file.string()), max_require_depth - 1));
				}
				break;

			case OP_ECHO:
				{
					output.append(vm.pop()->toString());
				}
				break;
			}
		}
	}
}
