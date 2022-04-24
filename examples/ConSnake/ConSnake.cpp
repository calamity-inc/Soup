#include <thread>

#include <Canvas.hpp>
#include <console.hpp>
#include <rand.hpp>
#include <unicode.hpp>
#include <Vector2.hpp>

constexpr auto res_scale = 1;
constexpr auto tickrate = 50;

constexpr soup::rgb colour_bg{ 0, 0, 0 };
constexpr soup::rgb colour_snake{ 255, 255, 255 };
constexpr soup::rgb colour_fruit{ 255, 0, 255 };

static soup::canvas c{ 80 * res_scale, 24 * res_scale };

static soup::vector2 snake_head{ 2, 0 };
static soup::vector2 snake_momentum{ 1, 0 };

static std::vector<soup::vector2> snake_tail{};

static void clampPos(soup::vector2& pos)
{
	if (pos.x < 0)
	{
		pos.x = c.width - 1;
	}
	else if (pos.x == c.width)
	{
		pos.x = 0;
	}
	if (pos.y < 0)
	{
		pos.y = c.height - 1;
	}
	else if (pos.y == c.height)
	{
		pos.y = 0;
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
		int x = soup::rand(0, c.width - 1);
		int y = soup::rand(0, c.height - 1);
		if (c.get(x, y) == colour_bg)
		{
			c.set(x, y, colour_fruit);
			break;
		}
	}
}
static void render()
{
	soup::console.setCursorPos(1, 1);
	if constexpr (res_scale == 2)
	{
		soup::console << soup::unicode::utf16_to_utf8(c.toStringDownsampled());
	}
	else
	{
		soup::console << c.toString();
	}
	soup::console.setCursorPos(1, 1);
}

static void gameover(const char* reason)
{
	soup::console.cleanup();
	soup::console << reason << "\n";
	exit(0);
}

static void setMomentum(soup::vector2 momentum)
{
	auto next_pos = (snake_head + momentum);
	clampPos(next_pos);
	if (c.get(next_pos.x, next_pos.y) != colour_snake)
	{
		snake_momentum = momentum;
	}
}

int main()
{
	soup::console.init();

	soup::console.setTitle("ConSnake");

	soup::console.overrideCtrlC([]
	{
		gameover("Bye, bye!");
	});

	soup::console.char_handler = [](char32_t c, const soup::capture&)
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

	snake_tail.emplace_back(soup::vector2{ 0, 0 });
	snake_tail.emplace_back(soup::vector2{ 1, 0 });
	c.set(0, 0, colour_snake);
	c.set(1, 0, colour_snake);
	placeFruit();
	render();

	std::thread t([]
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(tickrate));
			move();
			const auto prev_colour = c.get(snake_head.x, snake_head.y);
			if (prev_colour == colour_snake)
			{
				gameover("Game over! You tried to eat yourself.");
			}
			c.set(snake_head.x, snake_head.y, colour_snake);
			if (prev_colour == colour_fruit)
			{
				placeFruit();
			}
			else
			{
				c.set(snake_tail.begin()->x, snake_tail.begin()->y, colour_bg);
				snake_tail.erase(snake_tail.begin());
			}
			render();
		}
	});
	t.detach();

	soup::console.run();
}
