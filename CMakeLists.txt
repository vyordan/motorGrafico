cmake_minimum_required(VERSION 3.10)
project(Visualizador3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(ARCHIVOS_FUENTE
    src/main.cpp
    src/glad.c
    src/Matematicas/Vector3.cpp
    src/Matematicas/Matrix4.cpp
    src/Punto3D.cpp
    src/Conexion.cpp
    src/GestorPuntos.cpp
)

add_executable(Visualizador3D ${ARCHIVOS_FUENTE})    
target_link_libraries(Visualizador3D OpenGL::GL glfw) 

message("Cconfiguracion terminada")