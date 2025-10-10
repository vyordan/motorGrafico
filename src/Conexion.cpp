#include "Conexion.h"

// Constructor - crea una conexión entre dos puntos
Conexion::Conexion(int idA, int idB)
    : idPuntoA(idA), idPuntoB(idB), activa(true) {
    
    // Color por defecto para conexiones (gris claro)
    color = Vector3(0.7f, 0.7f, 0.7f);
}

// Verificar si la conexión está activa
bool Conexion::estaActiva() const {
    return activa;
}

// Desactivar la conexión
void Conexion::desactivar() {
    activa = false;
}