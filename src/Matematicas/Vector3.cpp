#include "Vector3.h"

// Constructor - inicializa los valores
Vector3::Vector3(float x, float y, float z) 
    : x(x), y(y), z(z) {
}

// Suma de vectores: (x1+x2, y1+y2, z1+z2)
Vector3 Vector3::operator+(const Vector3& otro) const {
    return Vector3(x + otro.x, y + otro.y, z + otro.z);
}

// Resta de vectores: (x1-x2, y1-y2, z1-z2)  
Vector3 Vector3::operator-(const Vector3& otro) const {
    return Vector3(x - otro.x, y - otro.y, z - otro.z);
}

// Multiplicación por escalar: (x*s, y*s, z*s)
Vector3 Vector3::operator*(float escalar) const {
    return Vector3(x * escalar, y * escalar, z * escalar);
}