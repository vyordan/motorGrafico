# Motor Gráfico 3D - Visualizador de Puntos

Motor gráfico 3D desarrollado en C++/OpenGL para visualizar estructuras matemáticas.

**Tecnologías:** OpenGL 4.6 • C++17 • GLFW • CMake

**Características:** Renderizado 3D • Cámara orbital • Consola interactiva • Matemáticas 3D


```bash
# Gestión de Puntos
a x y z              # Agregar punto en coordenadas (x,y,z)
c P1 P2              # Conectar dos puntos
l                    # Listar todos los puntos activos
d P1                 # Eliminar punto específico
r                    # Reiniciar escena completa

# Cámara y Navegación
# - Click izquierdo + arrastrar: Rotar cámara
# - Rueda del mouse: Zoom in/out

# Sistema
h                    # Mostrar ayuda
s                    # Salir del programa

#---------------------------------------------------------------------

### Linux
# Instalar dependencias
# Arch:
sudo pacman -S base-devel cmake glfw-x11 mesa

# Ubuntu/Debian:
sudo apt install build-essential cmake libglfw3-dev libglu1-mesa-dev

# Compilar (la carpeta build ya existe en el repositorio)
cd build
cmake ..
make

# Ejecutar
./Visualizador3D

```

![capturaMotorGrafico](https://github.com/vyordan/motorGrafico/blob/master/capturas/2025-10-22_03-51-01.png)
![capturaMotorGrafico](https://github.com/vyordan/motorGrafico/blob/master/capturas/2025-10-22_00-59-19.png)
