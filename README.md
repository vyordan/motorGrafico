# 🧊 Motor Gráfico 3D - Visualizador de Puntos 
### [TAREA DE CALCULO II - Ing. Ricardo  Figueroa]

Un motor gráfico 3D desarrollado desde cero en C++ con OpenGL para visualización interactiva de puntos y funciones matemáticas.


## Comandos de uso
```bash
# Agregar punto 3D
a x y z

# Conectar puntos
c P1 P2

# Funciones 3D
graf paraboloide
graf seno
graf plano
graf montana  
graf onda

# Controles
grid on/off
l
r
h
s
```
 - Click izquierdo + arrastrar: Rotar cámara

 - Rueda mouse: Zoom


**VIDEO DEMOSTRATIVO de uso**

[![VIDEO DEMOSTRATIVO](https://img.youtube.com/vi/QP6T9h0j4mI/0.jpg)](https://www.youtube.com/watch?v=QP6T9h0j4mI)


## Ejecución 
```bash
# Navegar al directorio build
cd build

# Compilar el proyecto
make

# Ejecutar la aplicación
./Visualizador3D
```
## Instalar dependencias

base-devel / build-essential - Compilador C++

cmake - Sistema de compilación

glfw - Ventanas y input

glm - Matemáticas 3D

glew - OpenGL moderno

freeglut / mesa - Renderizado 3D

**Para Arch Linux:**
```bash
sudo pacman -S base-devel cmake glfw-x11 glm glew freeglut mesa
```
**Para Ubuntu | Debian Linux:**
```bash
sudo apt install build-essential cmake libglfw3-dev libglm-dev libglew-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev
```
**Para Fedora Linux:**
```bash
sudo dnf install gcc-c++ cmake glfw-devel glm-devel glew-devel freeglut-devel mesa-libGLU
```

## 

![cap](https://github.com/vyordan/motorGrafico/blob/master/capturas/2025-10-22_00-59-19.png)

