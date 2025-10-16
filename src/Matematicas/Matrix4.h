#pragma once
#include "Vector3.h"

class Matrix4 {
public:
    float m[16];
    
    Matrix4();
    
    static Matrix4 lookAt(const Vector3& eye, const Vector3& center, const Vector3& up);
    static Matrix4 perspective(float fov, float aspect, float near, float far);
};