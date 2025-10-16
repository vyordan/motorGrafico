#include "Matrix4.h"
#include <cmath>

Matrix4::Matrix4() {
    for(int i = 0; i < 16; i++) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

Matrix4 Matrix4::lookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
    Matrix4 result;
    
    Vector3 f = (center - eye).normalizado();
    Vector3 s = f.cross(up).normalizado();
    Vector3 u = s.cross(f);
    
    result.m[0] = s.x;
    result.m[4] = s.y;
    result.m[8] = s.z;
    
    result.m[1] = u.x;
    result.m[5] = u.y;
    result.m[9] = u.z;
    
    result.m[2] = -f.x;
    result.m[6] = -f.y;
    result.m[10] = -f.z;
    
    result.m[12] = -s.dot(eye);
    result.m[13] = -u.dot(eye);
    result.m[14] = f.dot(eye);
    
    return result;
}

Matrix4 Matrix4::perspective(float fov, float aspect, float near, float far) {
    Matrix4 result;
    float tanHalfFov = std::tan(fov * 0.5f * 3.14159f / 180.0f);
    
    result.m[0] = 1.0f / (aspect * tanHalfFov);
    result.m[5] = 1.0f / tanHalfFov;
    result.m[10] = -(far + near) / (far - near);
    result.m[11] = -1.0f;
    result.m[14] = -(2.0f * far * near) / (far - near);
    result.m[15] = 0.0f;
    
    return result;
}