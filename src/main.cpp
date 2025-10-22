#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <thread> 
#include <atomic>
#include <mutex>
#include <queue>
#include <cmath> 
#include "GestorPuntos.h" 
#include "Punto3D.h"
#include "Conexion.h"
#include "Matematicas/Vector3.h"
#include "Matematicas/Matrix4.h"
using namespace std;

atomic<bool> consolaActiva{true};
mutex mutexComandos;
queue<string> colaComandos;

//codigo de los haders como strings
const char* codigoVertexLinea = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        gl_Position = projection * view * vec4(aPos, 1.0);
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

//Shaders para PUNTOS 
const char* codigoVertexPunto = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 colorPunto;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        colorPunto = aColor;
        gl_Position = projection * view * vec4(aPos, 1.0);
        gl_PointSize = 25.0;
    }
)glsl";

const char* codigoFragmentPunto = R"glsl(
    #version 330 core
    out vec4 FragColor;
    in vec3 colorPunto;

    void main()
    {
        vec2 coord = gl_PointCoord * 2.0 - 1.0;
        float distancia = length(coord);
        
        if (distancia > 1.0) {
            discard;
        }
        
        float suavizado = smoothstep(0.8, 1.0, distancia);
        float alpha = 1.0 - suavizado * suavizado;
        
        FragColor = vec4(colorPunto, alpha);
        
        float brillo = 1.0 - (distancia * 0.5);
        FragColor.rgb += brillo * 0.1;
    }
)glsl";

const char* codigoVertexGrid = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 view;
    uniform mat4 projection;
    void main()
    {
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
)glsl";

const char* codigoFragmentGrid = R"glsl(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 colorGrid;
    void main()
    {
        FragColor = vec4(colorGrid, 0.15);  // Muy transparente
    }
)glsl";

struct Camera {
    Vector3 posicion;
    Vector3 objetivo;
    Vector3 arriba;
    float distancia;
    float anguloX;
    float anguloY;
    
    Camera() : posicion(0, 0, 3), objetivo(0, 0, 0), arriba(0, 1, 0), 
               distancia(3.0f), anguloX(0), anguloY(0) {}
    
    void actualizar() {
        posicion.x = objetivo.x + distancia * std::cos(anguloY) * std::sin(anguloX);
        posicion.y = objetivo.y + distancia * std::sin(anguloY);
        posicion.z = objetivo.z + distancia * std::cos(anguloY) * std::cos(anguloX);
    }
    
    Matrix4 obtenerVista() {
        return Matrix4::lookAt(posicion, objetivo, arriba);
    }
    
    Matrix4 obtenerProyeccion() {
        return Matrix4::perspective(45.0f, 800.0f/600.0f, 0.1f, 100.0f);
    }
};

Camera camara;
bool mouseBotonIzqPresionado = false;
bool mouseBotonDerPresionado = false;
double mouseXAnterior = 0, mouseYAnterior = 0;

bool mostrarGrid = true;
unsigned int shaderGrid;
unsigned int VBOGrid, VAOGrid;


void callbackClickMouse(GLFWwindow* window, int button, int action, int mods) ;
void callbackMovimientoMouse(GLFWwindow* window, double xpos, double ypos);
void callbackRuedaMouse(GLFWwindow* window, double xoffset, double yoffset);
void mostrarMenu();
void procesarComando(const string& comando, GestorPuntos& gestor);
void procesarComandosPendientes(GestorPuntos& gestor);
void hiloConsola();
void manejarInput(GLFWwindow *ventana); //leer el teclado
void ajustarVentana(GLFWwindow *ventana, int ancho, int alto); //redimencionar con el mouse
unsigned int compilarShader(const char* codigoVertice, const char* codigoFragmento); // Función para compilar shaders desde strings
vector<float> generarVerticesGrid();

const int ANCHO = 800;
const int ALTO = 600;
GLFWwindow *ventana; 

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
    ventana = glfwCreateWindow(ANCHO, ALTO, "Grafica3D - puntos", NULL, NULL);

    if(ventana == NULL) {
        cout <<"no se pudeo crear la ventana"<<endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(ventana);
    glfwSetFramebufferSizeCallback(ventana, ajustarVentana);

    glfwSetMouseButtonCallback(ventana, callbackClickMouse);
    glfwSetCursorPosCallback(ventana, callbackMovimientoMouse);
    glfwSetScrollCallback(ventana, callbackRuedaMouse);

    //inicializamos glad y veriricar que opengl funciona
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "no esta funcionando glad" << endl;
        return -1;
    }

    //COMPILAR SHADERS
    unsigned int shaderLineas = compilarShader(codigoVertexLinea, codigoFragmentLinea);
    unsigned int shaderPuntos = compilarShader(codigoVertexPunto, codigoFragmentPunto);
    // COMPILAR SHADER DEL GRID
    unsigned int shaderGrid = compilarShader(codigoVertexGrid, codigoFragmentGrid);
    if (shaderGrid == 0) {
        cout << "Error creando shader del grid" << endl;
        return -1;
    }

    // CREAR Y CONFIGURAR BUFFERS DEL GRID
    vector<float> verticesGrid = generarVerticesGrid();

    glGenVertexArrays(1, &VAOGrid);
    glGenBuffers(1, &VBOGrid);

    glBindVertexArray(VAOGrid);
    glBindBuffer(GL_ARRAY_BUFFER, VBOGrid);
    glBufferData(GL_ARRAY_BUFFER, verticesGrid.size() * sizeof(float), verticesGrid.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (shaderLineas == 0 || shaderPuntos == 0) {
        cout << "error creando shaders" << endl;
        return -1;
    }


    //para mostrar la informacion de los datos
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    cout << "\n=== INFORMACIÓN OPENGL ===" << endl;
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version: " << version << endl;
    cout << "GLSL version: " << glslVersion << endl;
    cout << "=========================" << endl;

    // CREAR GESTOR DE PUNTOS
    mostrarMenu();
    GestorPuntos gestorPuntos;
    
    ///*
  
    //*/                                     


    //  DATOS DE LOS EJES 3D (para referencia visual)
    /*float verticesEjes[] = {
        // Eje X (Rojo) - de -1 a +1 en X
        -1.0f,  0.0f,  0.0f,  // Punto inicio eje X
        1.0f,  0.0f,  0.0f,  // Punto fin eje X
        
        // Eje Y (Verde) - de -1 a +1 en Y
        0.0f, -1.0f,  0.0f,  // Punto inicio eje Y
        0.0f,  1.0f,  0.0f,  // Punto fin eje Y
        
        // Eje Z (Azul) - de -1 a +1 en Z  
        0.0f,  0.0f, -1.0f,  // Punto inicio eje Z
        0.0f,  0.0f,  1.0f   // Punto fin eje Z
    };*/

    // DATOS DE LOS EJES 3D CON FLECHAS
    float verticesEjes[] = {
        // Eje X (Rojo) - línea principal + flecha
        -1.0f,  0.0f,  0.0f,  // Inicio eje X
        1.0f,  0.0f,  0.0f,   // Fin eje X (antes de la flecha)
        
        // Flecha eje X (triángulo)
        1.0f,  0.0f,  0.0f,   // Punta de la flecha
        0.9f,  0.05f, 0.0f,   // Esquina 1 de la flecha
        1.0f,  0.0f,  0.0f,   // Punta de la flecha  
        0.9f, -0.05f, 0.0f,   // Esquina 2 de la flecha
        1.0f,  0.0f,  0.0f,   // Punta de la flecha
        0.9f,  0.0f,  0.05f,  // Esquina 3 de la flecha
        1.0f,  0.0f,  0.0f,   // Punta de la flecha
        0.9f,  0.0f, -0.05f,  // Esquina 4 de la flecha
        
        // Eje Y (Verde) - línea principal + flecha
        0.0f, -1.0f,  0.0f,   // Inicio eje Y
        0.0f,  1.0f,  0.0f,   // Fin eje Y (antes de la flecha)
        
        // Flecha eje Y (triángulo)
        0.0f,  1.0f,  0.0f,   // Punta de la flecha
        0.05f, 0.9f,  0.0f,   // Esquina 1 de la flecha
        0.0f,  1.0f,  0.0f,   // Punta de la flecha
        -0.05f,0.9f,  0.0f,   // Esquina 2 de la flecha
        0.0f,  1.0f,  0.0f,   // Punta de la flecha
        0.0f,  0.9f,  0.05f,  // Esquina 3 de la flecha
        0.0f,  1.0f,  0.0f,   // Punta de la flecha
        0.0f,  0.9f, -0.05f,  // Esquina 4 de la flecha
        
        // Eje Z (Azul) - línea principal + flecha
        0.0f,  0.0f, -1.0f,   // Inicio eje Z
        0.0f,  0.0f,  1.0f,   // Fin eje Z (antes de la flecha)
        
        // Flecha eje Z (triángulo)
        0.0f,  0.0f,  1.0f,   // Punta de la flecha
        0.05f, 0.0f,  0.9f,   // Esquina 1 de la flecha
        0.0f,  0.0f,  1.0f,   // Punta de la flecha
        -0.05f,0.0f,  0.9f,   // Esquina 2 de la flecha
        0.0f,  0.0f,  1.0f,   // Punta de la flecha
        0.0f,  0.05f, 0.9f,   // Esquina 3 de la flecha
        0.0f,  0.0f,  1.0f,   // Punta de la flecha
        0.0f, -0.05f, 0.9f,   // Esquina 4 de la flecha
    };

    // CREAR BUFFERS PARA EJES
    unsigned int VBOEjes, VAOEjes;
    glGenVertexArrays(1, &VAOEjes);
    glGenBuffers(1, &VBOEjes);

    glBindVertexArray(VAOEjes);
    glBindBuffer(GL_ARRAY_BUFFER, VBOEjes);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesEjes), verticesEjes, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //BUFFERS DINÁMICOS PARA PUNTOS Y CONEXIONES
    unsigned int VBOPuntos, VAOPuntos;
    unsigned int VBOConexiones, VAOConexiones;

    glGenVertexArrays(1, &VAOPuntos);
    glGenBuffers(1, &VBOPuntos);
    glGenVertexArrays(1, &VAOConexiones);
    glGenBuffers(1, &VBOConexiones);

    //configurar opengl
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f); //gris, de ese color es la ventana
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);

    
    //cout<<"--MENU--"<<endl;
    //cout<<"ESC para salir"<<endl;

    thread threadConsola(hiloConsola);

    //este bucle nos sireve para mantener la ventana abierta (loop)
    while (!glfwWindowShouldClose(ventana)){ 

        manejarInput(ventana);
        procesarComandosPendientes(gestorPuntos);
        
        glClear(GL_COLOR_BUFFER_BIT);

        // OBTENER MATRICES DE CÁMARA
        Matrix4 vista = camara.obtenerVista();
        Matrix4 proyeccion = camara.obtenerProyeccion();

        // OBTENER DATOS ACTUALIZADOS
        vector<Punto3D> puntosActivos = gestorPuntos.obtenerPuntosActivos();
        vector<Conexion> conexionesActivas = gestorPuntos.obtenerConexionesActivas();

        // DIBUJAR EJES DE REFERENCIA
        /*glUseProgram(shaderLineas);
        glBindVertexArray(VAOEjes);
        
        glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "view"), 1, GL_FALSE, &vista.m[0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "projection"), 1, GL_FALSE, &proyeccion.m[0]);

        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINES, 0, 2);  // Eje X

        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_LINES, 2, 2);  // Eje Y

        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 4, 2);  // Eje Z

        glBindVertexArray(0);
        */
        // DIBUJAR EJES DE REFERENCIA CON FLECHAS
        glUseProgram(shaderLineas);
        glBindVertexArray(VAOEjes);

        glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "view"), 1, GL_FALSE, &vista.m[0]);
        glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "projection"), 1, GL_FALSE, &proyeccion.m[0]);

        // Eje X (Rojo) - Línea principal
        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINES, 0, 2);  // Línea del eje X

        // Flecha del eje X
        glDrawArrays(GL_LINES, 2, 8);  // Las 4 líneas de la flecha

        // Eje Y (Verde) - Línea principal
        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_LINES, 10, 2); // Línea del eje Y

        // Flecha del eje Y  
        glDrawArrays(GL_LINES, 12, 8); // Las 4 líneas de la flecha

        // Eje Z (Azul) - Línea principal
        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 20, 2); // Línea del eje Z

        // Flecha del eje Z
        glDrawArrays(GL_LINES, 22, 8); // Las 4 líneas de la flecha

        glBindVertexArray(0);


        // ==================================================
        // DIBUJAR GRID 3D
        // ==================================================
        if (mostrarGrid) {
            glUseProgram(shaderGrid);
            glBindVertexArray(VAOGrid);
            
            // Pasar matrices al shader del grid
            glUniformMatrix4fv(glGetUniformLocation(shaderGrid, "view"), 1, GL_FALSE, &vista.m[0]);
            glUniformMatrix4fv(glGetUniformLocation(shaderGrid, "projection"), 1, GL_FALSE, &proyeccion.m[0]);
            
            // Dibujar grid con colores tenues por plano
            int totalVertices = verticesGrid.size() / 3;
            int verticesPorPlano = totalVertices / 3;
            
            // Plano XY - Verde tenue
            glUniform3f(glGetUniformLocation(shaderGrid, "colorGrid"), 0.2f, 0.5f, 0.2f);
            glDrawArrays(GL_LINES, 0, verticesPorPlano);
            
            // Plano XZ - Azul tenue  
            glUniform3f(glGetUniformLocation(shaderGrid, "colorGrid"), 0.2f, 0.2f, 0.5f);
            glDrawArrays(GL_LINES, verticesPorPlano, verticesPorPlano);
            
            // Plano YZ - Rojo tenue
            glUniform3f(glGetUniformLocation(shaderGrid, "colorGrid"), 0.5f, 0.2f, 0.2f);
            glDrawArrays(GL_LINES, verticesPorPlano * 2, verticesPorPlano);
            
            glBindVertexArray(0);
        }

        // ==================================================
        // DIBUJAR CONEXIONES ENTRE PUNTOS 
        // ==================================================
        if (!conexionesActivas.empty()) {
            vector<float> verticesConexiones;
            for (const auto& conexion : conexionesActivas) {
                if (conexion.estaActiva() && 
                    conexion.idPuntoA < puntosActivos.size() && 
                    conexion.idPuntoB < puntosActivos.size()) {
                    
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoA].posicion.x);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoA].posicion.y);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoA].posicion.z);
                    
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoB].posicion.x);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoB].posicion.y);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoB].posicion.z);
                }
            }

            if (!verticesConexiones.empty()) {
                glUseProgram(shaderLineas);
                glBindVertexArray(VAOConexiones);
                glBindBuffer(GL_ARRAY_BUFFER, VBOConexiones);
                glBufferData(GL_ARRAY_BUFFER, verticesConexiones.size() * sizeof(float), 
                            verticesConexiones.data(), GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "view"), 1, GL_FALSE, &vista.m[0]);
                glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "projection"), 1, GL_FALSE, &proyeccion.m[0]);
                glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.7f, 0.7f, 0.7f);
                glDrawArrays(GL_LINES, 0, verticesConexiones.size() / 3);
                glBindVertexArray(0);
            }
        }

        // ==================================================
        // DIBUJAR PUNTOS
        // ==================================================
        if (!puntosActivos.empty()) {
            glUseProgram(shaderLineas);
            
            for (const auto& punto : puntosActivos) {
                if (punto.estaActivo()) {
                    float tam = 0.02f;
                    float x = punto.posicion.x;
                    float y = punto.posicion.y;
                    float z = punto.posicion.z;
                    
                    float cruzVertices[] = {
                        x-tam, y, z, x+tam, y, z,
                        x, y-tam, z, x, y+tam, z
                    };
                    
                    glBindVertexArray(VAOConexiones);
                    glBindBuffer(GL_ARRAY_BUFFER, VBOConexiones);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(cruzVertices), cruzVertices, GL_DYNAMIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    
                    glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "view"), 1, GL_FALSE, &vista.m[0]);
                    glUniformMatrix4fv(glGetUniformLocation(shaderLineas, "projection"), 1, GL_FALSE, &proyeccion.m[0]);
                    glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 
                            punto.color.x, punto.color.y, punto.color.z);
                    glDrawArrays(GL_LINES, 0, 4);
                }
            }
            glBindVertexArray(0);
        }

        glfwSwapBuffers(ventana);
        glfwPollEvents();    
    }
  

    //limpiar y salir
    glfwDestroyWindow(ventana);
    glfwTerminate();
    
    cout<<"programa finalizado"<<endl;
    return 0;
}

void manejarInput(GLFWwindow *ventana){
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

void mostrarMenu() {
    cout << "\n         COMANDOS" << endl;
    cout << "a x y z - Agregar punto" << endl;
    cout << "c P1 P2 - Conectar puntos" << endl;
    cout << "l - Listar puntos" << endl;
    cout << "d P1 - Eliminar punto" << endl;
    cout << "r - Reiniciar escena" << endl;
    cout << "grid on/off - Mostrar/ocultar grid 3D" << endl;  // NUEVA LÍNEA
    cout << "s - Salir del programa" << endl;
    cout << "h - Mostrar este menu" << endl << endl;
}

void procesarComando(const string& comando, GestorPuntos& gestor) {
    stringstream ss(comando);
    string accion;
    ss >> accion;

    if (accion == "a") {
        float x, y, z;
        if (ss >> x >> y >> z) {
            gestor.agregarPunto(x, y, z);
            cout << "Punto agregado en (" << x << ", " << y << ", " << z << ")" << endl;
        } else {
            cout << "Error: Formato incorrecto. Use: a x y z" << endl;
        }
    }
    else if (accion == "c") {
        string id1, id2;
        if (ss >> id1 >> id2) {
            if (gestor.conectarPuntos(id1, id2)) {
                cout << "Conexion creada: " << id1 << " <-> " << id2 << endl;
            } else {
                cout << "Error: No se pudo crear la conexion" << endl;
            }
        } else {
            cout << "Error: Formato incorrecto. Use: c id1 id2" << endl;
        }
    }
    else if (accion == "l") {
        gestor.listarPuntos();
    }
    else if (accion == "d") {
        string id;
        if (ss >> id) {
            if (gestor.eliminarPunto(id)) {
                cout << "Punto " << id << " eliminado" << endl;
            } else {
                cout << "Error: Punto no encontrado" << endl;
            }
        }
    }
    else if (accion == "r") {
        // Implementar reinicio en GestorPuntos
        cout << "Reiniciando escena..." << endl;
    }
    else if (accion == "h") {
        mostrarMenu();
    }
    else if (accion == "s") {
        cout << "Saliendo..." << endl;
        exit(0);
    } 
    else if (accion == "grid") {
        string estado;
        if (ss >> estado) {
            if (estado == "on") {
                mostrarGrid = true;
                cout << "Grid activado" << endl;
            } else if (estado == "off") {
                mostrarGrid = false;
                cout << "Grid desactivado" << endl;
            }
        } else {
            // Toggle si no se especifica estado
            mostrarGrid = !mostrarGrid;
            cout << "Grid " << (mostrarGrid ? "activado" : "desactivado") << endl;
        }
    }
    else {
        cout << "Comando desconocido. Use 'h' para ayuda." << endl;
    }
}

void procesarComandosPendientes(GestorPuntos& gestor) {
    lock_guard<mutex> lock(mutexComandos);
    
    while (!colaComandos.empty()) {
        string comando = colaComandos.front();
        colaComandos.pop();
        
        // Procesar el comando en el hilo principal
        stringstream ss(comando);
        string accion;
        ss >> accion;

        if (accion == "a") {
            float x, y, z;
            if (ss >> x >> y >> z) {
                gestor.agregarPunto(x, y, z);
            }
        }
        else if (accion == "c") {
            string id1, id2;
            if (ss >> id1 >> id2) {
                gestor.conectarPuntos(id1, id2);
            }
        }
        else if (accion == "l") {
            gestor.listarPuntos();
        }
        else if (accion == "d") {
            string id;
            if (ss >> id) {
                gestor.eliminarPunto(id);
            }
        }
        else if (accion == "grid") {
            string estado;
            if (ss >> estado) {
                if (estado == "on") {
                    mostrarGrid = true;
                    cout << "Grid activado" << endl;
                } else if (estado == "off") {
                    mostrarGrid = false;
                    cout << "Grid desactivado" << endl;
                }
            } else {
                // Toggle si no se especifica estado
                mostrarGrid = !mostrarGrid;
                cout << "Grid " << (mostrarGrid ? "activado" : "desactivado") << endl;
            }
        }
        else if (accion == "r") {
            gestor.limpiarPuntos();
            gestor.limpiarConexiones();
        }
        else if (accion == "h") {
            mostrarMenu();
        }
        else if (accion == "s") {
            consolaActiva = false;
            // Necesitarás acceso a la ventana aquí - podemos hacerla global
            glfwSetWindowShouldClose(ventana, true);
        }
    }
}

void hiloConsola() {
    string comando;
    
    while (consolaActiva) {
        if (getline(cin, comando)) {
            if (!comando.empty()) {
                lock_guard<mutex> lock(mutexComandos);
                colaComandos.push(comando);
            }
        }
    }
}

void callbackClickMouse(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        mouseBotonIzqPresionado = (action == GLFW_PRESS);
        if (action == GLFW_PRESS) {
            glfwGetCursorPos(window, &mouseXAnterior, &mouseYAnterior);
        }
    }
}

void callbackMovimientoMouse(GLFWwindow* window, double xpos, double ypos) {
    if (mouseBotonIzqPresionado) {
        // ROTACIÓN con sensibilidad
        float sensitivity = 0.01f;
        camara.anguloX += (xpos - mouseXAnterior) * sensitivity;
        camara.anguloY += (ypos - mouseYAnterior) * sensitivity;
        
        // Limitar ángulo vertical para evitar voltear
        if (camara.anguloY > 1.4f) camara.anguloY = 1.4f;
        if (camara.anguloY < -1.4f) camara.anguloY = -1.4f;
        
        camara.actualizar();
    }
    
    mouseXAnterior = xpos;
    mouseYAnterior = ypos;
}

void callbackRuedaMouse(GLFWwindow* window, double xoffset, double yoffset) {
    // ZOOM con rueda del mouse
    camara.distancia -= yoffset * 0.5f;
    
    // Limitar distancia
    if (camara.distancia < 1.0f) camara.distancia = 1.0f;
    if (camara.distancia > 20.0f) camara.distancia = 20.0f;
    
    camara.actualizar();
}

vector<float> generarVerticesGrid() {
    vector<float> vertices;
    float tamaño = 2.0f;  // Grid de -2 a +2 en cada eje
    float paso = 0.5f;    // Líneas cada 0.5 unidades
    
    // PLANO XY (Z = 0) - Color verde tenue
    for(float i = -tamaño; i <= tamaño; i += paso) {
        // Líneas paralelas al eje X
        vertices.insert(vertices.end(), {-tamaño, i, 0.0f});
        vertices.insert(vertices.end(), {tamaño, i, 0.0f});
        // Líneas paralelas al eje Y  
        vertices.insert(vertices.end(), {i, -tamaño, 0.0f});
        vertices.insert(vertices.end(), {i, tamaño, 0.0f});
    }
    
    // PLANO XZ (Y = 0) - Color azul tenue
    for(float i = -tamaño; i <= tamaño; i += paso) {
        // Líneas paralelas al eje X
        vertices.insert(vertices.end(), {-tamaño, 0.0f, i});
        vertices.insert(vertices.end(), {tamaño, 0.0f, i});
        // Líneas paralelas al eje Z
        vertices.insert(vertices.end(), {i, 0.0f, -tamaño});
        vertices.insert(vertices.end(), {i, 0.0f, tamaño});
    }
    
    // PLANO YZ (X = 0) - Color rojo tenue  
    for(float i = -tamaño; i <= tamaño; i += paso) {
        // Líneas paralelas al eje Y
        vertices.insert(vertices.end(), {0.0f, -tamaño, i});
        vertices.insert(vertices.end(), {0.0f, tamaño, i});
        // Líneas paralelas al eje Z
        vertices.insert(vertices.end(), {0.0f, i, -tamaño});
        vertices.insert(vertices.end(), {0.0f, i, tamaño});
    }
    
    return vertices;
}