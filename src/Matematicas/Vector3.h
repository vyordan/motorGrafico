#pragma once

class Vector3 {
public:
    float x, y, z;

    Vector3(float x = 0, float y = 0, float z = 0);
    
    Vector3 operator+(const Vector3& otro) const;
    Vector3 operator-(const Vector3& otro) const;
    Vector3 operator*(float escalar) const;
    
    float dot(const Vector3& otro) const;
    Vector3 cross(const Vector3& otro) const;
    Vector3 normalizado() const;
    float longitud() const;
};