
#include <iostream>
#include <fstream>
#include <stb_image_write.h>

#include <camera.h>
#include <sphere.h>
#include <triangle.h>
#include <bezier.h>

#define smaller(x, y) ((x > y) ? y : x)
#define greater(x, y) ((x > y) ? x : y)

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 3UL

#define HEIGHT  600UL
#define WIDTH   600UL

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
    Sphere(vertices[5], 25.0f, Color("GREEN"), true),
    Sphere(vertices[6], 25.0f, Color("YELLOW"), true),
};
const uint32_t sphere_number = sizeof(spheres) / sizeof(spheres[0]);

/* ----------------------------------------------------------------------*/

const Triangle triangles[] = {
    Triangle(vertices[0], vertices[1], vertices[4], Color("RED"), true), // Pyramid Side surfaces
    Triangle(vertices[1], vertices[2], vertices[4], Color("GREEN"), true),
    Triangle(vertices[2], vertices[3], vertices[4], Color("BLUE"), true),
    Triangle(vertices[0], vertices[3], vertices[4], Color("MAGENTA"), true),

    Triangle(vertices[0], vertices[1], vertices[2], Color("CYAN"), true), // Pyramid Bottom surface
    Triangle(vertices[0], vertices[3], vertices[2], Color("YELLOW"), true),

    Triangle(vertices[9], vertices[10], vertices[11], Color("GRAY"), true), // plane
};
const uint32_t triangle_number = sizeof(triangles) / sizeof(triangles[0]);

/* ----------------------------------------------------------------------*/

// Initialized in the main function
std::vector<BezierSurface> bezier_vector;
BezierSurface* beziers;
uint32_t bezier_number;

/* ----------------------------------------------------------------------*/

const Light lights[] = {
    Light{vertices[7], Color("WHITE")},
    Light{vertices[8], Color("CYAN")},
};
const uint32_t light_number = sizeof(lights) / sizeof(lights[0]);

/* ----------------------------------------------------------------------*/

const Camera camera(
    WIDTH, HEIGHT, 
    Vector3D(-50.0f, -50.0f, 100.0f), 
    Vector3D(50.0f, 50.0f, 100.0f), 
    Vector3D(0.0f, 0.0f, 0.0f)
);

void cast_ray(Ray& ray, Color& color, uint32_t depth_count) {
    // If the max recursive depth is exceeded, then stop
    if (depth_count > MAX_RECURSIVE_RAY_TRACING_DEPTH) {
        return;
    }

    const float epsilon = 0.1f;
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

    // Check intersections with beziers
    for (i = 0; i < bezier_number; i++) {
        if (beziers[i].intersect(&intersect, ray) && intersect.t < closest_intersect.t) {
            closest_shape = (Shape*)(beziers+i);
            closest_intersect = intersect;
        }
    }

    // Check if the ray hits to an object
    if (closest_shape != NULL) {
        // Add the ambient lighting once
        if (depth_count == 1) {
            color += AMBIENT_COLOR*AMBIENT_COEF;
        }

        for (i = 0; i < light_number; i++) {
            // Generate the light ray pointing from the hit location to the light source
            const Vector3D light_dir = (lights[i].position - closest_intersect.hit_location).normalize();
            const Ray hit_ray = {
                .source = closest_intersect.hit_location + light_dir*epsilon,
                .dir = light_dir,
            };

            bool shadowed = false;
            float diffuse = 0.0f;
            float specular = 0.0f;

            // Check if any sphere prevents light beams from hitting to the hit location
            for (j = 0; j < sphere_number && !shadowed; j++) {
                shadowed = spheres[j].intersect(NULL, hit_ray);
            }

            // Check if any triangle prevents light beams from hitting to the hit location
            for (j = 0; j < triangle_number && !shadowed; j++) {
                shadowed = triangles[j].intersect(NULL, hit_ray);
            }

            // Check if any bezier prevents light beams from hitting to the hit location
            for (j = 0; j < bezier_number && !shadowed; j++) {
                shadowed = beziers[j].intersect(NULL, hit_ray);
            }

            // If the hit location can get light beam directly from the light source
            if (!shadowed) {
                // Calculate diffuse and specular light intensity
                diffuse = DIFFUSE_COEF * greater(light_dir.dot(closest_intersect.normal), 0.0f);
                Vector3D bisector = Vector3D::bisector(light_dir, -ray.dir);
                specular = SPECULAR_COEF * powf(greater(bisector.dot(closest_intersect.normal), 0.0f), SPECULAR_POW);
            }

            // Update the color
            // As the recursion depth increases, the effect of reflections get smaller
            color += closest_shape->color * lights[i].color * ((diffuse+specular) / (depth_count*depth_count));

            // If the surface is reflective
            if (closest_shape->reflect) {
                // Calculate the new ray and call recursively
                Ray new_ray = {
                    .source = closest_intersect.hit_location,
                    .dir = Vector3D::reflection(-ray.dir, closest_intersect.normal)
                };
                cast_ray(new_ray, color, depth_count+1);
            }
        }
    } // If the ray does not hit to any object, then set the color to the background color
    else if (depth_count == 1) {
        color = BACKGROUND_COLOR;
    }
}

std::vector<Vector3D> readCubicBezierData(const char* filename) {
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

int main(int argc, char **argv) {
    Color image[HEIGHT*WIDTH];

    std::cout << "Processing Bezier Curves..." << std::endl;
    const Vector3D bezier_position = Vector3D(0.0f, 0.0f, 100.0f);
    const float bezier_scalar = 6.0f;

    std::vector<Vector3D> bezier_vertices = readCubicBezierData("data/bezier.txt");
    for (uint32_t i = 0; i < bezier_vertices.size(); i++) {
        bezier_vertices[i] *= bezier_scalar;
        bezier_vertices[i] += bezier_position;
    }

    bezier_number = bezier_vertices.size() / 16;
    for (uint32_t i = 0; i < bezier_number; i++) {
        bezier_vector.push_back(BezierSurface(bezier_vertices.data() + 16*i, Color("RED"), true));
    }
    beziers = bezier_vector.data();
    bezier_vertices.clear();

    std::cout << "Rendering..." << std::endl;

    // Recursive ray tracing
    for (uint32_t i = 0; i < WIDTH; i++) { // x axis
        for (uint32_t j = 0; j < HEIGHT; j++) { // y axis     
            // Generate a ray from camera to the center of the pixel
            Ray ray = camera.generate_ray((i+0.5f)/WIDTH, (j+0.5f)/HEIGHT);
            
            // Calculate the color of the pixel
            Color color = Color("BLACK");
            cast_ray(ray, color, 1);
            
            // Write to buffer
            image[(HEIGHT-j-1)*WIDTH + i] = color;
        }
        if ((i+1)%(WIDTH/10) == 0) {
            std::cout << 100*(i+1)/WIDTH << " % is completed..." << std::endl;
        }
    }

    std::cout << "Writing image.png ..." << std::endl;
    stbi_write_png("image.png", WIDTH, HEIGHT, 3, image, 3*WIDTH);
    std::cout << "Finished..." << std::endl;
    return 0;
}
