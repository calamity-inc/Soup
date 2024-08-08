#include "cli.hpp"

#include <thread>

#include <Canvas.hpp>
#include <console.hpp>
#include <rand.hpp>
#include <unicode.hpp>
#include <Vector2.hpp>

constexpr auto tickrate = 50;

constexpr soup::Rgb colour_bg{ 0, 0, 0 };
constexpr soup::Rgb colour_snake{ 255, 255, 255 };
constexpr soup::Rgb colour_fruit{ 255, 0, 255 };

static soup::Canvas c;

static soup::Vector2 snake_head;
static soup::Vector2 snake_momentum;

static std::vector<soup::Vector2> snake_tail{};

static void clampPos(soup::Vector2& pos)
{
	if (pos.x < 0.0f)
	{
		pos.x = static_cast<float>(c.width - 1);
	}
	else if (pos.x == c.width)
	{
		pos.x = 0.0f;
	}
	if (pos.y < 0.0f)
	{
		pos.y = static_cast<float>(c.height - 1);
	}
	else if (pos.y == c.height)
	{
		pos.y = 0.0f;
	}
}

static void move()
{
	snake_tail.emplace_back(snake_head);
	snake_head += snake_momentum;
	clampPos(snake_head);
}

static void placeFruit()
{
	while (true)
	{
		auto x = soup::rand.t<unsigned int>(0, c.width - 1);
		auto y = soup::rand.t<unsigned int>(0, c.height - 1);
		if (c.get(x, y) == colour_bg)
		{
			c.set(x, y, colour_fruit);
			break;
		}
	}
}
static void render()
{
	soup::console.setCursorPos(0, 0);
	soup::console << soup::unicode::utf16_to_utf8(c.toStringDownsampledDoublewidth(false, false));
	soup::console.setCursorPos(0, 0);
}

static void setMomentum(soup::Vector2 momentum)
{
	auto next_pos = (snake_head + momentum);
	clampPos(next_pos);
	if (c.get(static_cast<unsigned int>(next_pos.x), static_cast<unsigned int>(next_pos.y)) != colour_snake)
	{
		snake_momentum = momentum;
	}
}

static void resetState()
{
	snake_head = { 2, 0 };
	snake_momentum = { 1, 0 };
	snake_tail.clear();
	snake_tail.emplace_back(soup::Vector2{ 0, 0 });
	snake_tail.emplace_back(soup::Vector2{ 1, 0 });
	c.fill(soup::Rgb::BLACK);
	c.set(0, 0, colour_snake);
	c.set(1, 0, colour_snake);
	placeFruit();
}

static bool game_thread_running = false;

static void startGameThread()
{
	if (game_thread_running)
	{
		return;
	}
	game_thread_running = true;
	std::thread t([]
	{
		while (game_thread_running)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(tickrate));
			move();
			const auto prev_colour = c.get(static_cast<unsigned int>(snake_head.x), static_cast<unsigned int>(snake_head.y));
			if (prev_colour == colour_snake)
			{
				resetState();
			}
			else
			{
				c.set(static_cast<unsigned int>(snake_head.x), static_cast<unsigned int>(snake_head.y), colour_snake);
				if (prev_colour == colour_fruit)
				{
					placeFruit();
				}
				else
				{
					c.set(static_cast<unsigned int>(snake_tail.begin()->x), static_cast<unsigned int>(snake_tail.begin()->y), colour_bg);
					snake_tail.erase(snake_tail.begin());
				}
			}
			render();
		}
		soup::console.cleanup();
		soup::console << "Bye, bye!\n";
		exit(0);
	});
	t.detach();
}

void cli_snake()
{
	soup::console.init(true);

	soup::console.setTitle("ConSnake");

	soup::console.overrideCtrlC([]
	{
		game_thread_running = false;
	});

	soup::console.enableSizeTracking([](unsigned int width, unsigned int height, const soup::Capture&)
	{
		c = { width, height * 2 };
		resetState();
		render();
		startGameThread();
	});

	soup::console.char_handler = (soup::console_impl::char_handler_t)[](char32_t c)
	{
		if (c == 'w')
		{
			setMomentum({ 0, -1 });
		}
		else if (c == 'a')
		{
			setMomentum({ -1, 0 });
		}
		else if (c == 's')
		{
			setMomentum({ 0, 1 });
		}
		else if (c == 'd')
		{
			setMomentum({ 1, 0 });
		}
	};

	soup::console.run();
}
