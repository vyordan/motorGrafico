cmake_minimum_required(VERSION 3.10)
project(Visualizador3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(ARCHIVOS_FUENTE
    src/main.cpp
)

add_executable(Visualizador3D ${ARCHIVOS_FUENTE})    
target_link_libraries(Visualizador3D OpenGL::GL glfw) 

message("Configuración de CMake lista para Visualizador 3D")