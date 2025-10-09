#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
using namespace std;

//codigo de los haders como strings
const char* codigoVertexLinea = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    void main()
    {
        gl_Position = vec4(aPos, 1.0);
    }
)glsl";

const char* codigoFragmentLinea = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 colorLinea;
    void main()
    {
        FragColor = vec4(colorLinea, 1.0);
    }
)glsl";

void manejarInput(GLFWwindow *ventana); //leer el teclado
void ajustarVentana(GLFWwindow *ventana, int ancho, int alto); //redimencionar con el mouse
unsigned int compilarShader(const char* codigoVertice, const char* codigoFragmento); // Función para compilar shaders desde strings

const int ANCHO = 800;
const int ALTO = 600;

int main(){
    cout<<"programa inicializado"<<endl;
    
    if (!glfwInit()){
        cout<<"no se pudo inicializar glfw"<<endl;
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

    //inicializamos glad y veriricar que opengl funciona
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "no esta funcionando glad" << endl;
        return -1;
    }

    //CONFIGURAR EJES 3D
    unsigned int shaderLineas = compilarShader(codigoVertexLinea, codigoFragmentLinea); //comipilando shader para las lineas
    if (shaderLineas == 0) {
        cout << "error creando shader de líneas" << endl;
        return -1;
    }

    float verticesEjes[] = { //datos de los ejers 3d
        // Eje X (Rojo) - de -1 a +1 en X
        -1.0f,  0.0f,  0.0f,  // Punto inicio eje X
         1.0f,  0.0f,  0.0f,  // Punto fin eje X
        
        // Eje Y (Verde) - de -1 a +1 en Y
         0.0f, -1.0f,  0.0f,  // Punto inicio eje Y
         0.0f,  1.0f,  0.0f,  // Punto fin eje Y
        
        // Eje Z (Azul) - de -1 a +1 en Z  
         0.0f,  0.0f, -1.0f,  // Punto inicio eje Z
         0.0f,  0.0f,  1.0f   // Punto fin eje Z
    };

    //creamos los buffers 
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
        // Configurar el VAO (Vertex Array Object)
    glBindVertexArray(VAO);
    
        // Configurar el VBO (Vertex Buffer Object)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEjes), verticesEjes, GL_STATIC_DRAW);
    
        // Configurar cómo OpenGL interpreta los datos de vértices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
        // Desvincular buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //configurar opengl
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //gris, de ese color es la ventana
    
    
    cout<<"--MENU--"<<endl;
    cout<<"ESC para salir"<<endl;

    //este bucle nos sireve para mantener la ventana abierta (loop)
    while (!glfwWindowShouldClose(ventana)){ 
        manejarInput(ventana);
        
        // impiar la pantalla con el color de fondo
        glClear(GL_COLOR_BUFFER_BIT);

            // DIBUJAMOS
            // Usar el shader de líneas
            glUseProgram(shaderLineas);
            
            // Activar el VAO de los ejes
            glBindVertexArray(VAO);
            
            // Dibujar eje X (ROJO)
            glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 1.0f, 0.0f, 0.0f);
            glDrawArrays(GL_LINES, 0, 2);  // Dibujar primera línea (vértices 0-1)
            
            // Dibujar eje Y (VERDE)
            glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 1.0f, 0.0f);
            glDrawArrays(GL_LINES, 2, 2);  // Dibujar segunda línea (vértices 2-3)
            
            // Dibujar eje Z (AZUL)
            glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_LINES, 4, 2);  // Dibujar tercera línea (vértices 4-5)
            
            glBindVertexArray(0);
        
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

unsigned int compilarShader(const char* codigoVertice, const char* codigoFragmento) {
    // Variables para verificación de errores
    int exito;
    char infoLog[512];
    
    // 1. CREAR Y COMPILAR VERTEX SHADER
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &codigoVertice, NULL);
    glCompileShader(vertexShader);
    
    // Verificar errores
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &exito);
    if (!exito) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERROR compilando VERTEX shader:\n" << infoLog << endl;
        return 0;
    }
    
    // 2. CREAR Y COMPILAR FRAGMENT SHADER  
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &codigoFragmento, NULL);
    glCompileShader(fragmentShader);
    
    // Verificar errores
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &exito);
    if (!exito) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "WRROR compilando FRAGMENT shader:\n" << infoLog << endl;
        return 0;
    }
    
    // 3. CREAR PROGRAMA DE SHADERS Y ENLAZAR
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Verificar errores de enlace
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &exito);
    if (!exito) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERROR enlazando shader program:\n" << infoLog << endl;
        return 0;
    }
    
    // 4. LIMPIAR SHADERS INDIVIDUALES
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return shaderProgram;
}

