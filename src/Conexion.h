#pragma once
#include "Matematicas/Vector3.h"

class Conexion {
public:
    int idPuntoA;        // Índice del primer punto en el vector
    int idPuntoB;        // Índice del segundo punto
    Vector3 color;       // Color de la línea
    bool activa;         // Si la conexión está visible
    
    // Constructor
    Conexion(int idA, int idB);
    
    // Métodos útiles
    bool estaActiva() const;
    void desactivar();
};