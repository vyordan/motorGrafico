#pragma once

class Vector3 {
public:
    float x, y, z;
    
    // Constructor con valores por defecto
    Vector3(float x = 0.0f, float y = 0.0f, float z = 0.0f);
    
    // Operaciones básicas que necesitaremos
    Vector3 operator+(const Vector3& otro) const;
    Vector3 operator-(const Vector3& otro) const;
    Vector3 operator*(float escalar) const;
};