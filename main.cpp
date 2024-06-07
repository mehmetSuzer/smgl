
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

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 4UL

#define IMAGE_HEIGHT  840UL
#define IMAGE_WIDTH   840UL

#define THREAD_NUMBER 12UL
#define WIDTH_PER_THREAD (IMAGE_WIDTH/THREAD_NUMBER)

const float GROUND_LEVEL = -50.0f;
const Color& AMBIENT_COLOR = Color::White;
const Color& BACKGROUND_COLOR = Color::Black;
Color image[IMAGE_HEIGHT * IMAGE_WIDTH] = {BACKGROUND_COLOR};

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
const uint32_t vertexNumber = sizeof(vertices) / sizeof(vertices[0]);

/* ----------------------------------------------------------------------*/

const bool pyramidReflect = true;
const float pyramidTransparency = 0.0f;
const Triangle triangles[] = {
    Triangle(vertices[0], vertices[1], vertices[4], Color::Cyan, pyramidReflect, pyramidTransparency, GLASS_REFRACTIVE_INDEX), // pyramid side surfaces
    Triangle(vertices[1], vertices[2], vertices[4], Color::Green, pyramidReflect, pyramidTransparency, GLASS_REFRACTIVE_INDEX),
    Triangle(vertices[2], vertices[3], vertices[4], Color::Blue, pyramidReflect, pyramidTransparency, GLASS_REFRACTIVE_INDEX),
    Triangle(vertices[0], vertices[3], vertices[4], Color::Red, pyramidReflect, pyramidTransparency, GLASS_REFRACTIVE_INDEX),

    Triangle(vertices[0], vertices[1], vertices[2], Color::Magenta, pyramidReflect, pyramidTransparency, GLASS_REFRACTIVE_INDEX), // pyramid bottom surface
    Triangle(vertices[0], vertices[3], vertices[2], Color::Yellow, pyramidReflect, pyramidTransparency, GLASS_REFRACTIVE_INDEX),

    Triangle(vertices[5], vertices[6], vertices[7], Color::Gray, true, 0.0f, GLASS_REFRACTIVE_INDEX), // plane
};
const uint32_t triangleNumber = sizeof(triangles) / sizeof(triangles[0]);

/* ----------------------------------------------------------------------*/

const Sphere spheres[] = {
    Sphere(Vector3D(-50.0f, GROUND_LEVEL + 50.0f, 220.0f), 50.0f, Color::Yellow, true, 0.0f, GLASS_REFRACTIVE_INDEX),
};
const uint32_t sphereNumber = sizeof(spheres) / sizeof(spheres[0]);

/* ----------------------------------------------------------------------*/

const AABB aabbs[] = {
    AABB(Vector3D(-50.0f, GROUND_LEVEL + EPSILON4, 160.0f), Vector3D(50.0f, GROUND_LEVEL + 20.0f, 170.0f), Color::Red, false, 0.0f, GLASS_REFRACTIVE_INDEX),
    AABB(Vector3D(-10.0f, GROUND_LEVEL + EPSILON4, 140.0f), Vector3D(30.0f, GROUND_LEVEL + 90.0f, 155.0f), Color::Blue, false, 0.95f, GLASS_REFRACTIVE_INDEX),
};
const uint32_t aabbNumber = sizeof(aabbs) / sizeof(aabbs[0]);

/* ----------------------------------------------------------------------*/

const float bezierScalar = 16.0f;
const float bezierRadianX = 0.0f;
const float bezierRadianY = 0.0f;
const float bezierRadianZ = 0.0f;
const uint32_t bezierSubdivision = 4;
const float bezierTransparency = 0.99f;
const float bezierRefractiveIndex = GLASS_REFRACTIVE_INDEX;
const Vector3D bezierPosition = Vector3D(0.0f, GROUND_LEVEL, 115.0f);

// Initialized in the main function
std::vector<BezierSurface> bezierVector;
BezierSurface* bezierSurfaces;
uint32_t bezierSurfaceNumber = 0;

/* ----------------------------------------------------------------------*/

const PointLight pointLight = PointLight(Vector3D(0.0f, GROUND_LEVEL + 40.0f, 0.0f), Color::White, 2E-5f, 1E-5f);
const DirectionalLight directionalLight = DirectionalLight(Vector3D(sqrtf(0.5f), 0.0f, sqrtf(0.5f)), Color::White, 1.0f);
const SpotLight spotLight = SpotLight(Vector3D(0.0f, GROUND_LEVEL + 250.0f, 150.0f), Color::White, 2E-5f, 1E-5f, Vector3D(0.0f, -1.0f, 0.0f), M_PIf/3.0f);

const Light* lights[] = {
    (Light*)&pointLight,
    // (Light*)&directionalLight,
    // (Light*)&spotLight,
};
const uint32_t lightNumber = sizeof(lights) / sizeof(lights[0]);

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

// Recursive ray tracers that supports reflection and refraction
void traceRay(Ray& ray, Color& color, float incomingRefractiveIndex, uint32_t depthCount) {
    // If the max recursive depth is exceeded, then stop tracing
    if (depthCount > MAX_RECURSIVE_RAY_TRACING_DEPTH) {
        return;
    }

    // Avoid floating point errors
    ray.origin += ray.dir * EPSILON3;

    uint32_t i, j;
    Intersect intersect;
    Intersect closestIntersect = {.t = INFINITY};
    Shape* closestShape = NULL;

    // Check intersections with spheres
    for (i = 0; i < sphereNumber; i++) {
        if (spheres[i].intersect(&intersect, ray, camera.getFar()) && intersect.t < closestIntersect.t) {
            closestShape = (Shape*)(spheres+i);
            closestIntersect = intersect;
        }
    }

    // Check intersections with AABBs
    for (i = 0; i < aabbNumber; i++) {
        if (aabbs[i].intersect(&intersect, ray, camera.getFar()) && intersect.t < closestIntersect.t) {
            closestShape = (Shape*)(aabbs+i);
            closestIntersect = intersect;
        }
    }

    // Check intersections with triangles
    for (i = 0; i < triangleNumber; i++) {
        if (triangles[i].intersect(&intersect, ray, camera.getFar()) && intersect.t < closestIntersect.t) {
            closestShape = (Shape*)(triangles+i);
            closestIntersect = intersect;
        }
    }

    // Check intersections with bezier surfaces
    for (i = 0; i < bezierSurfaceNumber; i++) {
        if (bezierSurfaces[i].intersect(&intersect, ray, camera.getFar()) && intersect.t < closestIntersect.t) {
            closestShape = (Shape*)(bezierSurfaces+i);
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

            // Check if any sphere prevents light beams from hitting to the hit location
            for (j = 0; j < sphereNumber && shadowingShapeTransparency == 1.0f; j++) {
                if (spheres[j].intersect(NULL, lightRay, lightInfo.distance)) {
                    shadowingShapeTransparency = spheres[j].getTransparency();
                }
            }

            // Check if any AABB prevents light beams from hitting to the hit location
            for (j = 0; j < aabbNumber && shadowingShapeTransparency == 1.0f; j++) {
                if (aabbs[j].intersect(NULL, lightRay, lightInfo.distance)) {
                    shadowingShapeTransparency = aabbs[j].getTransparency();
                }
            }

            // Check if any triangle prevents light beams from hitting to the hit location
            for (j = 0; j < triangleNumber && shadowingShapeTransparency == 1.0f; j++) {
                if (triangles[j].intersect(NULL, lightRay, lightInfo.distance)) {
                    shadowingShapeTransparency = triangles[j].getTransparency();
                }
            }

            // Check if any bezier surface prevents light beams from hitting to the hit location
            for (j = 0; j < bezierSurfaceNumber && shadowingShapeTransparency == 1.0f; j++) {
                if (bezierSurfaces[j].intersect(NULL, lightRay, lightInfo.distance)) {
                    shadowingShapeTransparency = bezierSurfaces[j].getTransparency();
                }
            }

            // Calculate diffuse and specular light intensity
            const float diffuse = DIFFUSE_COEF * greater(lightInfo.directionToLight.dot(closestIntersect.normal), 0.0f);
            const Vector3D bisector = Vector3D::bisector(lightInfo.directionToLight, -ray.dir);
            const float specular = SPECULAR_COEF * powf(greater(bisector.dot(closestIntersect.normal), 0.0f), SPECULAR_POW);
            
            // Calculate the color that will be added
            const Color addedColor = closestShape->getColor() * lights[i]->getColor()
                * ((diffuse + specular) 
                * lightInfo.intensity                           // Intensity of the light at the point
                / (depthCount * depthCount)                     // As the recursion depth increases, the effect of reflections get smaller
                * shadowingShapeTransparency                    // If an object casts shadow onto the point, use its transparency
                * (1.0f - closestShape->getTransparency()));    // Opacity of the object contributes to the color at the point
            color += addedColor;
        }

        bool totalReflection = false;
        if (closestShape->getTransparency() > 0.0f) {
            const float normalDotComingRayDir = closestIntersect.normal.dot(ray.dir);
            const float sinComingAngle = sqrtf(1.0f - normalDotComingRayDir*normalDotComingRayDir);
            const float outgoingRefractiveIndex = 
                (incomingRefractiveIndex == WORLD_REFRACTIVE_INDEX) ? closestShape->getRefractiveIndex() : WORLD_REFRACTIVE_INDEX;
            const float outgoingToIncomingRefractiveIndexRatio = outgoingRefractiveIndex / incomingRefractiveIndex;

            // Check whether there is a total reflection or not
            totalReflection = sinComingAngle >= outgoingToIncomingRefractiveIndexRatio;

            // If there is no total reflection, then calculate the ray and call the function recursively
            if (!totalReflection) {
                const Vector3D dirPerpendicularComponentToNormal = ray.dir - closestIntersect.normal * normalDotComingRayDir;
                const Vector3D refractiveRayDir = 
                    (-closestIntersect.normal + dirPerpendicularComponentToNormal / outgoingToIncomingRefractiveIndexRatio).normalize();

                Ray refractiveRay = {
                    .origin = closestIntersect.hitLocation + refractiveRayDir * EPSILON3,
                    .dir = refractiveRayDir,
                };

                // Recursion depth does not increase since we want objects behind a transparent object to contribute more
                traceRay(refractiveRay, color, outgoingRefractiveIndex, depthCount);
            }
        }

        // If a total reflection occurs, or the surface is reflective
        if (totalReflection || closestShape->isReflective()) {
            // Calculate the reflection ray and call the function recursively
            Ray reflectionRay = {
                .origin = closestIntersect.hitLocation,
                .dir = Vector3D::reflection(-ray.dir, closestIntersect.normal)
            };
            traceRay(reflectionRay, color, incomingRefractiveIndex, depthCount+1);
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
            Ray ray = camera.generateRay(x, y);
            
            // Calculate the color of the pixel
            Color& color = image[(IMAGE_HEIGHT-j-1)*IMAGE_WIDTH + i];
            traceRay(ray, color, WORLD_REFRACTIVE_INDEX, 1);
        }
    }
}

int main(int argc, char **argv) {
    std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::vector<Vector3D> bezierVertices = readCubicBezierData("data/bezier.txt");
    for (uint32_t i = 0; i < bezierVertices.size(); i++) {
        bezierVertices[i] *= bezierScalar;
        bezierVertices[i].rotate(bezierRadianX, bezierRadianY, bezierRadianZ);
        bezierVertices[i] += bezierPosition;
    }
    bezierSurfaceNumber = bezierVertices.size() >> 4; // divide by 16

    for (uint32_t i = 0; i < bezierSurfaceNumber; i++) {
        bezierVector.push_back(
            BezierSurface(
                bezierVertices.data() + (i << 4), 
                bezierSubdivision, 
                Color::Cyan, 
                false, 
                bezierTransparency, 
                bezierRefractiveIndex
            )
        );
    }
    bezierSurfaces = bezierVector.data();
    bezierVertices.clear();

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

    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    float durationInSeconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1E6f;
    std::cout << "Finished in " << durationInSeconds << " seconds..." << std::endl;

    return 0;
}
