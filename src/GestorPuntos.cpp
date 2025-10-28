#include "GestorPuntos.h"
#include <iostream>

using namespace std;

// Constructor - inicializa el contador
GestorPuntos::GestorPuntos() : contadorPuntos(1) {
    agregarPunto(0.0f, 0.0f, 0.0f);
    cout<<"El punto 'P0' es el origen (puede hacer conexiones con el*)"<<endl;
    if (!puntos.empty()) {
        puntos[0].nombre = "P0";
    }
    contadorPuntos = 1;
}

// Agregar un nuevo punto con coordenadas específicas
bool GestorPuntos::agregarPunto(float x, float y, float z) {
    string nombre;
    
    // Si es el primer punto, llamarlo P0
    if (puntos.empty()) {
        nombre = "P0";
    } else {
        nombre = generarNombrePunto();
    }
    
    Punto3D nuevoPunto(nombre, x, y, z);
    puntos.push_back(nuevoPunto);
    
    cout << "Punto " << nombre << " creado en (" 
         << x << ", " << y << ", " << z << ")" << endl;
    return true;
}

// Eliminar un punto por nombre
bool GestorPuntos::eliminarPunto(const string& nombre) {
    int indice = encontrarIndicePunto(nombre);
    if (indice == -1) {
        cout << "Punto " << nombre << " no encontrado" << endl;
        return false;
    }
    
    puntos[indice].desactivar();
    cout << "Punto " << nombre << " eliminado" << endl;
    return true;
}

// Limpiar todos los puntos
void GestorPuntos::limpiarPuntos() {
    for (auto& punto : puntos) {
        punto.desactivar();
    }
    cout << "todos los puntos eliminados" << endl;
}

// Conectar dos puntos por nombre
bool GestorPuntos::conectarPuntos(const string& nombreA, const string& nombreB) {
    int indiceA = encontrarIndicePunto(nombreA);
    int indiceB = encontrarIndicePunto(nombreB);
    
    if (indiceA == -1 || indiceB == -1) {
        cout << "No se pueden conectar - puntos no encontrados" << endl;
        return false;
    }
    
    if (indiceA == indiceB) {
        cout << "No se puede conectar un punto consigo mismo" << endl;
        return false;
    }
    
    Conexion nuevaConexion(indiceA, indiceB);
    conexiones.push_back(nuevaConexion);
    
    cout << nombreA << " conectado con " << nombreB << endl;
    return true;
}

// Desconectar dos puntos
bool GestorPuntos::desconectarPuntos(const string& nombreA, const string& nombreB) {
    // Por simplicidad, por ahora limpiamos todas las conexiones
    // En una versión más avanzada buscaríamos la conexión específica
    cout << "!!!!!!!!!!!!!Función desconectar no implementada aún" << endl;
    return false;
}

// Limpiar todas las conexiones
void GestorPuntos::limpiarConexiones() {
    for (auto& conexion : conexiones) {
        conexion.desactivar();
    }
    cout << "Todas las conexiones eliminadas" << endl;
}

// Obtener cantidad de puntos activos
int GestorPuntos::obtenerCantidadPuntos() const {
    int count = 0;
    for (const auto& punto : puntos) {
        if (punto.estaActivo()) count++;
    }
    return count;
}

// Obtener cantidad de conexiones activas
int GestorPuntos::obtenerCantidadConexiones() const {
    int count = 0;
    for (const auto& conexion : conexiones) {
        if (conexion.estaActiva()) count++;
    }
    return count;
}

// Obtener solo los puntos activos
vector<Punto3D> GestorPuntos::obtenerPuntosActivos() const {
    vector<Punto3D> activos;
    for (const auto& punto : puntos) {
        if (punto.estaActivo()) {
            activos.push_back(punto);
        }
    }
    return activos;
}

// Obtener solo las conexiones activas
vector<Conexion> GestorPuntos::obtenerConexionesActivas() const {
    vector<Conexion> activas;
    for (const auto& conexion : conexiones) {
        if (conexion.estaActiva()) {
            activas.push_back(conexion);
        }
    }
    return activas;
}

// Encontrar el índice de un punto por nombre
int GestorPuntos::encontrarIndicePunto(const string& nombre) const {
    for (size_t i = 0; i < puntos.size(); i++) {
        if (puntos[i].nombre == nombre && puntos[i].estaActivo()) {
            return i;
        }
    }
    return -1; // No encontrado
}

// Generar nombre automático: P1, P2, P3...
string GestorPuntos::generarNombrePunto() {
    string nombre = "P" + to_string(contadorPuntos);
    contadorPuntos++;
    return nombre;
}

// Listar todos los puntos activos
void GestorPuntos::listarPuntos() const {
    cout << "\n=== PUNTOS ACTIVOS ===" << endl;
    if (puntos.empty()) {
        cout << "No hay puntos en la escena" << endl;
        return;
    }
    
    int activos = 0;
    for (size_t i = 0; i < puntos.size(); ++i) {
        if (puntos[i].estaActivo()) {
            cout << puntos[i].nombre << ": (" 
                 << puntos[i].posicion.x << ", "
                 << puntos[i].posicion.y << ", "
                 << puntos[i].posicion.z << ")";
            
            // Mostrar conexiones de este punto
            bool tieneConexiones = false;
            for (const auto& conexion : conexiones) {
                if (conexion.estaActiva() && 
                    (conexion.idPuntoA == i || conexion.idPuntoB == i)) {
                    if (!tieneConexiones) {
                        cout << " - Conectado con: ";
                        tieneConexiones = true;
                    }
                    int otroPunto = (conexion.idPuntoA == i) ? conexion.idPuntoB : conexion.idPuntoA;
                    if (otroPunto < puntos.size() && puntos[otroPunto].estaActivo()) {
                        cout << puntos[otroPunto].nombre << " ";
                    }
                }
            }
            cout << endl;
            activos++;
        }
    }
    cout << "Total: " << activos << " puntos activos, " 
         << obtenerCantidadConexiones() << " conexiones" << endl;
}