#pragma once
#include <string>
#include "Matematicas/Vector3.h"
using namespace std;

class Punto3D {
public:
    string nombre;    // "P1", "P2", "P3"...
    Vector3 posicion;      // Coordenadas (x, y, z)
    Vector3 color;         // Color (r, g, b)
    bool activo;           // Si el punto está visible
    
    Punto3D(const std::string& nombre, float x, float y, float z);
    
    bool estaActivo() const;
    void desactivar();
};