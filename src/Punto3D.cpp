#include "Punto3D.h"

Punto3D::Punto3D(const std::string& nombre, float x, float y, float z)
    : nombre(nombre), posicion(x, y, z), activo(true) {


    if (nombre == "P1") {
        color = Vector3(1.0f, 0.0f, 0.0f);  // Rojo
    } else if (nombre == "P2") {
        color = Vector3(0.0f, 1.0f, 0.0f);  // Verde
    } else if (nombre == "P3") {
        color = Vector3(0.0f, 0.0f, 1.0f);  // Azul
    } else {
        color = Vector3(1.0f, 1.0f, 0.0f);  // Amarillo por defecto todo slos que se ingresen despues del numero 3
    }
}

// vrificar si el punto está activo
bool Punto3D::estaActivo() const {
    return activo;
}

// desactivar el punto (como "eliminarlo" pero manteniendo datos)
void Punto3D::desactivar() {
    activo = false;
}