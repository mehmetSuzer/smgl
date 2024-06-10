
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
#include <mesh.h>

#define MAX_RECURSIVE_RAY_TRACING_DEPTH 6UL
#define MAX_SHAPE_COUNT 100UL
#define MIN_ENERGY_DENSITY (1.0f/255.0f)

#define IMAGE_HEIGHT  840UL
#define IMAGE_WIDTH   840UL

#define THREAD_NUMBER 12UL
#define WIDTH_PER_THREAD (IMAGE_WIDTH/THREAD_NUMBER)

const float GROUND_LEVEL = -50.0f;
const Color& AMBIENT_COLOR = Color::White;
const Color& BACKGROUND_COLOR = Color::Black;

Color image[IMAGE_HEIGHT * IMAGE_WIDTH] = {BACKGROUND_COLOR};

Shape* shapes[MAX_SHAPE_COUNT];
uint32_t shapeNumber = 0;

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

/* ----------------------------------------------------------------------*/

const float pyramidReflectivity = 0.2f;
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
    Sphere(Vector3D(-30.0f, GROUND_LEVEL + 50.0f, 200.0f), 50.0f, Color::Yellow, 0.3f, 0.0f, GLASS_REFRACTIVE_INDEX),
    // Sphere(Vector3D(50.0f, GROUND_LEVEL + 50.0f, 220.0f), 50.0f, Color::Green, 0.4f, 0.0f, GLASS_REFRACTIVE_INDEX),
    // Sphere(Vector3D(0.0f, GROUND_LEVEL + 20.0f, 120.0f), 20.0f, Color::Blue, 0.0f, 0.99f, GLASS_REFRACTIVE_INDEX),
};

/* ----------------------------------------------------------------------*/

const AABB aabbs[] = {
    // AABB(Vector3D(-50.0f, GROUND_LEVEL + EPSILON4, 160.0f), Vector3D(-30.0f, GROUND_LEVEL + 10.0f, 170.0f), Color::Green, 0.0f, 0.0f, GLASS_REFRACTIVE_INDEX),
    // AABB(Vector3D(20.0f, GROUND_LEVEL + EPSILON4, 90.0f), Vector3D(30.0f, GROUND_LEVEL + 90.0f, 100.0f), Color::Blue, 0.0f, 0.99f, GLASS_REFRACTIVE_INDEX),
};

/* ----------------------------------------------------------------------*/

const float teapotScale = 20.0f;
const float teapotRadianX = 0.0f;
const float teapotRadianY = 0.0f;
const float teapotRadianZ = 0.0f;
const uint32_t teapotSubdivision = 4;
const Color& teapotColor = Color::Cyan;
const float teapotReflectivity = 0.0f;
const float teapotTransparency = 0.99f;
const float teapotRefractiveIndex = GLASS_REFRACTIVE_INDEX;
const Vector3D teapotPosition = Vector3D(0.0f, GROUND_LEVEL + 10.0f, 140.0f);

/* ----------------------------------------------------------------------*/

const PointLight pointLight = PointLight(
    Vector3D(0.0f, GROUND_LEVEL + 120.0f, 80.0f), 
    Color::White, 
    2E-5f, 1E-5f
);

const DirectionalLight directionalLight = DirectionalLight(
    Vector3D(0.0f, -sqrtf(0.5f), sqrtf(0.5f)), 
    Color::White, 
    1.0f
);
const SpotLight spotLight = SpotLight(
    Vector3D(0.0f, GROUND_LEVEL + 220.0f, 80.0f), 
    Color::White, 
    2E-5f, 1E-5f, 
    Vector3D(0.0f, -1.0f, 0.0f), 
    M_PIf/2.0f
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

void traceRay(const Ray& ray, Color& color, float incomingRefractiveIndex, float energyDensity, uint32_t depthCount) {
    // If the max recursive depth is exceeded or the energy density is less than a threshold, stop tracing
    if (depthCount > MAX_RECURSIVE_RAY_TRACING_DEPTH || energyDensity < MIN_ENERGY_DENSITY) {
        return;
    }

    Intersect closestIntersect = {.t = INFINITY};
    Intersect currentIntersect;
    Shape* closestShape = NULL;
    Shape* currentShape;

    // Check whether the ray intersects with a shape
    for (uint32_t i = 0; i < shapeNumber; i++) {
        if (shapes[i]->intersect(&currentIntersect, &currentShape, ray, camera.getFar()) && currentIntersect.t < closestIntersect.t) {
            closestIntersect = currentIntersect;
            closestShape = currentShape;
        }
    }

    // Check if the ray hits to an object
    if (closestShape != NULL) {
        // Add the ambient lighting once
        if (depthCount == 1) {
            color += AMBIENT_COLOR * AMBIENT_COEF;
        }

        for (uint32_t i = 0; i < lightNumber; i++) {
            const LightInfo lightInfo = lights[i]->shine(closestIntersect.hitLocation);
            if (lightInfo.distance == -INFINITY) { // Light does not hit to the hit location
                continue;
            }
            
            const Ray lightRay = {
                .origin = closestIntersect.hitLocation + closestIntersect.normal * EPSILON3,
                .dir = lightInfo.directionToLight,
            };

            // Check if a shape casts a shadow onto the point
            Shape* shadowingShape = NULL;
            for (uint32_t j = 0; j < shapeNumber && shadowingShape == NULL; j++) {
                shapes[j]->intersect(NULL, &shadowingShape, lightRay, lightInfo.distance);
            }
            const float shadowingShapeTransparency = (shadowingShape != NULL) ? shadowingShape->getTransparency() : WORLD_TRANSPARENCY;

            // Calculate diffuse and specular light intensity
            const float diffuse = DIFFUSE_COEF * greater(lightInfo.directionToLight.dot(closestIntersect.normal), 0.0f);
            const Vector3D bisector = Vector3D::bisector(lightInfo.directionToLight, -ray.dir);
            const float specular = SPECULAR_COEF * powf(greater(bisector.dot(closestIntersect.normal), 0.0f), SPECULAR_POW);
                        
            // Update the color
            color += closestShape->getColor() * lights[i]->getColor()
                * ((diffuse + specular) 
                * lightInfo.intensity                         // As intensity of the light increases, the point looks brighter
                * energyDensity                               // As the reflectivity and the transparency of the previous shape increases, the current object gets more visible
                * shadowingShapeTransparency                  // If an object casts shadow onto the point, the point looks dimmer
                * (1.0f - closestShape->getTransparency()     // As the transparency of the shape increases, its color gets less visible
                        - closestShape->getReflectivity()));  // As the reflectivity of the shape increases, its color gets less visible
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
                    energyDensity * closestShape->getTransparency(), 
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
                energyDensity * reflectivePortion,
                depthCount+1
            );
        }
    }
}

// Reads the cubic beziers and returns them in a vector
std::vector<Vector3D> readCubicBezierVertices(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        file.close();
        throw std::invalid_argument("Error opening file: " + std::string(filename));
    }

    std::vector<Vector3D> data;
    Vector3D vector;
    while ((file >> vector.x) && (file >> vector.y) && (file >> vector.z)) {
        data.push_back(vector);     
    }
    file.close();

    if (data.size() & 0xF != 0) {
        throw std::invalid_argument("Number of vertices is not a multiple of 16!");
    }
    return data;
}

// The function which renders a portion of the image
void threadFunction(uint32_t index) {
    const float dx = 1.0f / WIDTH_PER_THREAD;
    const float dy = 1.0f / IMAGE_HEIGHT;
    float x = (index+0.5f) / IMAGE_WIDTH;

    for (uint32_t i = 0; i < WIDTH_PER_THREAD; i++) { // x axis
        uint32_t colorIndex = index + i*THREAD_NUMBER;
        float y = 1.0f - dy / 2.0f;
        for (uint32_t j = 0; j < IMAGE_HEIGHT; j++) { // y axis     
            const Ray ray = camera.generateRay(x, y);
            Color& color = image[colorIndex];
            traceRay(ray, color, WORLD_REFRACTIVE_INDEX, 1.0f, 1);

            colorIndex += IMAGE_WIDTH;
            y -= dy;
        }
        x += dx;
    }
}

int main(int argc, char **argv) {
    // Start timing
    std::chrono::_V2::system_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Scale, rotate, and translate the cubic bezier vertices that are read from the file
    std::vector<Vector3D> teapotBezierVertices = readCubicBezierVertices("data/utah_teapot_bezier.txt");
    for (uint32_t i = 0; i < teapotBezierVertices.size(); i++) {
        teapotBezierVertices[i] *= teapotScale;
        teapotBezierVertices[i].rotate(teapotRadianX, teapotRadianY, teapotRadianZ);
        teapotBezierVertices[i] += teapotPosition;
    }

    std::vector<BezierSurface> teapotBodyBezierSurfaces; 
    for (uint32_t i = 0; i < 12; i++) { // Body
        teapotBodyBezierSurfaces.push_back( 
            BezierSurface(
                teapotBezierVertices.data() + (i << 4), 
                teapotSubdivision, 
                teapotColor, 
                teapotReflectivity, 
                teapotTransparency, 
                teapotRefractiveIndex
            )
        );
    }
    std::vector<Shape*> teapotBodyShapes;
    for (uint32_t i = 0; i < teapotBodyBezierSurfaces.size(); i++) {
        teapotBodyShapes.push_back((Shape*)&teapotBodyBezierSurfaces[i]);
    }
    const Mesh teapotBody = Mesh(teapotBodyShapes);

    std::vector<BezierSurface> teapotHandleBezierSurfaces; 
    for (uint32_t i = 12; i < 16; i++) {
        teapotHandleBezierSurfaces.push_back( 
            BezierSurface(
                teapotBezierVertices.data() + (i << 4), 
                teapotSubdivision, 
                teapotColor, 
                teapotReflectivity, 
                teapotTransparency, 
                teapotRefractiveIndex
            )
        );
    }
    std::vector<Shape*> teapotHandleShapes;
    for (uint32_t i = 0; i < teapotHandleBezierSurfaces.size(); i++) {
        teapotHandleShapes.push_back((Shape*)&teapotHandleBezierSurfaces[i]);
    }
    const Mesh teapotHandle = Mesh(teapotHandleShapes);

    std::vector<BezierSurface> teapotSpoutBezierSurfaces; 
    for (uint32_t i = 16; i < 20; i++) {
        teapotSpoutBezierSurfaces.push_back( 
            BezierSurface(
                teapotBezierVertices.data() + (i << 4), 
                teapotSubdivision, 
                teapotColor, 
                teapotReflectivity, 
                teapotTransparency, 
                teapotRefractiveIndex
            )
        );
    }
    std::vector<Shape*> teapotSpoutShapes;
    for (uint32_t i = 0; i < teapotSpoutBezierSurfaces.size(); i++) {
        teapotSpoutShapes.push_back((Shape*)&teapotSpoutBezierSurfaces[i]);
    }
    const Mesh teapotSpout = Mesh(teapotSpoutShapes);

    std::vector<BezierSurface> teapotLidBezierSurfaces; 
    for (uint32_t i = 20; i < 28; i++) {
        teapotLidBezierSurfaces.push_back( 
            BezierSurface(
                teapotBezierVertices.data() + (i << 4), 
                teapotSubdivision, 
                teapotColor, 
                teapotReflectivity, 
                teapotTransparency, 
                teapotRefractiveIndex
            )
        );
    }
    std::vector<Shape*> teapotLidShapes;
    for (uint32_t i = 0; i < teapotLidBezierSurfaces.size(); i++) {
        teapotLidShapes.push_back((Shape*)&teapotLidBezierSurfaces[i]);
    }
    const Mesh teapotLid = Mesh(teapotLidShapes);

    const std::vector<Shape*> teapotShapes = {
        (Shape*)&teapotBody, 
        (Shape*)&teapotHandle, 
        (Shape*)&teapotSpout, 
        (Shape*)&teapotLid,
    };
    const Mesh teapot = Mesh(teapotShapes);

    // Move all shapes to the Shapes vector
    for (uint32_t i = 0; i < sizeof(spheres) / sizeof(Sphere); i++) {
        shapes[shapeNumber++] = ((Shape*)(spheres+i));
    }
    for (uint32_t i = 0; i < sizeof(aabbs) / sizeof(AABB); i++) {
        shapes[shapeNumber++] = ((Shape*)(aabbs+i));
    }
    for (uint32_t i = 0; i < sizeof(triangles) / sizeof(Triangle); i++) {
        shapes[shapeNumber++] = ((Shape*)(triangles+i));
    }
    shapes[shapeNumber++] = ((Shape*)&teapot);

    std::cout << "Rendering..." << std::endl;

    // Start threads
    std::vector<std::thread> threads(THREAD_NUMBER);
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        threads[i] = std::thread(threadFunction, i);
    }

    // Wait until all threads join
    for (uint32_t i = 0; i < THREAD_NUMBER; i++) {
        threads[i].join();
    }

    // Write the image
    std::cout << "Writing image.png..." << std::endl;
    stbi_write_png("image.png", IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, sizeof(Color)*IMAGE_WIDTH);

    // Stop timing
    std::chrono::_V2::system_clock::time_point end = std::chrono::high_resolution_clock::now();
    float durationInSeconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1E6f;
    std::cout << "Finished in " << durationInSeconds << " seconds..." << std::endl;

    return 0;
}
