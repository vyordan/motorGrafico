#version 330 core
out vec4 FragColor;
uniform vec3 colorLinea; //este color lo cvamos a pasar desde c=+

void main(){
    FragColor = vec4(colorLinea, 1.0);
}