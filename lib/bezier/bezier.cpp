
#include "bezier.h"

BezierSurface::BezierSurface() : Shape(Color("BLACK"), false) {}

BezierSurface::BezierSurface(Vector3D* controls, const Color& color, bool reflect) : Shape(color, reflect) {
    for (uint32_t i = 0; i < subdivision; i++) {
        float u = static_cast<float>(i) / subdivision;
        for (uint32_t j = 0; j < subdivision; j++) {
            float v = static_cast<float>(j) / subdivision;
            vertices.push_back(getPoint(controls, u, v));
        }
    }
}

void BezierSurface::generateControlPointScalars(float* x_vector, float x) const {
    assert(0.0f <= x && x <= 1.0f);
    const float one_minus_x = 1.0f - x;

    x_vector[0] = one_minus_x * one_minus_x * one_minus_x;
    x_vector[1] = 3.0f * one_minus_x * one_minus_x * x;
    x_vector[2] = 3.0f * one_minus_x * x * x;
    x_vector[3] = x * x * x;
}

Vector3D BezierSurface::getPoint(Vector3D* controls, float u, float v) const {
    float u_vector[4];
    float v_vector[4];
    generateControlPointScalars(u_vector, u);
    generateControlPointScalars(v_vector, v);

    Vector3D point = Vector3D();
    for (uint32_t i = 0; i < 4; i++) {
        for (uint32_t j = 0; j < 4; j++) {
            point += controls[4*i+j] * (u_vector[i] * v_vector[j]);
        }
    }

    return point;
}

bool BezierSurface::intersect(Intersect *intersect, const Ray& ray) const {
    for (uint32_t i = 0; i < subdivision-1; i++) {
        for (uint32_t j = 0; j < subdivision-1; j++) {
            Vector3D vertex1 = vertices[i*subdivision+j];
            Vector3D vertex2 = vertices[i*subdivision+j+1];
            Vector3D vertex3 = vertices[(i+1)*subdivision+j];
            Vector3D vertex4 = vertices[(i+1)*subdivision+j+1];

            if (Triangle(vertex1, vertex2, vertex3, color, reflect).intersect(intersect, ray) || 
                Triangle(vertex4, vertex2, vertex3, color, reflect).intersect(intersect, ray)) {
                return true;
            }
        }
    }
    return false;
}

/*

void BezierSurface::generateControlPointScalars(float* x_vector, float* x_drv_vector, float x) const {
    assert(0.0f <= x && x <= 1.0f);
    const float one_minus_x = 1.0f - x;

    if (x_vector != NULL) {
        x_vector[0] = one_minus_x * one_minus_x * one_minus_x;
        x_vector[1] = 3.0f * one_minus_x * one_minus_x * x;
        x_vector[2] = 3.0f * one_minus_x * x * x;
        x_vector[3] = x * x * x;
    }

    if (x_drv_vector != NULL) {
        x_drv_vector[0] = -3.0f * one_minus_x * one_minus_x;
        x_drv_vector[1] = 3.0f - 12.0f * x + 9.0f * x * x;
        x_drv_vector[2] = (6.0f - 9.0f * x) * x;
        x_drv_vector[3] = 3.0f * x * x;
    }
}

*/