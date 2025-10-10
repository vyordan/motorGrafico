#pragma once
#include <vector>
#include <string>
#include "Punto3D.h"
#include "Conexion.h"

class GestorPuntos {
private:
    std::vector<Punto3D> puntos;           // Todos los puntos creados
    std::vector<Conexion> conexiones;      // Todas las conexiones
    int contadorPuntos;                    // Para nombres automáticos P1, P2, P3...

public:
    GestorPuntos();
    
    // Gestión de puntos
    bool agregarPunto(float x, float y, float z);
    bool eliminarPunto(const std::string& nombre);
    void limpiarPuntos();
    
    // Gestión de conexiones
    bool conectarPuntos(const std::string& nombreA, const std::string& nombreB);
    bool desconectarPuntos(const std::string& nombreA, const std::string& nombreB);
    void limpiarConexiones();
    
    // Consultas
    int obtenerCantidadPuntos() const;
    int obtenerCantidadConexiones() const;
    std::vector<Punto3D> obtenerPuntosActivos() const;
    std::vector<Conexion> obtenerConexionesActivas() const;
    
    // Utilidades
    int encontrarIndicePunto(const std::string& nombre) const;
    std::string generarNombrePunto();
};