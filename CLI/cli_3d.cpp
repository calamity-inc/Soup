#include "cli.hpp"

#include <fstream>

#include <box.hpp>
#include <canvas.hpp>
#include <console.hpp>
#include <control_input.hpp>
#include <plane.hpp>
#include <point_light.hpp>
#include <poly.hpp>
#include <poly_vector.hpp>
#include <ray.hpp>
#include <scene.hpp>
#include <uv_sphere.hpp>

using namespace soup;

static canvas c;
static scene s;
static float fov = 80.0f;

static void amogus()
{
	for (const auto& p : box{ vector3{ -0.5f, 2.0f, -0.5f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ 0.5f, 2.0f, -0.5f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ -0.5f, 2.0f, 0.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ -0.0f, 2.0f, 0.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ 0.5f, 2.0f, 0.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ 1.0f, 2.0f, 0.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ -0.5f, 2.0f, 0.5f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 255, 255 } });
	}

	for (const auto& p : box{ vector3{ 0.0f, 2.0f, 0.5f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 255, 255 } });
	}

	for (const auto& p : box{ vector3{ 0.5f, 2.0f, 0.5f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ 1.0f, 2.0f, 0.5f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ -0.5f, 2.0f, 1.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ 0.0f, 2.0f, 1.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	for (const auto& p : box{ vector3{ 0.5f, 2.0f, 1.0f }, vector3{ 0.0f, 0.0f, 0.0f }, vector3{ 0.5f, 0.5f, 0.5f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}
}

static void updateScene()
{
	s.tris.clear();

	for (const auto& p : plane{ vector3{ 5.0f, 5.0f, -1.0f }, vector3{ -5.0f, -5.0f, -1.0f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 0, 255, 0 }, true });
	}

	for (const auto& p : box{ vector3{ 0.5f, 1.0f, 0.0f }, vector3{ 10.0f, 10.0f, 10.0f }, vector3{ 0.6f, 0.6f, 0.6f } }.toPolys())
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 255 } });
	}

	for (const auto& p : uv_sphere{ vector3 { -0.5f, 1.0f, 0.0f }, 0.5f }.toPolys(10))
	{
		s.tris.emplace_back(scene::tri{ p, rgb { 255, 0, 0 } });
	}

	amogus();
}

static void render()
{
	s.renderOnto(c, fov);
	console.setCursorPos(0, 0);
	console << c.toStringx1();
	console.setCursorPos(0, 0);
	console.setBackgroundColour(rgb::BLACK);
	console.setForegroundColour(255, 0, 255);
	console << "[WASDRF] Position  [↑←↓→] Rotation  [QE] FOV  [H] Save hi res image";
}

void cli_3d()
{
	s.render_distance = 25.0f;

	console.init();
	console.enableSizeTracking([](unsigned int width, unsigned int height, const capture&)
	{
		c = canvas(width, height);
		console.clearScreen();
		updateScene();
		render();
	});
	console.char_handler = [](char32_t c)
	{
		if (c == 'w')
		{
			s.cam_pos += (vector3{ 0.0f, 0.0f, 0.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 'a')
		{
			s.cam_pos += (vector3{ 0.0f, 0.0f, 270.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 's')
		{
			s.cam_pos += (vector3{ 0.0f, 0.0f, 180.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
			render();
		}
		if (c == 'd')
		{
			s.cam_pos += (vector3{ 0.0f, 0.0f, 90.0f + s.cam_rot.z }).toDirNoZ() * 0.1f;
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
			canvas hires{ 500, 500 };
			s.renderOnto(hires, fov);
			{
				std::ofstream of("hires.ppm");
				of << hires.toPPM();
			}
			console.setBackgroundColour(rgb::BLACK);
			console.setForegroundColour(rgb::WHITE);
			system("ffmpeg -y -i hires.ppm hires.png");
			console.clearScreen();
			render();
		}
	};
	console.control_handler = [](control_input i)
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
