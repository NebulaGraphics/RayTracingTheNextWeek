#include "./opencv2/opencv.hpp"
#include "./opencv2/core/opengl.hpp"
#include "./opencv2/highgui/highgui.hpp"
#include "./opencv2/highgui/highgui_c.h"
#include "rt_weekend.h"
#include <cmath>
#include "vec3.h"
#include "ray.h"
#include "interval.h"
#include "material.h"
#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "hittable_list.h"
#include "sphere.h"
#include "bvh_node.h"
#include "texture.h"

#if _DEBUG
#pragma comment(lib,"opencv_world481d.lib")
#else
#pragma comment(lib,"opencv_world481.lib")
#endif

void random_sphere(hittable_list& world)
{
	auto checker = make_shared<checker_texture>(0.1, color(.2, .3, .1), color(.9, .9, .9));
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(checker)));

	for (int a = -11; a < 11; a++) {
		for (int b = -11; b < 11; b++) {
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

			if ((center - point3(4, 0.2, 0)).length() > 0.9) {
				shared_ptr<material> sphere_material;

				if (choose_mat < 0.8) {
					// diffuse
					auto albedo = color::random() * color::random();
					sphere_material = make_shared<lambertian>(albedo);
					auto center2 = center + vec3(0, random_double(0, .5), 0);

					world.add(make_shared<sphere>(center, center2, 0.2, sphere_material));
					world.add(make_shared<sphere>(center, 0.2, sphere_material));

				}
				else if (choose_mat < 0.95) {
					// metal
					auto albedo = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_material = make_shared<metal>(albedo, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
				else {
					// glass
					sphere_material = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_material));
				}
			}
		}
	}

	auto material1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

	auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

	auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

}

void two_spheres(hittable_list& world)
{
	auto checker = make_shared<checker_texture>(0.8, color(.2, .3, .1), color(.9, .9, .9));

	world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	world.add(make_shared<sphere>(point3(0, 10, 0), 10, make_shared<lambertian>(checker)));
}

void earth(hittable_list& world) {
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	
	world.add(make_shared<sphere>(point3(0, 0, 0), 2, earth_surface));
}

void two_perlin_spheres(hittable_list& world) {

	auto pertext = make_shared<noise_texture>(4);
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

}
int main(int argc, char* argv[])
{

#if _DEBUG

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

#endif

	hittable_list world;
	
	std::cout << " Input a number (0 ~ 3) to select scene:\n";
	int scene_selection = -1;
	std::cin >> scene_selection;
	switch (scene_selection)
	{
	case 0:
		random_sphere(world);
		break;
	case 1:
		two_spheres(world);
		break;
	case 2:
		earth(world);
		break;
	case 3:
		two_perlin_spheres(world);
		break;
	default:
		return 0;
	}

	cv::String window_name = "Raytracing: The Next Week";

	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);

	const float aspect_ratio = 16.0f / 9.0f;
	const int height = 300;
	const int width = (int)(height * aspect_ratio);
	auto buffer = new cv::Mat(height, width, CV_8UC3);

	std::cout << "width:" << width << ", height: " << height << "\n";
	int keyCode = 0;


	world = hittable_list(make_shared<bvh_node>(world));

	camera main_camera(
		vec3{ 13, 2, 3 },               // position
		view_port{ 0, 0, 1, 1 },        // view port 
		0.1f,                           // near
		10000.f,                       // far
		20.f,                          // fov
		aspect_ratio                   // aspect ratio
	);


	main_camera.set_fov(20);
	main_camera.set_position(vec3(13, 2, 3));
	main_camera.look_at(vec3(0, 0, 0));
	main_camera.set_defocus_angle(0.02);
	main_camera.set_focus_dist(10);
	main_camera.sample_count = 10;
	main_camera.bounce = 20;

	main_camera.render(buffer, world, 128);

	while (keyCode != 27)
	{
		cv::imshow(window_name, *buffer);
		keyCode = cv::pollKey();
	}

	main_camera.dispose();

	cv::destroyAllWindows();

	delete buffer;

	return 0;
}


























