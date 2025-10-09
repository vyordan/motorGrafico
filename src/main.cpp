#include <iostream>
#include <GLFW/glfw3.h>
using namespace std;

void manejarInput(GLFWwindow *ventana); //el teclado
void ajustarVentana(GLFWwindow *ventana, int ancho, int alto); //redimencionar con el mouse

const int ANCHO = 800;
const int ALTO = 600;

int main(){
    cout<<"programa inicializado"<<endl;
    
    if (!glfwInit()){
        cout<<"ERROR al inicializar glfw"<<endl;
    }

    //configurar la version de oopengl
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //creamos la ventana
    GLFWwindow *ventana = glfwCreateWindow(ANCHO, ALTO, "Grafica3D - puntos", NULL, NULL);

    if(ventana == NULL) {
        cout <<"no se pudeo crear la ventana"<<endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(ventana);
    glfwSetFramebufferSizeCallback(ventana, ajustarVentana); //aqui no le paso argumentos por que no se llama a la funcion inmediatamente

    //configurar opengl
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //gris, de ese color es la ventana
    
    
    cout<<"--MENU--"<<endl;
    cout<<"ESC para salir"<<endl;

    //este bucle nos sireve para mantener la ventana abierta (loop)
    while (!glfwWindowShouldClose(ventana)){ 
        manejarInput(ventana);
        
        // impiar la pantalla con el color de fondo
        glClear(GL_COLOR_BUFFER_BIT);
        
        // aqui vamos a dibujar mas despues
        
        // Intercambiar buffers y procesar eventos
        glfwSwapBuffers(ventana);
        glfwPollEvents();    
    }

    //limpiar y salir
    glfwDestroyWindow(ventana);
    glfwTerminate();

    cout<<"programa finalizado"<<endl;
    return 0;
}

void manejarInput(GLFWwindow *ventana){ //detecta cuando se preciona esc y marca la ventan para cerrarse
    if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(ventana, true);
    }
}

void ajustarVentana(GLFWwindow *ventana, int ancho, int alto){
    glViewport(0, 0, ancho, alto);
}
