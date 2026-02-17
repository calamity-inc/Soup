#include "cli.hpp"

#include <iostream>
#include <thread>

#include <audDevice.hpp>
#include <audPlayback.hpp>
#include <Canvas.hpp>
#include <Chatbot.hpp>
#include <ChessCli.hpp>
#include <CompiledExecutable.hpp>
#include <country_names.hpp>
#include <console.hpp>
#include <dhcp.hpp>
#include <Editor.hpp>
#include <FileReader.hpp>
#include <HttpRequest.hpp>
#include <hwGamepad.hpp>
#include <hwHid.hpp>
#include <json.hpp>
#include <main.hpp>
#include <netIntel.hpp>
#include <netIntrospectTask.hpp>
#include <os.hpp>
#include <QrCode.hpp>
#include <riff.hpp>
#include <string.hpp>
#include <unicode.hpp>
#include <Uri.hpp>
#include <wasm.hpp>
#include <WavFmtChunk.hpp>

using namespace soup;

int entry(std::vector<std::string>&& args, bool)
{
	if (args.size() > 1)
	{
		std::string subcommand = args[1];
		string::lower(subcommand);

		if (subcommand == "3d")
		{
			cli_3d();
			return 0;
		}

		if (subcommand == "bench")
		{
			cli_bench();
			return 0;
		}

		if (subcommand == "cat2json")
		{
			return cli_cat2json(args.size(), args.data());
		}

#ifdef SOUP_ENABLE_CHATBOT
		if (subcommand == "chatbot")
		{
			while (true)
			{
				console.init(false);
				std::cout << "> ";
				std::string line;
				if (!std::getline(std::cin, line))
				{
					break;
				}
				try
				{
					auto res = Chatbot::process(line);
					std::cout << res.response << std::endl;
					if (res.type == CB_RES_IMAGE)
					{
						std::cout << res.extra.getCanvas().toStringDownsampledDoublewidthUtf8(true, true);
					}
				}
				catch (std::exception& e)
				{
					std::cout << "Error: " << e.what() << std::endl;
				}
			}
			return 0;
		}
#endif

		if (subcommand == "chatgpt")
		{
			if (args.size() <= 2)
			{
				std::cout << "Syntax: soup chatgpt <token> [model]" << std::endl;
				return 1;
			}
			cli_chatgpt(args.size() - 2, &args[2]);
			return 0;
		}

#ifdef SOUP_ENABLE_CHESS
		if (subcommand == "chess")
		{
			ChessCli cc{};
			if (args.size() > 2)
			{
				cc.board.loadFen(args[2]);
			}
			cc.run();
			return 0;
		}
#endif

		if (subcommand == "datareflection")
		{
			cli_datareflection();
			return 0;
		}

		if (subcommand == "dhcp")
		{
			if (auto info = dhcp::requestInfo(); info.isValid())
			{
				std::cout << "Router \"" << info.server_name << "\" responded to DHCPInform and provided DNS server " << IpAddr(info.dns_server).toString() << "\n";
			}
			else
			{
				std::cout << "I screamed into the void, but the void did not respond. Possibly VPN network?\n";
			}
			return 0;
		}

		if (subcommand == "dig")
		{
			cli_dig(args.size() - 2, &args[2]);
			return 0;
		}

		if (subcommand == "dnsserver")
		{
			if (args.size() <= 2)
			{
				std::cout << "Syntax: soup dnsserver <file> [bind-ip]" << std::endl;
				return 1;
			}
			cli_dnsserver(args.size() - 2, &args[2]);
			return 0;
		}

		if (subcommand == "dvd")
		{
			cli_dvd();
			return 0;
		}

		if (subcommand == "edit")
		{
			if (args.size() <= 2)
			{
				std::cout << "Syntax: soup edit <files...>" << std::endl;
				return 1;
			}
			Editor edit{};
			for (int i = 2; i != args.size(); ++i)
			{
				auto& tab = edit.addTab(args[i], unicode::utf8_to_utf32(string::fromFile(args[i])));
				if (i == 2)
				{
					tab.setActive(edit);
				}
			}
			edit.run();
			return 0;
		}

		if (subcommand == "gamepad")
		{
			for (auto& gp : hwGamepad::getAll())
			{
				console.init(false);
				const std::string name = gp.name ? gp.name : gp.hid.getProductName();
				std::cout << name << " detected, awaiting input.\n";
				hwGamepad::Status prev_status{};
				while (true)
				{
					auto status = gp.receiveStatus();
					if (gp.disconnected)
					{
						std::cout << name << " disconnected.\n";
						return 0;
					}
					if (memcmp(&status, &prev_status, sizeof(hwGamepad::Status)) != 0)
					{
						prev_status = status;
						console.clearScreen();
						console.setCursorPos(0, 0);
						std::cout << name << "\n";
						std::cout << "Left Stick: " << status.left_stick_x << ", " << status.left_stick_y << "\n";
						std::cout << "Right Stick: " << status.right_stick_x << ", " << status.right_stick_y << "\n";
						if (gp.hasAnalogueTriggers())
						{
							std::cout << "Left Trigger: " << status.left_trigger << "\n";
							std::cout << "Right Trigger: " << status.right_trigger << "\n";
						}
						std::cout << "Buttons: ";
						std::vector<std::string> buttons{};
						if (status.buttons[BTN_DPAD_UP]) buttons.emplace_back("DPAD UP");
						if (status.buttons[BTN_DPAD_DOWN]) buttons.emplace_back("DPAD DOWN");
						if (status.buttons[BTN_DPAD_LEFT]) buttons.emplace_back("DPAD LEFT");
						if (status.buttons[BTN_DPAD_RIGHT]) buttons.emplace_back("DPAD RIGHT");
						if (status.buttons[BTN_ACT_DOWN]) buttons.emplace_back("ACT DOWN");
						if (status.buttons[BTN_ACT_RIGHT]) buttons.emplace_back("ACT RIGHT");
						if (status.buttons[BTN_ACT_LEFT]) buttons.emplace_back("ACT LEFT");
						if (status.buttons[BTN_ACT_UP]) buttons.emplace_back("ACT UP");
						if (status.buttons[BTN_LBUMPER]) buttons.emplace_back("L1");
						if (status.buttons[BTN_RBUMPER]) buttons.emplace_back("R1");
						if (status.buttons[BTN_LTRIGGER]) buttons.emplace_back("L2");
						if (status.buttons[BTN_RTRIGGER]) buttons.emplace_back("R2");
						if (status.buttons[BTN_LSTICK]) buttons.emplace_back("L3");
						if (status.buttons[BTN_RSTICK]) buttons.emplace_back("R3");
						if (status.buttons[BTN_META]) buttons.emplace_back("META");
						if (status.buttons[BTN_TOUCHPAD]) buttons.emplace_back("TOUCHPAD");
						if (status.buttons[BTN_SHARE]) buttons.emplace_back("SHARE");
						if (status.buttons[BTN_OPTIONS]) buttons.emplace_back("OPTIONS");
						if (status.buttons[BTN_MINUS]) buttons.emplace_back("MINUS");
						if (status.buttons[BTN_PLUS]) buttons.emplace_back("PLUS");
						if (status.buttons[BTN_ASSISTANT]) buttons.emplace_back("ASSISTANT");
						if (status.buttons[BTN_MENU]) buttons.emplace_back("MENU");
						if (buttons.empty())
						{
							std::cout << "None";
						}
						else
						{
							std::cout << string::join(buttons, ", ");
						}
						std::cout << "\n";
						if (gp.hasTouchpad())
						{
							std::cout << "Touchpad: ";
							std::vector<std::string> finger_coords{};
							for (uint8_t i = 0; i != status.num_fingers_on_touchpad; ++i)
							{
								std::string str = std::to_string(status.finger_coords[i].x);
								str.append(", ");
								str.append(std::to_string(status.finger_coords[i].y));
								finger_coords.emplace_back(std::move(str));
							}
							if (finger_coords.empty())
							{
								std::cout << "Inactive";
							}
							else
							{
								std::cout << string::join(finger_coords, "; ");
							}
							std::cout << "\n";
						}
						if (gp.canRumble())
						{
							std::cout << "Press DPAD UP+LEFT to rumble: Left trigger to actuate weak motor, right trigger to actuate strong motor.\n";
							if (status.buttons[BTN_DPAD_UP] && status.buttons[BTN_DPAD_LEFT])
							{
								gp.rumbleWeak(static_cast<uint8_t>(status.left_trigger * 255), 100);
								gp.rumbleStrong(static_cast<uint8_t>(status.right_trigger * 255), 100);
							}
						}
						if (gp.hasLight())
						{
							std::cout << "Press DPAD DOWN+RIGHT to change the light: Use right trigger to adjust hue and left trigger to adjust value.\n";
							if (status.buttons[BTN_DPAD_DOWN] && status.buttons[BTN_DPAD_RIGHT])
							{
								gp.setLight(Rgb::fromHsv(status.right_trigger * 360.0, 1.0, /*1.0 -*/ status.left_trigger));
							}
						}
						std::this_thread::sleep_for(std::chrono::milliseconds(7));
					}
				}
				break;
			}
			std::cout << "No gamepad detected.\n";
			return 0;
		}

		if (subcommand == "geoip")
		{
			IpAddr addr;
			if (args.size() != 3
				|| !addr.fromString(args[2])
				)
			{
				std::cout << "Syntax: soup geoip <ip>" << std::endl;
				return 1;
			}
			netIntel intel;
			if (addr.isV4())
			{
				std::cout << "Initialising netIntel for IPv4..." << std::endl;
				intel.init(true, false);
			}
			else
			{
				std::cout << "Initialising netIntel for IPv6..." << std::endl;
				intel.init(false, true);
			}
			if (auto loc = intel.getLocationByIp(addr))
			{
				if (loc->city)
				{
					std::cout << "City: " << loc->city << std::endl;
				}
				if (loc->state)
				{
					std::cout << "State: " << loc->state << std::endl;
				}
				if (auto cn = getCountryName(loc->country_code.c_str()); !cn.empty())
				{
					std::cout << "Country: " << cn << std::endl;
				}
				else
				{
					std::cout << "Country: " << loc->country_code.c_str() << std::endl;
				}
			}
			if (auto as = intel.getAsByIp(addr))
			{
				std::cout << "AS Number: " << as->number << "\n";
				std::cout << "AS Handle: " << as->handle << "\n";
				std::cout << "AS Name: " << as->name << "\n";
				std::cout << "AS is hosting provider? " << (as->isHosting(intel) ? "Yes" : "No") << "\n";
			}
			return 0;
		}

		if (subcommand == "hid")
		{
			std::cout << "VID\tPID\n";
			for (const auto& hid : hwHid::getAll())
			{
				std::cout << std::hex;
				std::cout << hid.vendor_id << "\t" << hid.product_id << "\t";
				if (auto name = hid.getProductName(); !name.empty())
				{
					std::cout << name;
				}
				else
				{
					if (auto vendor = hid.getManufacturerName(); !vendor.empty())
					{
						std::cout << vendor << " Device ";
					}
					std::cout << "[Product Name Not Provided]";
				}
				std::cout << "\n";
				std::cout << "\t\t- Path: " << hid.path << "\n";
				std::cout << "\t\t- Usage Page: " << hid.usage_page << "\n";
				std::cout << "\t\t- Usage: " << hid.usage << "\n";
				std::cout << "\t\t- Bus: " << (hid.isBluetooth() ? "BT" : "USB") << "\n";
				std::cout << std::dec;
				std::cout << "\t\t- Input Report Byte Length: " << hid.input_report_byte_length << "\n";
				std::cout << "\t\t- Output Report Byte Length: " << hid.output_report_byte_length << "\n";
				std::cout << "\t\t- Feature Report Byte Length: " << hid.feature_report_byte_length << "\n";
#if !SOUP_WINDOWS
				std::cout << "\t\t- Have Permission? " << (hid.havePermission() ? "Yes" : "No") << "\n";
#endif
				if (auto serial = hid.getSerialNumber(); !serial.empty())
				{
					std::cout << "\t\t- Serial: " << serial << "\n";
				}
			}
			return 0;
		}

		if (subcommand == "html")
		{
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup html <file>" << std::endl;
				return 1;
			}
			cli_html(args[2]);
			return 0;
		}

		if (subcommand == "http")
		{
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup http <uri>" << std::endl;
				return 1;
			}
			auto hr = HttpRequest(Uri(args[2]));
			auto res = hr.execute();
			if (res.has_value())
			{
				console.init(false); // to "correctly" display UTF-8 on Windows
				std::cout << res->status_code << "\n";
				std::cout << res->toString() << "\n";
			}
			else
			{
				std::cout << "Request failed.\n";
			}
			return 0;
		}

		if (subcommand == "ircserver")
		{
			return cli_ircserver();
		}

		if (subcommand == "json2bin")
		{
			return cli_json2bin(args.size(), args.data());
		}

		if (subcommand == "keyboard")
		{
			cli_keyboard();
			return 0;
		}

		if (subcommand == "maze")
		{
			cli_maze();
			return 0;
		}

		if (subcommand == "mesh")
		{
			return cli_mesh(args.size() - 2, &args[2]);
		}

		if (subcommand == "midi")
		{
			cli_midi(args.size() - 2, &args[2]);
			return 0;
		}

		if (subcommand == "morse")
		{
			cli_morse(args.size() - 2, &args[2]);
			return 0;
		}

		if (subcommand == "mouse")
		{
			cli_mouse();
			return 0;
		}

		if (subcommand == "netintrospect")
		{
			netIntrospectTask t;
			t.run();
			std::cout << t.getDiagnosticsString();
			return 0;
		}

		if (subcommand == "qr")
		{
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup qr <contents>" << std::endl;
				return 1;
			}
			auto qrcode = QrCode::encodeText(args[2]);
			console.init(false);
			console << qrcode.toCanvas(4, true).toStringDownsampledDoublewidth(true, true);
			console.resetColour();
			return 0;
		}

		if (subcommand == "repl")
		{
			cli_repl();
			return 0;
		}

		if (subcommand == "script")
		{
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup script <.cpp file>" << std::endl;
				return 1;
			}
			auto res = CompiledExecutable::fromCpp(args[2]);
			std::cout << res.compiler_output;
			if (res.exe_file.exists())
			{
				std::cout << os::execute(res.exe_file);
			}
			return 0;
		}

		if (subcommand == "snake")
		{
			cli_snake();
			return 0;
		}

		if (subcommand == "test")
		{
			cli_test();
			return 0;
		}

		if (subcommand == "wasm")
		{
			if (args.size() < 3)
			{
				std::cout << "Syntax: soup wasm <file>" << std::endl;
				return 1;
			}
			FileReader fr(args[2]);
			WasmScript scr;
			if (!scr.load(fr))
			{
				std::cout << "Failed to load\n";
				return 2;
			}
			if (!scr.instantiate())
			{
				std::cout << "Failed to instantiate\n";
				return 3;
			}
			auto code = scr.getExportedFuntion("_start");
			if (!code)
			{
				std::cout << "WASM file has loaded but \"_start\" function not found in exports.\n";
				return 4;
			}
			std::vector<std::string> wasi_args(args.begin() + 2, args.end());
			scr.linkWasiPreview1(std::move(wasi_args));
			WasmVm vm(scr);
			if (!vm.run(*code))
			{
				std::cout << "A runtime error occurred.\n";
				return 5;
			}
			return 0;
		}

		if (subcommand == "wast")
		{
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup wast <file>" << std::endl;
				return 1;
			}
			/*std::cout << "Attach debugger now." << std::endl;
			Sleep(5000);
			std::cout << "Starting." << std::endl;*/
			if (auto jr = json::decode(string::fromFile(args[2])))
			{
				SharedPtr<WasmScript> scr = soup::make_shared<WasmScript>();
				std::unordered_map<std::string, SharedPtr<WasmScript>> named_modules;
				std::vector<std::pair<std::string, SharedPtr<WasmScript>>> registered_module;
				try
				{
					for (const auto& cmd_entry : jr->asObj().at("commands").asArr())
					{
						const auto& cmd = cmd_entry.asObj();
						const auto& type = cmd.at("type").asStr();
						if (type == "module")
						{
							FileReader fr(cmd.at("filename").asStr());
							scr = soup::make_shared<WasmScript>();
							SOUP_IF_UNLIKELY (!scr->load(fr))
							{
								std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
								return 1;
							}
							scr->linkSpectestShim();
							for (const auto& mod : registered_module)
							{
								scr->importFromModule(mod.first, mod.second);
							}
							SOUP_IF_UNLIKELY (!scr->instantiate())
							{
								std::cout << "Failed to instantiate module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
								return 1;
							}
							if (cmd.contains("name"))
							{
								named_modules.emplace(cmd.at("name").asStr().value, scr);
							}
						}
						else if (type == "register")
						{
							registered_module.emplace_back(cmd.at("as").asStr().value, scr);
						}
						else if (type == "assert_malformed")
						{
							FileReader fr(cmd.at("filename").asStr());
							WasmScript tmp;
							SOUP_IF_UNLIKELY(tmp.load(fr))
							{
								std::cout << "Did not fail to load malformed module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
								goto _wast_next_cmd;
							}
						}
						else if (type == "assert_uninstantiable")
						{
							FileReader fr(cmd.at("filename").asStr());
							WasmScript tmp;
							SOUP_IF_UNLIKELY (!tmp.load(fr))
							{
								std::cout << "Failed to load module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
								return 1;
							}
							tmp.linkSpectestShim();
							SOUP_IF_UNLIKELY (tmp.instantiate())
							{
								std::cout << "Did not fail to instantiate malformed module " << cmd.at("filename").reinterpretAsStr().value << " (defined on line " << cmd.at("line").asInt().value << ")" << std::endl;
								goto _wast_next_cmd;
							}
						}
						else if (type == "assert_invalid")
						{
							// Soup doesn't do static validation
						}
						else
						{
							std::stack<WasmValue> stack;
							if (cmd.contains("action"))
							{
								const auto& action = cmd.at("action").asObj();
								if (action.contains("module"))
								{
									scr = named_modules.at(action.at("module").asStr().value);
								}
								const auto func_idx = scr->getExportedFuntion2(action.at("field").asStr());
								SOUP_IF_UNLIKELY (func_idx == -1)
								{
									std::cout << "Could not find export " << action.at("field").reinterpretAsStr().value  << " for test at line " << cmd.at("line").asInt().value << std::endl;
									goto _wast_next_cmd;
								}
								//std::cout << "running code from line " << cmd.at("line").asInt().value << std::endl;
								std::vector<WasmValue> args;
								for (const auto& arg : action.at("args").asArr())
								{
									const std::string& value = arg.asObj().at("value").asStr();
									const auto type = wasm_type_from_string(arg.asObj().at("type").asStr());
									if (value == "null")
									{
										args.emplace_back(static_cast<int64_t>(0)).type = type;
									}
									else
									{
										args.emplace_back(string::toIntOpt<uint64_t>(value, string::TI_FULL).value()).type = type;
										if (type == WASM_FUNCREF)
										{
											args.back().i64 |= 0x1'0000'0000;
										}
									}
								}
								if (!scr->call(func_idx, std::move(args), &stack))
								{
									SOUP_IF_UNLIKELY (type != "assert_trap" && type != "assert_exhaustion")
									{
										std::cout << "Execution failed for test at line " << cmd.at("line").asInt().value << std::endl;
										goto _wast_next_cmd;
									}
								}
								else
								{
									SOUP_IF_UNLIKELY (type == "assert_trap" || type == "assert_exhaustion")
									{
										std::cout << "Execution did not fail for test at line " << cmd.at("line").asInt().value << std::endl;
										goto _wast_next_cmd;
									}
								}
							}
							if (type == "assert_return")
							{
								const auto& expected_arr = cmd.at("expected").asArr();
								for (auto i = expected_arr.children.rbegin(); i != expected_arr.children.rend(); ++i)
								{
									SOUP_IF_UNLIKELY (stack.empty())
									{
										std::cout << "Stack too empty for test at line " << cmd.at("line").asInt().value << std::endl;
										goto _wast_next_cmd;
									}
									const auto& expected = (*i)->asObj();
									const std::string& type = expected.at("type").asStr();
									const std::string& value = expected.at("value").asStr();
									if (value == "nan:arithmetic" || value == "nan:canonical")
									{
										SOUP_IF_UNLIKELY (type == "f32"
											? !std::isnan(stack.top().f32)
											: !std::isnan(stack.top().f64)
											)
										{
											std::cout << "Return value was not NaN for test at line " << cmd.at("line").asInt().value << std::endl;
											goto _wast_next_cmd;
										}
									}
									/*else if (value == "nan:canonical")
									{
										if (type == "f32"
											? stack.top().i32 != 0x400000
											: stack.top().i64 != 0x8000000000000ll
											)
										{
											std::cout << "Return value was not nan:canonical for test at line " << cmd.at("line").asInt().value << std::endl;
											goto _wast_next_cmd;
										}
									}*/
									else
									{
										const auto stack_top_type = wasm_type_to_string(stack.top().type);
										SOUP_IF_UNLIKELY (type != stack_top_type
											|| (value == "null"
												? stack.top().i64 != 0
												: type == "i32" || type == "f32" || type == "funcref" // 32-bit type?
													? string::toIntOpt<uint32_t>(value, string::TI_FULL).value() != stack.top().i32
													: string::toIntOpt<uint64_t>(value, string::TI_FULL).value() != stack.top().i64
												)
											)
										{
											std::cout << "Return value mismatch for test at line " << cmd.at("line").asInt().value << std::endl;
											if (value == "null")
											{
												std::cout << "- Expected: <" << type << "> 0 (null)" << std::endl;
											}
											else
											{
												std::cout << "- Expected: <" << type << "> " << string::toIntOpt<uint64_t>(value, string::TI_FULL).value() << std::endl;
											}
											if (type == "i32" || type == "f32" || type == "funcref") // 32-bit type?
											{
												std::cout << "- Actual: <" << stack_top_type << "> " << (uint32_t)stack.top().i32 << std::endl;
											}
											else
											{
												std::cout << "- Actual: <" << stack_top_type << "> " << (uint64_t)stack.top().i64 << std::endl;
											}
											goto _wast_next_cmd;
										}
									}
									stack.pop();
								}
								// When code uses 'return', there may be superfluous values on the stack. This doesn't affect VM semantics, tho.
								/*SOUP_IF_UNLIKELY (!stack.empty())
								{
									std::cout << "Stack too full for test at line " << cmd.at("line").asInt().value << std::endl;
									goto _wast_next_cmd;
								}*/
							}
							else if (type != "action" && type != "assert_trap" && type != "assert_exhaustion")
							{
								std::cout << "Unknown command type: " << type.value << std::endl;
							}
						}
					_wast_next_cmd:;
					}
				}
				catch (const std::exception& e)
				{
					std::cout << e.what() << std::endl;
					return 1;
				}
			}
			else
			{
				std::cout << "Input file is not valid JSON (use wast2json if need be)" << std::endl;
				return 1;
			}
			std::cout << "Done." << std::endl;
			return 0;
		}

		if (subcommand == "wav")
		{
#if SOUP_WINDOWS || SOUP_LINUX
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup wav <file>" << std::endl;
				return 1;
			}
			static FileReader fr(args[2]);
			RiffReader rr(fr);
			if (rr.seekChunk("fmt ").isValid())
			{
				WavFmtChunk fmt;
				fmt.read(fr);
				if (fmt.isGoodForAudPlayback()
					&& rr.seekChunk("data").isValid()
					)
				{
					auto dev = audDevice::getDefault();
					std::cout << "WAV is good, playing on " << dev.getName() << "\n";
					auto pb = dev.open(fmt.channels, [](audPlayback& pb, audSample* block)
					{
						if (!fr.hasMore())
						{
							pb.stop();
							return;
						}
						std::string data;
						fr.str(AUD_BLOCK_BYTES, data);
						memcpy(block, data.data(), data.size());
					});
					pb->awaitCompletion();
					return 0;
				}
			}
			std::cout << "WAV is bad.\n";
#else
			std::cout << "Sorry, audio is currently not supported on your platform.\n";
#endif
			return 0;
		}

		if (subcommand == "websrv")
		{
			if (args.size() != 3)
			{
				std::cout << "Syntax: soup websrv <dir>" << std::endl;
				return 1;
			}
			return cli_websrv(args[2]);
		}
	}

	std::string all_tools;
	string::listAppend(all_tools, "3d");
	string::listAppend(all_tools, "bench");
	string::listAppend(all_tools, "cat2json");
#ifdef SOUP_ENABLE_CHATBOT
	string::listAppend(all_tools, "chatbot");
#endif
	string::listAppend(all_tools, "chatgpt");
#ifdef SOUP_ENABLE_CHESS
	string::listAppend(all_tools, "chess");
#endif
	string::listAppend(all_tools, "datareflection");
	string::listAppend(all_tools, "dhcp");
	string::listAppend(all_tools, "dig");
	string::listAppend(all_tools, "dnsserver");
	string::listAppend(all_tools, "dvd");
	string::listAppend(all_tools, "edit");
	string::listAppend(all_tools, "gamepad");
	string::listAppend(all_tools, "geoip");
	string::listAppend(all_tools, "hid");
	string::listAppend(all_tools, "html");
	string::listAppend(all_tools, "http");
	string::listAppend(all_tools, "ircserver");
	string::listAppend(all_tools, "json2bin");
	string::listAppend(all_tools, "keyboard");
	string::listAppend(all_tools, "maze");
	string::listAppend(all_tools, "mesh");
	string::listAppend(all_tools, "midi");
	string::listAppend(all_tools, "morse");
	string::listAppend(all_tools, "mouse");
	string::listAppend(all_tools, "netintrospect");
	string::listAppend(all_tools, "qr");
	string::listAppend(all_tools, "repl");
	string::listAppend(all_tools, "script");
	string::listAppend(all_tools, "snake");
	string::listAppend(all_tools, "test");
	string::listAppend(all_tools, "wasm");
	string::listAppend(all_tools, "wav");
	string::listAppend(all_tools, "websrv");

	std::cout << R"EOC(Syntax: soup <tool>

Available tools: )EOC" << all_tools << R"EOC(

Legend: <Required> [Optional])EOC" << std::endl;
	return 1;
}

SOUP_MAIN_CLI(entry);
