#include "Vector3.h"
#include <cmath>

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

// Producto punto
float Vector3::dot(const Vector3& otro) const {
    return x * otro.x + y * otro.y + z * otro.z;
}

// Producto cruz
Vector3 Vector3::cross(const Vector3& otro) const {
    return Vector3(
        y * otro.z - z * otro.y,
        z * otro.x - x * otro.z,
        x * otro.y - y * otro.x
    );
}

// Normalizar
Vector3 Vector3::normalizado() const {
    float length = std::sqrt(x*x + y*y + z*z);
    if (length > 0) {
        return Vector3(x/length, y/length, z/length);
    }
    return *this;
}

// Longitud
float Vector3::longitud() const {
    return std::sqrt(x*x + y*y + z*z);
}