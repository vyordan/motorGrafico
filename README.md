# Visualizador 3D - Motor Gráfico 

Por si queres ver como se ve:
https://youtu.be/QP6T9h0j4mI

Demo
Proyecto de visualización 3D con OpenGL que permite crear y manipular puntos en el espacio, con soporte para múltiples funciones matemáticas predefinidas.

## Requisitos del Sistema

### Dependencias necesarias

#### Herramientas de compilación
| Herramienta | Versión mínima | Propósito |
|-------------|---------------|-----------|
| CMake | 3.10 | Generar archivos de compilación |
| GCC/G++ | 9.0 | Compilador C++ |
| Make | 4.0 | Ejecutar la compilación |

#### Librerías
| Librería | Propósito | Instalación |
|----------|-----------|-------------|
| GLFW | Creación de ventanas y gestión de input | glfw-x11 / libglfw3-dev |
| OpenGL | Renderizado 3D | mesa / libgl1-mesa-dev |
| GLAD | Cargador de OpenGL | Incluido en el proyecto |


## Instalación de dependencias por sistema operativo

### Arch Linux / Manjaro
```bash
sudo pacman -S base-devel cmake glfw-x11 mesa
```

### Ubuntu / Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libglfw3-dev libgl1-mesa-dev
```

### Fedora
```bash
sudo dnf install gcc-c++ make cmake glfw-devel mesa-libGL-devel
```

## Para compilar y ejecutar

```bash
cd ~/VISUALIZADOR_3D    #(vas a la carpeta del proyecto)
mkdir -p build          #(aqui creas la carpeta build)
cd build                #(entras a la carpeta build que es la que acabas de crear)
cmake ..                #(preparas la compilacion)
make                    #(compilas)
./Visualizador3D        #(ejecutas el archivo compilado que se acaba de crear)
```
