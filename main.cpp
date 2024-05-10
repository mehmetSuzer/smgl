
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <limits>

#include <color.h>
#include <vector3d.h>
#include <matrix3x3.h>
#include <camera.h>
#include <light.h>
#include <sphere.h>
#include <triangle.h>
#include <stb_image_write.h>

#define greater(x, y) ((x > y) ? x : y)

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 4UL

#define HEIGHT  1000UL
#define WIDTH   1000UL

const float SURFACE_LEVEL = -50.0f;
const Color AMBIENT_COLOR = Color("WHITE");
const Color BACKGROUND_COLOR = Color("BLACK");

/* ----------------------------------------------------------------------*/
const Vector3D vertices[] = {
    Vector3D(0.0f, SURFACE_LEVEL, 120.0f), // pyramid
    Vector3D(30.0f, SURFACE_LEVEL, 120.0f),
    Vector3D(30.0f, SURFACE_LEVEL, 150.0f),
    Vector3D(0.0f, SURFACE_LEVEL, 150.0f),
    Vector3D(15.0f, SURFACE_LEVEL + 15*sqrtf(2), 135.0f),

    Vector3D(-50.0f, SURFACE_LEVEL+25.0f, 200.0f), // spheres
    Vector3D(0.0f, SURFACE_LEVEL+25.0f, 200.0f),

    Vector3D(-80.0f, 80.0f, 200.0f), // lights
    Vector3D(80.0f, 80.0f, 250.0f),

    Vector3D(0.0f, SURFACE_LEVEL, -100.0f), // plane
    Vector3D(-1000.0f, SURFACE_LEVEL, 1000.0f),
    Vector3D(1000.0f, SURFACE_LEVEL, 1000.0f),
};
const uint32_t vertex_number = sizeof(vertices) / sizeof(vertices[0]);

/* ----------------------------------------------------------------------*/
const Sphere spheres[] = {
    Sphere(vertices[5], 25.0f, Color("GREEN"), DEFAULT_SPHERE_DIFFUSE_COEF, DEFAULT_SPHERE_SPECULAR_COEF, DEFAULT_SPHERE_SPECULAR_POW, DEFAULT_SPHERE_AMBIENT_COEF, true),
    Sphere(vertices[6], 25.0f, Color("YELLOW")),
};
const uint32_t sphere_number = sizeof(spheres) / sizeof(spheres[0]);

/* ----------------------------------------------------------------------*/
const Triangle triangles[] = {
    Triangle(vertices[0], vertices[1], vertices[4], Color("RED")), // Pyramid Side surfaces
    Triangle(vertices[1], vertices[2], vertices[4], Color("GREEN")),
    Triangle(vertices[2], vertices[3], vertices[4], Color("BLUE")),
    Triangle(vertices[0], vertices[3], vertices[4], Color("MAGENTA")),

    Triangle(vertices[0], vertices[1], vertices[2], Color("CYAN")), // Pyramid Bottom surface
    Triangle(vertices[0], vertices[3], vertices[2], Color("YELLOW")),

    Triangle(vertices[9], vertices[10], vertices[11], Color("GRAY")), // plane
};
const uint32_t triangle_number = sizeof(triangles) / sizeof(triangles[0]);

/* ----------------------------------------------------------------------*/
const Light lights[] = {
    Light(vertices[7], 12000.0f, Color("WHITE")),
    Light(vertices[8], 10000.0f, Color("CYAN")),
};
const uint32_t light_number = sizeof(lights) / sizeof(lights[0]);

/* ----------------------------------------------------------------------*/
Camera camera(
    WIDTH, HEIGHT, 
    Vector3D(-50.0f, -50.0f, 100.0f), 
    Vector3D(50.0f, 50.0f, 100.0f), 
    Vector3D(0.0f, 0.0f, 0.0f)
);

void cast_ray(const Vector3D& ray, const Vector3D& source, Color& color, uint32_t depth_count) {
    // If the max recursive depth is exceeded, then stop
    if (depth_count > MAX_RECURSIVE_RAY_TRACING_DEPTH) {
        return;
    }

    uint32_t i, j;
    Intersect intersect;
    Intersect closest_intersect = {.t = INFINITY};
    Shape* closest_shape = NULL;

    // Check intersections with spheres
    for (i = 0; i < sphere_number; i++) {
        if (spheres[i].intersect(&intersect, source + ray*0.1f, ray) && intersect.t < closest_intersect.t) {
            closest_shape = (Shape*)(spheres+i);
            closest_intersect = intersect;
        }
    }

    // Check intersections with triangles
    for (i = 0; i < triangle_number; i++) {
        if (triangles[i].intersect(&intersect, source + ray*0.1f, ray) && intersect.t < closest_intersect.t) {
            closest_shape = (Shape*)(triangles+i);
            closest_intersect = intersect;
        }
    }

    // Check if the ray hits to an object
    if (closest_shape != NULL) {
        // Add the ambient lighting once
        if (depth_count == 1) {
            color += AMBIENT_COLOR * closest_shape->ambient_coef;
        }

        for (i = 0; i < light_number; i++) {
            // Generate the light ray pointing from the hit location to the light source
            Vector3D light_vector = lights[i].position-closest_intersect.hit_location;
            Vector3D light_ray = light_vector.normalize();
            float light_intensity = lights[i].intensity / light_vector.mag_square();
            bool shadowed = false;
            float diffuse = 0.0f;
            float specular = 0.0f;

            // Check if any sphere prevents light beams from hitting to the hit location
            for (j = 0; j < sphere_number && !shadowed; j++) {
                shadowed = spheres[j].intersect(NULL, closest_intersect.hit_location + light_ray*0.1f, light_ray);
            }

            // Check if any triangle prevents light beams from hitting to the hit location
            for (j = 0; j < triangle_number && !shadowed; j++) {
                shadowed = triangles[j].intersect(NULL, closest_intersect.hit_location + light_ray*0.1f, light_ray);
            }

            // If the hit location can get light beam directly from the light source
            if (!shadowed) {
                // Calculate diffuse and specular light intensity
                diffuse = closest_shape->diffuse_coef * light_intensity * greater(light_ray.dot(closest_intersect.normal), 0.0f);
                Vector3D bisector = Vector3D::bisector(light_ray, -ray);
                specular = closest_shape->specular_coef * light_intensity * powf(greater(bisector.dot(closest_intersect.normal), 0.0f), closest_shape->specular_pow);
            }

            // Update the color
            // As the recursion depth increases, the effect of reflections get smaller
            color += closest_shape->color * lights[i].color * (diffuse + specular) / powf(depth_count, 2.0f);

            // If the surface is reflective
            if (closest_shape->reflect) {
                // Calculate the new ray and call recursively
                Vector3D new_ray = Vector3D::reflection(-ray, closest_intersect.normal);
                cast_ray(new_ray, closest_intersect.hit_location, color, depth_count+1);
            }
        }
    } // If the ray does not hit to any object, then set the color to the background color
    else if (depth_count == 1) {
        color = BACKGROUND_COLOR;
    }
}

int main(int argc, char **argv) {
    Color image[HEIGHT*WIDTH];
    std::cout << "Rendering..." << std::endl;

    // Ray tracing
    for (uint32_t i = 0; i < WIDTH; i++) { // x axis
        for (uint32_t j = 0; j < HEIGHT; j++) { // y axis     
            // Generate a ray from camera to the center of the pixel
            Vector3D ray = camera.generate_ray((i+0.5f)/WIDTH, (j+0.5f)/HEIGHT);
            
            // Calculate the color of the pixel
            Color color = Color("BLACK");
            cast_ray(ray, camera.position, color, 1);
            
            // Write to buffer
            image[(HEIGHT-j-1)*WIDTH + i] = color;
        } 
    }

    std::cout << "Writing image.png ..." << std::endl;
    stbi_write_png("image.png", WIDTH, HEIGHT, 3, image, 3*WIDTH);
    std::cout << "Finished..." << std::endl;
    return 0;
}
