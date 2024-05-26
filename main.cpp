
#include <iostream>
#include <fstream>
#include <thread>
#include <stb_image_write.h>

#include <camera.h>
#include <sphere.h>
#include <triangle.h>
#include <bezier.h>

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 4UL

#define IMAGE_HEIGHT  600UL
#define IMAGE_WIDTH   600UL

#define THREAD_NUMBER 12UL
#define WIDTH_PER_THREAD (IMAGE_WIDTH/THREAD_NUMBER)

const float GROUND_LEVEL = -50.0f;
const Color& AMBIENT_COLOR = Color::White;
const Color& BACKGROUND_COLOR = Color::Black;
Color image[IMAGE_HEIGHT * IMAGE_WIDTH] = {Color(0,0,0)};

/* ----------------------------------------------------------------------*/

const Vector3D vertices[] = {
            //          X                     Y                           Z
    Vector3D(60.0f,                     GROUND_LEVEL,                  180.0f), // pyramid
    Vector3D(60.0f + 40.0f*sqrtf(2.0f), GROUND_LEVEL,                  180.0f + 40.0f*sqrtf(2.0f)),
    Vector3D(60.0f,                     GROUND_LEVEL,                  180.0f + 80.0f*sqrtf(2.0f)),
    Vector3D(60.0f - 40.0f*sqrtf(2.0f), GROUND_LEVEL,                  180.0f + 40.0f*sqrtf(2.0f)),
    Vector3D(60.0f,                     GROUND_LEVEL + 50*sqrtf(2.0f), 180.0f + 40.0f*sqrtf(2.0f)),

    Vector3D(0.0f,                      GROUND_LEVEL,                  -100.0f), // plane
    Vector3D(-1000.0f,                  GROUND_LEVEL,                  1000.0f),
    Vector3D(1000.0f,                   GROUND_LEVEL,                  1000.0f),
};
const uint32_t vertex_number = sizeof(vertices) / sizeof(vertices[0]);

/* ----------------------------------------------------------------------*/

const Sphere spheres[] = {
    Sphere(Vector3D(-30.0f, GROUND_LEVEL + 50.0f, 200.0f), 50.0f, Color::Yellow, true, 0.0f, VACUUM_REFRACTIVE_INDEX),
};
const uint32_t sphere_number = sizeof(spheres) / sizeof(spheres[0]);

/* ----------------------------------------------------------------------*/

const Triangle triangles[] = {
    Triangle(vertices[0], vertices[1], vertices[4], Color::Cyan, true, 0.0f, VACUUM_REFRACTIVE_INDEX), // pyramid side surfaces
    Triangle(vertices[1], vertices[2], vertices[4], Color::Green, true, 0.0f, VACUUM_REFRACTIVE_INDEX),
    Triangle(vertices[2], vertices[3], vertices[4], Color::Blue, true, 0.0f, VACUUM_REFRACTIVE_INDEX),
    Triangle(vertices[0], vertices[3], vertices[4], Color::Red, true, 0.0f, VACUUM_REFRACTIVE_INDEX),

    Triangle(vertices[0], vertices[1], vertices[2], Color::Magenta, true, 0.0f, VACUUM_REFRACTIVE_INDEX), // pyramid bottom surface
    Triangle(vertices[0], vertices[3], vertices[2], Color::Yellow, true, 0.0f, VACUUM_REFRACTIVE_INDEX),

    Triangle(vertices[5], vertices[6], vertices[7], Color::Gray, true, 0.0f, VACUUM_REFRACTIVE_INDEX), // plane
};
const uint32_t triangle_number = sizeof(triangles) / sizeof(triangles[0]);

/* ----------------------------------------------------------------------*/

const float bezier_scalar = 20.0f;
const float bezier_rotation_x = 0.0f;
const float bezier_rotation_y = 0.0f;
const float bezier_rotation_z = 0.0f;
const uint32_t bezier_subdivision = 4;
const float bezier_transparency = 0.99f;
const float bezier_refractive_index = GLASS_REFRACTIVE_INDEX;
const Vector3D bezier_position = Vector3D(0.0f, GROUND_LEVEL + 10.0f, 140.0f);

// Initialized in the main function
std::vector<BezierSurface> bezier_vector;
BezierSurface* bezier_surfaces;
uint32_t bezier_surface_number;

/* ----------------------------------------------------------------------*/

const Light lights[] = {
    Light{Vector3D(0.0f, GROUND_LEVEL + 120.0f, 80.0f), Color::White},
    // Light{Vector3D(80.0f, GROUND_LEVEL + 150.0f, 250.0f), Color::Cyan},
};
const uint32_t light_number = sizeof(lights) / sizeof(lights[0]);

/* ----------------------------------------------------------------------*/

const Camera camera = Camera(
    Vector3D(0.0f, 0.0f, 0.0f), // position
    Vector3D(0.0f, 0.0f, 1.0f), // direction
    Vector3D(0.0f, 1.0f, 0.0f), // up
    1.0f,                       // near
    M_PIf / 3.0f,               // FOV
    IMAGE_WIDTH, 
    IMAGE_HEIGHT
);

/* ----------------------------------------------------------------------*/

void traceRay(Ray& ray, Color& color, float incoming_refractive_index, uint32_t depth_count) {
    // If the max recursive depth is exceeded, then stop
    if (depth_count > MAX_RECURSIVE_RAY_TRACING_DEPTH) {
        return;
    }

    // Avoid floating point errors
    static const float epsilon = 0.1f;
    ray.origin += ray.dir * epsilon;

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
            const Vector3D light_vector = lights[i].position - closest_intersect.hit_location;
            const Vector3D light_dir = light_vector.normalize();
            const Ray light_ray = {
                .origin = closest_intersect.hit_location + light_dir*epsilon,
                .dir = light_dir,
            };

            float shadowing_shape_transparency = 1.0f;
            // Check if any sphere prevents light beams from hitting to the hit location
            for (j = 0; j < sphere_number && shadowing_shape_transparency == 1.0f; j++) {
                if (spheres[j].intersect(NULL, light_ray)) {
                    shadowing_shape_transparency = spheres[j].getTransparency();
                }
            }

            // Check if any triangle prevents light beams from hitting to the hit location
            for (j = 0; j < triangle_number && shadowing_shape_transparency == 1.0f; j++) {
                if (triangles[j].intersect(NULL, light_ray)) {
                    shadowing_shape_transparency = triangles[j].getTransparency();
                }
            }

            // Check if any bezier surface prevents light beams from hitting to the hit location
            for (j = 0; j < bezier_surface_number && shadowing_shape_transparency == 1.0f; j++) {
                if (bezier_surfaces[j].intersect(NULL, light_ray)) {
                    shadowing_shape_transparency = bezier_surfaces[j].getTransparency();
                }
            }

            // Calculate diffuse and specular light intensity
            const float diffuse = DIFFUSE_COEF * greater(light_dir.dot(closest_intersect.normal), 0.0f);
            const Vector3D bisector = Vector3D::bisector(light_dir, -ray.dir);
            const float specular = SPECULAR_COEF * powf(greater(bisector.dot(closest_intersect.normal), 0.0f), SPECULAR_POW);
            
            // Calculate the intensity
	        const float a = 0.00002f;
	        const float b = 0.00001f;
            const float distance = light_vector.mag();
	        const float inverse_intensity = (a * distance + b) * distance + 1.0f; // inverse_intensity = 1/intensity

            // Calculate the color that will be added
            const Color added_color = closest_shape->getColor() * lights[i].color 
                * ((diffuse + specular) 
                / inverse_intensity                             // Intensity of the light at the point
                / (depth_count * depth_count)                   // As the recursion depth increases, the effect of reflections get smaller
                * shadowing_shape_transparency                  // If an object casts shadow onto the point, use its transparency
                * (1.0f - closest_shape->getTransparency()));   // Opacity of the object contributes to the color at the point
            color += added_color;
        }

        bool total_reflection = false;
        if (closest_shape->getTransparency() != 0.0f) {
            Ray refraction_ray;
            const float normal_dot_coming_dir = closest_intersect.normal.dot(ray.dir);
            const float sin_coming_angle = sqrtf(1.0f - normal_dot_coming_dir*normal_dot_coming_dir);
            const float outgoing_refractive_index = 
                (incoming_refractive_index == WORLD_REFRACTIVE_INDEX) ? closest_shape->getRefractiveIndex() : WORLD_REFRACTIVE_INDEX;

            // Check whether there is a total reflection or not
            total_reflection = sin_coming_angle >= outgoing_refractive_index/incoming_refractive_index;

            // If there is no total reflection, then calculate the ray and call the function recursively
            if (!total_reflection) {
                const Vector3D dir_perpendicular_to_normal = (ray.dir - closest_intersect.normal*normal_dot_coming_dir).normalize();
                const Vector3D refraction_dir = (-closest_intersect.normal + 
                    dir_perpendicular_to_normal*(incoming_refractive_index/outgoing_refractive_index*sin_coming_angle)).normalize();

                refraction_ray = {
                    .origin = closest_intersect.hit_location,
                    .dir = refraction_dir,
                };

                // Recursion depth does not increase since we want objects behind a transparent object to contribute more
                traceRay(refraction_ray, color, outgoing_refractive_index, depth_count);
            }
        }

        // If a total reflection occurs, or the surface is reflective
        if (total_reflection || closest_shape->isReflective()) {
            // Calculate the reflection ray and call the function recursively
            Ray reflection_ray = {
                .origin = closest_intersect.hit_location,
                .dir = Vector3D::reflection(-ray.dir, closest_intersect.normal)
            };
            traceRay(reflection_ray, color, incoming_refractive_index, depth_count+1);
        }
    } // If the ray does not hit to any object, then set the color to the background color
    else if (depth_count == 1) {
        color = BACKGROUND_COLOR;
    }
}

// Reads the cubic beziers and returns them in a vector
std::vector<Vector3D> readCubicBezierData(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::invalid_argument("Error opening file: " + std::string(filename));
    }

    std::vector<Vector3D> data;
    Vector3D vector;

    while (true) {
        for (uint32_t i = 0; i < 16; i++) { 
            if ((file >> vector.x) && (file >> vector.y) && (file >> vector.z)) {
                data.push_back(vector);     
            } else {
                goto readCubicBezierSurfacesOutOfLoop;
            }
        }
    }

readCubicBezierSurfacesOutOfLoop:
    file.close();
    return data;
}

// The function which renders a portion of the image
void thread_function(uint32_t width_start, uint32_t width_end) {
    // Recursive ray tracing
    for (uint32_t i = width_start; i < width_end; i++) { // x axis
        for (uint32_t j = 0; j < IMAGE_HEIGHT; j++) { // y axis     
            // Generate a ray from camera to the center of the pixel
            const float x = (i+0.5f) / IMAGE_WIDTH;
            const float y = (j+0.5f) / IMAGE_HEIGHT;
            Ray ray = camera.generateRay(x, y);
            
            // Calculate the color of the pixel
            Color& color = image[(IMAGE_HEIGHT-j-1)*IMAGE_WIDTH + i];
            traceRay(ray, color, WORLD_REFRACTIVE_INDEX, 1);
        }
    }
}

int main(int argc, char **argv) {
    std::cout << "Processing Bezier Curves..." << std::endl;
    std::vector<Vector3D> bezier_vertices = readCubicBezierData("data/bezier.txt");
    for (uint32_t i = 0; i < bezier_vertices.size(); i++) {
        bezier_vertices[i] *= bezier_scalar;
        bezier_vertices[i].rotate(bezier_rotation_x, bezier_rotation_y, bezier_rotation_z);
        bezier_vertices[i] += bezier_position;
    }
    bezier_surface_number = bezier_vertices.size() >> 4; // divide by 16

    for (uint32_t i = 0; i < bezier_surface_number; i++) {
        bezier_vector.push_back(
            BezierSurface(
                bezier_vertices.data() + (i << 4), 
                bezier_subdivision, 
                Color::Cyan, 
                false, 
                bezier_transparency, 
                bezier_refractive_index
            )
        );
    }
    bezier_surfaces = bezier_vector.data();
    bezier_vertices.clear();

    std::cout << "Rendering..." << std::endl;

    // Start threads
    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        const uint32_t width_start = i * WIDTH_PER_THREAD;
        const uint32_t width_end = width_start + WIDTH_PER_THREAD;
        threads.push_back(std::thread(thread_function, width_start, width_end));
    }

    // Wait until all threads join
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        threads[i].join();
    }

    // Write the image
    std::cout << "Writing image.png ..." << std::endl;
    stbi_write_png("image.png", IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, 3*IMAGE_WIDTH);
    std::cout << "Finished..." << std::endl;
    
    return 0;
}
