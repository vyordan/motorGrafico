#version 330 core
layout (location = 0) in vec3 aPos; //posicion del vertice x, y, zz

void main(){
    gl_Position = vec4(aPos, 1.0); //como usamos cordenadas de pantalla 4d aqui se hace la conversion de 3 a 4
}