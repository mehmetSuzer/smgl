
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <stb_image_write.h>

#include <point_light.h>
#include <directional_light.h>
#include <spot_light.h>

#include <camera.h>
#include <sphere.h>
#include <bezier.h>

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 8UL

#define IMAGE_HEIGHT  840UL
#define IMAGE_WIDTH   840UL

#define THREAD_NUMBER 12UL
#define WIDTH_PER_THREAD (IMAGE_WIDTH/THREAD_NUMBER)

const float GROUND_LEVEL = -50.0f;
const Color& AMBIENT_COLOR = Color::White;
const Color& BACKGROUND_COLOR = Color::Black;

Color image[IMAGE_HEIGHT * IMAGE_WIDTH] = {BACKGROUND_COLOR};
std::vector<Shape*> shapes;

/* ----------------------------------------------------------------------*/

const Vector3D vertices[] = {
            //          X                     Y                           Z
    Vector3D(60.0f,                     GROUND_LEVEL,                  220.0f), // pyramid
    Vector3D(60.0f + 40.0f*sqrtf(2.0f), GROUND_LEVEL,                  220.0f + 40.0f*sqrtf(2.0f)),
    Vector3D(60.0f,                     GROUND_LEVEL,                  220.0f + 80.0f*sqrtf(2.0f)),
    Vector3D(60.0f - 40.0f*sqrtf(2.0f), GROUND_LEVEL,                  220.0f + 40.0f*sqrtf(2.0f)),
    Vector3D(60.0f,                     GROUND_LEVEL + 50*sqrtf(2.0f), 220.0f + 40.0f*sqrtf(2.0f)),

    Vector3D(0.0f,                      GROUND_LEVEL,                  -100.0f), // plane
    Vector3D(-1000.0f,                  GROUND_LEVEL,                  1000.0f),
    Vector3D(1000.0f,                   GROUND_LEVEL,                  1000.0f),
};

/* ----------------------------------------------------------------------*/

const float pyramidReflectivity = 0.0f;
const float pyramidTransparency = 0.0f;
const Triangle triangles[] = {
    Triangle(vertices[0], vertices[1], vertices[4], Color::Cyan, pyramidReflectivity, pyramidTransparency, GLASS_REFRACTIVE_INDEX), // pyramid side surfaces
    Triangle(vertices[1], vertices[2], vertices[4], Color::Green, pyramidReflectivity, pyramidTransparency, GLASS_REFRACTIVE_INDEX),
    Triangle(vertices[2], vertices[3], vertices[4], Color::Blue, pyramidReflectivity, pyramidTransparency, GLASS_REFRACTIVE_INDEX),
    Triangle(vertices[0], vertices[3], vertices[4], Color::Red, pyramidReflectivity, pyramidTransparency, GLASS_REFRACTIVE_INDEX),

    Triangle(vertices[0], vertices[1], vertices[2], Color::Magenta, pyramidReflectivity, pyramidTransparency, GLASS_REFRACTIVE_INDEX), // pyramid bottom surface
    Triangle(vertices[0], vertices[3], vertices[2], Color::Yellow, pyramidReflectivity, pyramidTransparency, GLASS_REFRACTIVE_INDEX),

    Triangle(vertices[5], vertices[6], vertices[7], Color::Gray, 0.4f, 0.0f, GLASS_REFRACTIVE_INDEX), // plane
};

/* ----------------------------------------------------------------------*/

const Sphere spheres[] = {
    Sphere(Vector3D(-50.0f, GROUND_LEVEL + 50.0f, 220.0f), 50.0f, Color::Yellow, 0.25f, 0.0f, GLASS_REFRACTIVE_INDEX),
    // Sphere(Vector3D(50.0f, GROUND_LEVEL + 50.0f, 220.0f), 50.0f, Color::Green, 0.4f, 0.0f, GLASS_REFRACTIVE_INDEX),
    // Sphere(Vector3D(0.0f, GROUND_LEVEL + 20.0f, 120.0f), 20.0f, Color::Blue, 0.0f, 0.99f, GLASS_REFRACTIVE_INDEX),
};

/* ----------------------------------------------------------------------*/

const AABB aabbs[] = {
    // AABB(Vector3D(-50.0f, GROUND_LEVEL + EPSILON4, 160.0f), Vector3D(-30.0f, GROUND_LEVEL + 10.0f, 170.0f), Color::Green, 0.0f, 0.0f, GLASS_REFRACTIVE_INDEX),
    AABB(Vector3D(-70.0f, GROUND_LEVEL + EPSILON4, 160.0f), Vector3D(70.0f, GROUND_LEVEL + 90.0f, 170.0f), Color::Blue, 0.0f, 0.99f, GLASS_REFRACTIVE_INDEX),
};

/* ----------------------------------------------------------------------*/

const float bezierScalar = 16.0f;
const float bezierRadianX = 0.0f;
const float bezierRadianY = 0.0f;
const float bezierRadianZ = 0.0f;
const uint32_t bezierSubdivision = 4;
const Color& bezierColor = Color::Cyan;
const float bezierReflectivity = 0.0f;
const float bezierTransparency = 0.99f;
const float bezierRefractiveIndex = GLASS_REFRACTIVE_INDEX;
const Vector3D bezierPosition = Vector3D(0.0f, GROUND_LEVEL + 30.0f, 130.0f);

/* ----------------------------------------------------------------------*/

const PointLight pointLight = PointLight(
    Vector3D(-40.0f, GROUND_LEVEL + 100.0f, 0.0f), 
    Color::White, 
    2E-5f, 1E-5f
);

const DirectionalLight directionalLight = DirectionalLight(
    Vector3D(sqrtf(0.5f), 0.0f, sqrtf(0.5f)), 
    Color::White, 
    1.0f
);
const SpotLight spotLight = SpotLight(
    Vector3D(0.0f, GROUND_LEVEL + 250.0f, 150.0f), 
    Color::White, 
    2E-5f, 1E-5f, 
    Vector3D(0.0f, -1.0f, 0.0f), 
    M_PIf/3.0f
);

const Light* lights[] = {
    (Light*)&pointLight,
    // (Light*)&directionalLight,
    // (Light*)&spotLight,
};
const uint32_t lightNumber = sizeof(lights) / sizeof(Light*);

/* ----------------------------------------------------------------------*/

const Camera camera = Camera(
    Vector3D(0.0f, 0.0f, 0.0f), // position
    Vector3D(0.0f, 0.0f, 1.0f), // direction
    Vector3D(0.0f, 1.0f, 0.0f), // up
    1.0f,                       // near
    INFINITY,                   // far
    M_PIf / 3.0f,               // FOV
    IMAGE_WIDTH, 
    IMAGE_HEIGHT
);

/* ----------------------------------------------------------------------*/

void traceRay(const Ray& ray, Color& color, float incomingRefractiveIndex, float coefficientOfPreviousShape, uint32_t depthCount) {
    // If the max recursive depth is exceeded, then stop tracing
    if (depthCount > MAX_RECURSIVE_RAY_TRACING_DEPTH) {
        return;
    }

    uint32_t i, j;
    Intersect intersect;
    Intersect closestIntersect = {.t = INFINITY};
    Shape* closestShape = NULL;

    // Check whether the ray intersects with a shape
    for (i = 0; i < shapes.size(); i++) {
        if (shapes[i]->intersect(&intersect, ray, camera.getFar()) && intersect.t < closestIntersect.t) {
            closestShape = shapes[i];
            closestIntersect = intersect;
        }
    }

    // Check if the ray hits to an object
    if (closestShape != NULL) {
        // Add the ambient lighting once
        if (depthCount == 1) {
            color += AMBIENT_COLOR * AMBIENT_COEF;
        }

        for (i = 0; i < lightNumber; i++) {
            const LightInfo lightInfo = lights[i]->shine(closestIntersect.hitLocation);
            if (lightInfo.distance == -INFINITY) { // Light does not hit to the hit location
                continue;
            }
            const Ray lightRay = {
                .origin = closestIntersect.hitLocation + closestIntersect.normal * EPSILON3,
                .dir = lightInfo.directionToLight,
            };
            float shadowingShapeTransparency = 1.0f;

            // Check if a shape prevents light rays from hitting to the hit location
            for (j = 0; j < shapes.size() && shadowingShapeTransparency == 1.0f; j++) {
                if (shapes[j]->intersect(NULL, lightRay, lightInfo.distance)) {
                    shadowingShapeTransparency = shapes[j]->getTransparency();
                }
            }

            // Calculate diffuse and specular light intensity
            const float diffuse = DIFFUSE_COEF * greater(lightInfo.directionToLight.dot(closestIntersect.normal), 0.0f);
            const Vector3D bisector = Vector3D::bisector(lightInfo.directionToLight, -ray.dir);
            const float specular = SPECULAR_COEF * powf(greater(bisector.dot(closestIntersect.normal), 0.0f), SPECULAR_POW);
            
            // Calculate the color that will be added
            const Color addedColor = closestShape->getColor() * lights[i]->getColor()
                * ((diffuse + specular) 
                * lightInfo.intensity                           // As intensity of the light increases, the point looks brighter
                * coefficientOfPreviousShape                    // As the reflectivity and the transparency of the previous shape increases, the current object gets more visible
                * shadowingShapeTransparency                    // If an object casts shadow onto the point, the point looks dimmer
                * (1.0f - closestShape->getTransparency()       // As the transparency of the shape increases, its color gets less visible
                        - closestShape->getReflectivity()));    // As the reflectivity of the shape increases, its color gets less visible
            color += addedColor;
        }

        float reflectivePortion = closestShape->getReflectivity();
        if (closestShape->getTransparency() > 0.0f) {
            const float normalDotComingRayDir = closestIntersect.normal.dot(ray.dir);
            const float sinComingAngle = sqrtf(1.0f - normalDotComingRayDir * normalDotComingRayDir);
            const float outgoingRefractiveIndex = 
                (incomingRefractiveIndex == WORLD_REFRACTIVE_INDEX) ? closestShape->getRefractiveIndex() : WORLD_REFRACTIVE_INDEX;
            const float outgoingToIncomingRefractiveIndexRatio = outgoingRefractiveIndex / incomingRefractiveIndex;

            // If there is no total reflection, then calculate the refractive ray and call the function recursively
            if (sinComingAngle < outgoingToIncomingRefractiveIndexRatio) {
                const Vector3D dirPerpendicularComponentToNormal = ray.dir - closestIntersect.normal * normalDotComingRayDir;
                const Vector3D refractiveRayDir = 
                    (-closestIntersect.normal + dirPerpendicularComponentToNormal / outgoingToIncomingRefractiveIndexRatio).normalize();
                const Ray refractiveRay = {
                    .origin = closestIntersect.hitLocation + refractiveRayDir * EPSILON3,
                    .dir = refractiveRayDir,
                };

                traceRay(
                    refractiveRay, 
                    color, 
                    outgoingRefractiveIndex, 
                    coefficientOfPreviousShape * closestShape->getTransparency(), 
                    depthCount+1
                );
            } else { // A total reflection occurs
                reflectivePortion += closestShape->getTransparency();
            }
        }

        // If the ray reflects from the point, then calculat the refleective ray and call the function recursively
        if (reflectivePortion > 0.0f) {
            const Vector3D reflectiveDir = Vector3D::reflection(-ray.dir, closestIntersect.normal);
            const Ray reflectiveRay = {
                .origin = closestIntersect.hitLocation + reflectiveDir * EPSILON3,
                .dir = reflectiveDir,
            };

            traceRay(
                reflectiveRay, 
                color, 
                incomingRefractiveIndex, 
                coefficientOfPreviousShape * reflectivePortion,
                depthCount+1
            );
        }
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
void threadFunction(uint32_t widthStart, uint32_t widthEnd) {
    for (uint32_t i = widthStart; i < widthEnd; i++) { // x axis
        for (uint32_t j = 0; j < IMAGE_HEIGHT; j++) { // y axis     
            // Generate a ray from camera to the center of the pixel
            const float x = (i+0.5f) / IMAGE_WIDTH;
            const float y = (j+0.5f) / IMAGE_HEIGHT;
            const Ray ray = camera.generateRay(x, y);
            
            // Calculate the color of the pixel
            Color& color = image[(IMAGE_HEIGHT-j-1)*IMAGE_WIDTH + i];
            traceRay(ray, color, WORLD_REFRACTIVE_INDEX, 1.0f, 1);
        }
    }
}

int main(int argc, char **argv) {
    // Start timing
    std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Scale, rotate, and translate the cubic bezier vertices that are read from the file
    std::vector<Vector3D> bezierVertices = readCubicBezierData("data/bezier.txt");
    for (uint32_t i = 0; i < bezierVertices.size(); i++) {
        bezierVertices[i] *= bezierScalar;
        bezierVertices[i].rotate(bezierRadianX, bezierRadianY, bezierRadianZ);
        bezierVertices[i] += bezierPosition;
    }

    // 16 consecutive vertices define a cubic bezier surface
    const uint32_t bezierSurfaceNumber = bezierVertices.size() >> 4; // divide by 16
    std::vector<BezierSurface> bezierSurfaces; 
    for (uint32_t i = 0; i < bezierSurfaceNumber; i++) {
        bezierSurfaces.push_back( 
            BezierSurface(
                bezierVertices.data() + (i << 4), 
                bezierSubdivision, 
                bezierColor, 
                bezierReflectivity, 
                bezierTransparency, 
                bezierRefractiveIndex
            )
        );
    }
    bezierVertices.clear();

    // Move spheres, aabbs, triangles, and bezier surfaces to shapes vector
    for (uint32_t i = 0; i < sizeof(spheres) / sizeof(Sphere); i++) {
        shapes.push_back((Shape*)(spheres+i));
    }
    for (uint32_t i = 0; i < sizeof(aabbs) / sizeof(AABB); i++) {
        shapes.push_back((Shape*)(aabbs+i));
    }
    for (uint32_t i = 0; i < sizeof(triangles) / sizeof(Triangle); i++) {
        shapes.push_back((Shape*)(triangles+i));
    }
    for (uint32_t i = 0; i < bezierSurfaces.size(); i++) {
        shapes.push_back((Shape*)(bezierSurfaces.data()+i));
    }

    std::cout << "Rendering..." << std::endl;

    // Start threads
    std::vector<std::thread> threads;
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        const uint32_t widthStart = i * WIDTH_PER_THREAD;
        const uint32_t widthEnd = widthStart + WIDTH_PER_THREAD;
        threads.push_back(std::thread(threadFunction, widthStart, widthEnd));
    }

    // Wait until all threads join
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        threads[i].join();
    }

    // Write the image
    std::cout << "Writing image.png..." << std::endl;
    stbi_write_png("image.png", IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, 3*IMAGE_WIDTH);

    // Stop timing
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    float durationInSeconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1E6f;
    std::cout << "Finished in " << durationInSeconds << " seconds..." << std::endl;

    return 0;
}
