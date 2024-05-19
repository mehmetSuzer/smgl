
#include <iostream>
#include <fstream>
#include <thread>
#include <stb_image_write.h>

#include <camera.h>
#include <sphere.h>
#include <triangle.h>
#include <bezier.h>

#define smaller(x, y) ((x > y) ? y : x)
#define greater(x, y) ((x > y) ? x : y)

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 4UL

#define HEIGHT  600UL
#define WIDTH   600UL

#define THREAD_NUMBER 12UL
#define WIDTH_PER_THREAD (WIDTH/THREAD_NUMBER)

const float SURFACE_LEVEL = -50.0f;
const Color AMBIENT_COLOR = Color("WHITE");
const Color BACKGROUND_COLOR = Color("BLACK");
Color image[HEIGHT*WIDTH];

/* ----------------------------------------------------------------------*/

const Vector3D vertices[] = {
    Vector3D(10.0f, SURFACE_LEVEL, 200.0f), // pyramid
    Vector3D(60.0f, SURFACE_LEVEL, 200.0f),
    Vector3D(60.0f, SURFACE_LEVEL, 230.0f),
    Vector3D(10.0f, SURFACE_LEVEL, 230.0f),
    Vector3D(35.0f, SURFACE_LEVEL + 50*sqrtf(2), 215.0f),

    Vector3D(-50.0f, SURFACE_LEVEL+40.0f, 220.0f), // spheres

    Vector3D(0.0f, SURFACE_LEVEL, -100.0f), // plane
    Vector3D(-1000.0f, SURFACE_LEVEL, 1000.0f),
    Vector3D(1000.0f, SURFACE_LEVEL, 1000.0f),
};
const uint32_t vertex_number = sizeof(vertices) / sizeof(vertices[0]);

/* ----------------------------------------------------------------------*/

const Sphere spheres[] = {
    Sphere(vertices[5], 40.0f, Color("GREEN"), true, 0.0f, VACUUM_REFRACTIVE_INDEX),
};
const uint32_t sphere_number = sizeof(spheres) / sizeof(spheres[0]);

/* ----------------------------------------------------------------------*/

const Triangle triangles[] = {
    Triangle(vertices[0], vertices[1], vertices[4], Color("RED"), true, 0.0f, VACUUM_REFRACTIVE_INDEX), // Pyramid Side surfaces
    Triangle(vertices[1], vertices[2], vertices[4], Color("GREEN"), true, 0.0f, VACUUM_REFRACTIVE_INDEX),
    Triangle(vertices[2], vertices[3], vertices[4], Color("BLUE"), true, 0.0f, VACUUM_REFRACTIVE_INDEX),
    Triangle(vertices[0], vertices[3], vertices[4], Color("MAGENTA"), true, 0.0f, VACUUM_REFRACTIVE_INDEX),

    Triangle(vertices[0], vertices[1], vertices[2], Color("CYAN"), true, 0.0f, VACUUM_REFRACTIVE_INDEX), // Pyramid Bottom surface
    Triangle(vertices[0], vertices[3], vertices[2], Color("YELLOW"), true, 0.0f, VACUUM_REFRACTIVE_INDEX),

    Triangle(vertices[6], vertices[7], vertices[8], Color("GRAY"), true, 0.0f, VACUUM_REFRACTIVE_INDEX), // plane
};
const uint32_t triangle_number = sizeof(triangles) / sizeof(triangles[0]);

/* ----------------------------------------------------------------------*/

const float bezier_scalar = 16.0f;
const float bezier_rotation_x = 0.0f;
const float bezier_rotation_y = 0.0f;
const float bezier_rotation_z = 0.0f;
const Vector3D bezier_position = Vector3D(0.0f, -50.0f, 160.0f);

// Initialized in the main function
std::vector<BezierSurface> bezier_vector;
BezierSurface* bezier_surfaces;
uint32_t bezier_surface_number;

/* ----------------------------------------------------------------------*/

const Light lights[] = {
    Light{Vector3D(-80.0f, 80.0f, 150.0f), Color("WHITE")},
    // Light{Vector3D(80.0f, 80.0f, 250.0f), Color("CYAN")},
};
const uint32_t light_number = sizeof(lights) / sizeof(lights[0]);

/* ----------------------------------------------------------------------*/

const Camera camera(
    WIDTH, HEIGHT, 
    Vector3D(-50.0f, -50.0f, 100.0f), 
    Vector3D(50.0f, 50.0f, 100.0f), 
    Vector3D(0.0f, 0.0f, 0.0f)
);

/* ----------------------------------------------------------------------*/

void cast_ray(Ray& ray, Color& color, float incoming_refractive_index, bool passed_through_transparent, uint32_t depth_count) {
    // If the max recursive depth is exceeded, then stop
    if (depth_count > MAX_RECURSIVE_RAY_TRACING_DEPTH) {
        return;
    }

    // Avoid floating point errors
    static const float epsilon = 0.1f;
    ray.source += ray.dir * epsilon;

    uint32_t i, j;
    Intersect intersect;
    Intersect closest_intersect = {.t = INFINITY};
    Shape* closest_shape = NULL;

    // Check intersections with spheres
    for (i = 0; i < sphere_number; i++) {
        if (spheres[i].intersect(&intersect, ray) && intersect.t < closest_intersect.t) {
            closest_shape = (Shape*)(spheres+i);
            closest_intersect = intersect;
        }
    }

    // Check intersections with triangles
    for (i = 0; i < triangle_number; i++) {
        if (triangles[i].intersect(&intersect, ray) && intersect.t < closest_intersect.t) {
            closest_shape = (Shape*)(triangles+i);
            closest_intersect = intersect;
        }
    }

    // Check intersections with bezier surfaces
    for (i = 0; i < bezier_surface_number; i++) {
        if (bezier_surfaces[i].intersect(&intersect, ray) && intersect.t < closest_intersect.t) {
            closest_shape = (Shape*)(bezier_surfaces+i);
            closest_intersect = intersect;
        }
    }

    // Check if the ray hits to an object
    if (closest_shape != NULL) {
        // Add the ambient lighting once
        if (depth_count == 1) {
            color += AMBIENT_COLOR * AMBIENT_COEF;
        }

        for (i = 0; i < light_number; i++) {
            // Generate the light ray pointing from the hit location to the light source
            const Vector3D light_dir = (lights[i].position - closest_intersect.hit_location).normalize();
            const Ray light_ray = {
                .source = closest_intersect.hit_location + light_dir*epsilon,
                .dir = light_dir,
            };

            bool shadowed = false;
            float diffuse = 0.0f;
            float specular = 0.0f;

            // Check if any sphere prevents light beams from hitting to the hit location
            for (j = 0; j < sphere_number && !shadowed; j++) {
                shadowed = spheres[j].intersect(NULL, light_ray);
            }

            // Check if any triangle prevents light beams from hitting to the hit location
            for (j = 0; j < triangle_number && !shadowed; j++) {
                shadowed = triangles[j].intersect(NULL, light_ray);
            }

            // Check if any bezier surface prevents light beams from hitting to the hit location
            for (j = 0; j < bezier_surface_number && !shadowed; j++) {
                shadowed = bezier_surfaces[j].intersect(NULL, light_ray);
            }

            // If the hit location can get light beam directly from the light source
            if (!shadowed) {
                // Calculate diffuse and specular light intensity
                diffuse = DIFFUSE_COEF * greater(light_dir.dot(closest_intersect.normal), 0.0f);
                Vector3D bisector = Vector3D::bisector(light_dir, -ray.dir);
                specular = SPECULAR_COEF * powf(greater(bisector.dot(closest_intersect.normal), 0.0f), SPECULAR_POW);
            }

            // Calculate the color that will be added
            // As the recursion depth increases, the effect of reflections get smaller
            Color added_color = closest_shape->color * lights[i].color * ((diffuse+specular));
            if (!passed_through_transparent) {
                added_color *= 1.0f / (depth_count*depth_count);
            }

            color += added_color * (1.0f - closest_shape->transparency);
            bool total_reflection = false;

            if (closest_shape->transparency != 0.0f) {
                Ray refraction_ray;
                float normal_dot_coming_dir = closest_intersect.normal.dot(ray.dir);
                float sin_coming_angle = sqrtf(1.0f - normal_dot_coming_dir*normal_dot_coming_dir);
                float outgoing_refractive_index = 
                    (incoming_refractive_index == DEFAULT_REFRACTIVE_INDEX) ? closest_shape->refractive_index : DEFAULT_REFRACTIVE_INDEX;

                // Check whether there is a totla reflection or not
                total_reflection = sin_coming_angle >= closest_shape->refractive_index/incoming_refractive_index;

                if (!total_reflection) {
                    Vector3D dir_perpendicular_to_normal = (ray.dir - closest_intersect.normal*normal_dot_coming_dir).normalize();
                    Vector3D refraction_dir = (-closest_intersect.normal + 
                        dir_perpendicular_to_normal*(incoming_refractive_index/closest_shape->refractive_index*sin_coming_angle)).normalize();

                    refraction_ray = {
                        .source = closest_intersect.hit_location,
                        .dir = refraction_dir,
                    };

                    cast_ray(refraction_ray, color, closest_shape->refractive_index, true, depth_count+1);
                }
            }

            // If a total reflection occurs, or the surface is reflective
            if (total_reflection || closest_shape->reflect) {
                // Calculate the reflection ray and call recursively
                Ray reflection_ray = {
                    .source = closest_intersect.hit_location,
                    .dir = Vector3D::reflection(-ray.dir, closest_intersect.normal)
                };
                cast_ray(reflection_ray, color, incoming_refractive_index, false, depth_count+1);
            }
        }
    } // If the ray does not hit to any object, then set the color to the background color
    else if (depth_count == 1) {
        color = BACKGROUND_COLOR;
    }
}

std::vector<Vector3D> read_cubic_bezier_data(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::invalid_argument("Error opening file: " + std::string(filename));
    }

    std::vector<Vector3D> data;
    Vector3D vector;

    while (true) {
        for (uint32_t i = 0; i < 4; i++) { 
            for (uint32_t j = 0; j < 4; j++) {
                if ((file >> vector.x) && (file >> vector.y) && (file >> vector.z)) {
                    data.push_back(vector);     
                } else {
                    goto readCubicBezierSurfacesOutOfLoop;
                }
            }
        }
    }

readCubicBezierSurfacesOutOfLoop:
    file.close();
    return data;
}

void thread_function(uint32_t width_start, uint32_t width_end) {
    // Recursive ray tracing
    for (uint32_t i = width_start; i < width_end; i++) { // x axis
        for (uint32_t j = 0; j < HEIGHT; j++) { // y axis     
            // Generate a ray from camera to the center of the pixel
            Ray ray = camera.generate_ray((i+0.5f)/WIDTH, (j+0.5f)/HEIGHT);
            
            // Calculate the color of the pixel
            Color color = Color("BLACK");
            cast_ray(ray, color, DEFAULT_REFRACTIVE_INDEX, false, 1);
            
            // Write to buffer
            image[(HEIGHT-j-1)*WIDTH + i] = color;
        }
    }
}

int main(int argc, char **argv) {
    std::cout << "Processing Bezier Curves..." << std::endl;
    std::vector<Vector3D> bezier_vertices = read_cubic_bezier_data("data/bezier.txt");
    for (uint32_t i = 0; i < bezier_vertices.size(); i++) {
        bezier_vertices[i] *= bezier_scalar;
        bezier_vertices[i] = Matrix3x3::rotate(bezier_vertices[i], bezier_rotation_x, bezier_rotation_y, bezier_rotation_z);
        bezier_vertices[i] += bezier_position;
    }
    bezier_surface_number = bezier_vertices.size() / 16;

    for (uint32_t i = 0; i < bezier_surface_number; i++) {
        bezier_vector.push_back(BezierSurface(bezier_vertices.data() + 16*i, Color("MAGENTA"), false, 0.9f, GLASS_REFRACTIVE_INDEX));
    }
    bezier_surfaces = bezier_vector.data();
    bezier_vertices.clear();

    std::cout << "Rendering..." << std::endl;

    // Start threads
    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        uint32_t width_start = i*WIDTH_PER_THREAD;
        uint32_t width_end = (i+1)*WIDTH_PER_THREAD;
        threads.push_back(std::thread(thread_function, width_start, width_end));
    }

    // Wait until all threads join
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        threads[i].join();
    }

    // Write the image
    std::cout << "Writing image.png ..." << std::endl;
    stbi_write_png("image.png", WIDTH, HEIGHT, 3, image, 3*WIDTH);
    std::cout << "Finished..." << std::endl;
    return 0;
}
