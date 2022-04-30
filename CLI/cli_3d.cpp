#include "cli.hpp"

#include <fstream>

#include <Box.hpp>
#include <Canvas.hpp>
#include <console.hpp>
#include <ControlInput.hpp>
#include <Plane.hpp>
#include <PointLight.hpp>
#include <Poly.hpp>
#include <PolyVector.hpp>
#include <Ray.hpp>
#include <Scene.hpp>
#include <time.hpp>
#include <unicode.hpp>
#include <UvSphere.hpp>

using namespace soup;

static Canvas c;
static Scene s;
static float fov = 80.0f;

static void amogus()
{
	for (const auto& p : Box{ Vector3{ -0.5f, 2.0f, -0.5f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ 0.5f, 2.0f, -0.5f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ -0.5f, 2.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ -0.0f, 2.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ 0.5f, 2.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ 1.0f, 2.0f, 0.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ -0.5f, 2.0f, 0.5f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 255, 255 } });
	}

	for (const auto& p : Box{ Vector3{ 0.0f, 2.0f, 0.5f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 255, 255 } });
	}

	for (const auto& p : Box{ Vector3{ 0.5f, 2.0f, 0.5f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ 1.0f, 2.0f, 0.5f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ -0.5f, 2.0f, 1.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ 0.0f, 2.0f, 1.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	for (const auto& p : Box{ Vector3{ 0.5f, 2.0f, 1.0f }, Vector3{ 0.0f, 0.0f, 0.0f }, Vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}
}

static void updateScene()
{
	s.tris.clear();

	for (const auto& p : Plane{ Vector3{ 5.0f, 5.0f, -1.0f }, Vector3{ -5.0f, -5.0f, -1.0f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 0, 255, 0 }, true });
	}

	for (const auto& p : Box{ Vector3{ 0.5f, 1.0f, 0.0f }, Vector3{ 10.0f, 10.0f, 10.0f }, Vector3{ 0.6f, 0.6f, 0.6f } }.toPolys())
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 255 } });
	}

	for (const auto& p : UvSphere{ Vector3 { -0.5f, 1.0f, 0.0f }, 0.5f }.toPolys(10))
	{
		s.tris.emplace_back(Scene::Tri{ p, Rgb { 255, 0, 0 } });
	}

	amogus();
}

static void render()
{
	auto render_time = time::millis();
	s.renderOnto(c, fov);
	render_time = time::millis() - render_time;
	
	console.setCursorPos(0, 0);
	
	auto print_time = time::millis();
	console << c.toString();
	print_time = time::millis() - print_time;

	console.setCursorPos(0, 0);
	console.setBackgroundColour(Rgb::BLACK);
	console.setForegroundColour(255, 0, 255);
	std::string info = "[WASDRF] Position  [↑←↓→] Rotation  [QE] FOV  [H] Save hi res image";
	console << info;
	console.setCursorPos(0, 1 + (unicode::utf8_char_len(info) / c.width));
	console << "frame time: " << (render_time + print_time) << "ms (" << render_time << "ms render, " << print_time << "ms print)";
}

void cli_3d()
{
	s.render_distance = 25.0f;

	console.init(true);
	console.enableSizeTracking([](unsigned int width, unsigned int height, const Capture&)
	{
		c = Canvas(width, height);
		console.clearScreen();
		updateScene();
		render();
	});
	console.char_handler = [](char32_t c)
	{
		if (c == 'w')
		{
			s.cam_pos += (Vector3{ 0.0f, 0.0f, 0.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 'a')
		{
			s.cam_pos += (Vector3{ 0.0f, 0.0f, 270.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 's')
		{
			s.cam_pos += (Vector3{ 0.0f, 0.0f, 180.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 'd')
		{
			s.cam_pos += (Vector3{ 0.0f, 0.0f, 90.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 'r')
		{
			s.cam_pos.z += 0.1f;
			render();
		}
		if (c == 'f')
		{
			s.cam_pos.z -= 0.1f;
			render();
		}
		if (c == 'q')
		{
			fov -= 5.0f;
			render();
		}
		if (c == 'e')
		{
			fov += 5.0f;
			render();
		}
		if (c == 'h')
		{
			Canvas hires{ 500, 500 };
			s.renderOnto(hires, fov);
			{
				std::ofstream of("hires.ppm");
				of << hires.toPpm();
			}
			console.setBackgroundColour(Rgb::BLACK);
			console.setForegroundColour(Rgb::WHITE);
			system("ffmpeg -y -i hires.ppm hires.png");
			console.clearScreen();
			render();
		}
	};
	console.control_handler = [](ControlInput i)
	{
		if (i == RIGHT)
		{
			s.cam_rot.z += 5.0f;
			render();
		}
		if (i == LEFT)
		{
			s.cam_rot.z -= 5.0f;
			render();
		}
		if (i == UP)
		{
			s.cam_rot.x += 5.0f;
			render();
		}
		if (i == DOWN)
		{
			s.cam_rot.x -= 5.0f;
			render();
		}
	};
	console.overrideCtrlC([]
	{
		console.cleanup();
		exit(0);
	});
	console.run();
}
