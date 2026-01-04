#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hittable_list.h"
#include "bvh.h"
#include "camera.h"
#include "material.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <omp.h>


void write_color(std::ostream& out, const Color& pixel_color, int samples_per_pixel) {
	double r = pixel_color.x;
	double g = pixel_color.y;
	double b = pixel_color.z;
	
	double scale = 1.0 / samples_per_pixel;
	r *= scale;
	g *= scale;
	b *= scale;
	
	r = std::sqrt(r);
	g = std::sqrt(g);
	b = std::sqrt(b);
	
	int ir = static_cast<int>(256 * std::clamp(r, 0.0, 0.999));
	int ig = static_cast<int>(256 * std::clamp(g, 0.0, 0.999));
	int ib = static_cast<int>(256 * std::clamp(b, 0.0, 0.999));
	
	out << ir << ' ' << ig << ' ' << ib << '\n';
}

Camera build_camera(double aspect_ratio) {
    // Vec3 lookfrom(1, 5.0, 1.0);
    // Vec3 lookat(0, 0.1, -2.5);
    // Vec3 vup(0, 1, 0);
    // double vfov = 30.0;
	Vec3 lookfrom(0, 0, 0);
    Vec3 lookat(0, 0, -1);
    Vec3 vup(0, 1, 0);
    double vfov = 90.0;
    return Camera(lookfrom, lookat, vup, vfov, aspect_ratio);
}

HittableList build_scene() {
    HittableList world;
	/**
    auto material_ground = std::make_shared<Lambertian>(Color(0.1, 0.1, 0.1));
    auto material_glass = std::make_shared<Dielectric>(1.5);
    auto material_gold = std::make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.05);
    auto material_red = std::make_shared<Lambertian>(Color(0.9, 0.1, 0.1));
	auto material_green = std::make_shared<Lambertian>(Color(0.1, 0.9, 0.1));
	auto material_emission = std::make_shared<DiffuseLight>(Color(4.0, 4.0, 2.0), 1.3);

    world.add(std::make_shared<Sphere>(Vec3(0.0, -100.5, -1.0), 100.0, material_ground));

    const int grid_size = 4;
    const double sphere_radius = 0.1;
    const double spacing = 0.26;
    const Vec3 center_offset(-0.5, 0.0, -2.5);

    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            for (int k = 0; k < grid_size; k++) {
                Vec3 pos = center_offset + Vec3(i * spacing, j * spacing, k * spacing);
                
                std::shared_ptr<Material> mat;
                double choose = random_double();
                if (choose < 0.2) mat = material_gold;
                else if (choose < 0.5) mat = material_red;
				// 	else if (choose < 0.6) mat = material_green;
				else if (choose < 0.55) mat = material_emission;
                else mat = material_glass;
				// mat = material_red;

                world.add(std::make_shared<Sphere>(pos, sphere_radius, mat));
            }
        }
    }
	*/
	auto material_ground = std::make_shared<Lambertian>(Color(0.92, 0.86, 0.70));
	auto material_red = std::make_shared<Lambertian>(Color(0.62, 0.12, 0.09));
	auto material_white = std::make_shared<Lambertian>(Color(0.96, 0.94, 0.85));
	auto material_gold = std::make_shared<Lambertian>(Color(0.70, 0.50, 0.20));
	auto material_blue = std::make_shared<Lambertian>(Color(0.27, 0.36, 0.36));

	world.add(std::make_shared<Sphere>(Vec3(0.0, -100.5, -1.0), 100.0, material_ground));
	world.add(std::make_shared<Sphere>(Vec3(0.0, 0.0, -1.5), 0.5, material_red));
	world.add(std::make_shared<Sphere>(Vec3(-0.6, -0.3, -0.8), 0.2, material_white));
	world.add(std::make_shared<Sphere>(Vec3(0.8, -0.2, -1.0), 0.3, material_gold));
	world.add(std::make_shared<Sphere>(Vec3(-1.5, 0.2, -2.5), 0.7, material_blue));
    return world;
}

inline Color render_pixel(int i, int j, int image_width, int image_height, int samples_per_pixel, const Camera& camera, const Hittable& world, int max_depth) {
	Color pixel_color(0,0,0);

	for (int s = 0; s < samples_per_pixel; ++s) {
		double u = (i + random_double()) / (image_width - 1);
		double v = (j + random_double()) / (image_height - 1);
		Ray ray = camera.get_ray(u, v);
		pixel_color += ray_color(ray, world, max_depth);
	}
	return pixel_color;
}

void render_image(std::vector<Color>& framebuffer, int image_width, int image_height, int samples_per_pixel, const Camera& camera, const Hittable& world, int max_depth) {
	#pragma omp parallel for schedule(dynamic)
	for (int j = 0; j < image_height; ++j) {
		for (int i = 0; i < image_width; ++i) {
			Color c = render_pixel(
				i, j,
				image_width, image_height,
				samples_per_pixel,
				camera, world, max_depth
			);

			int flipped_j = image_height - 1 - j;
			framebuffer[flipped_j * image_width + i] = c;
		}
	}
}

void write_image(const std::string& filename, const std::vector<Color>& framebuffer, int image_width, int image_height, int samples_per_pixel) {
	std::ofstream out(filename);
	out << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (const auto& pixel : framebuffer)
		write_color(out, pixel, samples_per_pixel);
} 

int main() {
	const double aspect_ratio = 16.0 / 9.0;
	const int image_width = 1600;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	const int samples_per_pixel = 500;
	const int max_depth = 10;

	std::cout << "Rendering a " << image_width << "x" << image_height << " image with "
			  << samples_per_pixel << " samples per pixel and max depth " << max_depth << ".\n";
	std::cout << "Building Scene...\n";

	HittableList world = build_scene();
	std::cout << "Building BVH...\n";
	auto start_bvh = std::chrono::high_resolution_clock::now();
	BVHNode bvh_tree(world);
	auto end_bvh = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> bvh_duration = end_bvh - start_bvh;
	std::cout << "BVH built in " << bvh_duration.count() << " seconds" << std::endl;

	// HittableList world = build_scene();
	Camera camera = build_camera(aspect_ratio);
	std::vector<Color> framebuffer(image_width * image_height);
	
	render_image(
			framebuffer,
			image_width, image_height,
			samples_per_pixel,
			camera, bvh_tree, max_depth
	);
	
	write_image(
		"output/image.ppm",
		framebuffer,
		image_width, image_height,
		samples_per_pixel
	);
	
	return 0;
}