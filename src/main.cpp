#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <sstream>
#include <thread> 
#include <atomic>
#include <thread>
#include <mutex>
#include <queue>
#include "GestorPuntos.h" 
#include "Punto3D.h"
#include "Conexion.h"
using namespace std;

atomic<bool> consolaActiva{true};
mutex mutexComandos;
queue<string> colaComandos;

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

//Shaders para PUNTOS 
const char* codigoVertexPunto = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aColor;
    out vec3 colorPunto;

    void main()
    {
        colorPunto = aColor;
        gl_Position = vec4(aPos, 1.0);
        gl_PointSize = 35.0;
    }
)glsl";

const char* codigoFragmentPunto = R"glsl(
    #version 330 core
    out vec4 FragColor;
    in vec3 colorPunto;

    void main()
    {
        // Coordenadas normalizadas dentro del punto
        vec2 coord = gl_PointCoord * 2.0 - 1.0;
        float distancia = length(coord);
        
        // Crear un círculo perfecto con bordes suaves
        if (distancia > 1.0) {
            discard;
        }
        
        // Suavizado más pronunciado
        float suavizado = smoothstep(0.8, 1.0, distancia);
        float alpha = 1.0 - suavizado * suavizado;  // Más suavizado en bordes
        
        // Color final con transparencia en bordes
        FragColor = vec4(colorPunto, alpha);
        
        // Opcional: agregar un pequeño brillo interior
        float brillo = 1.0 - (distancia * 0.5);
        FragColor.rgb += brillo * 0.1;
    }
)glsl";

void mostrarMenu();
void procesarComando(const string& comando, GestorPuntos& gestor);
void procesarComandosPendientes(GestorPuntos& gestor);
void hiloConsola();
void manejarInput(GLFWwindow *ventana); //leer el teclado
void ajustarVentana(GLFWwindow *ventana, int ancho, int alto); //redimencionar con el mouse
unsigned int compilarShader(const char* codigoVertice, const char* codigoFragmento); // Función para compilar shaders desde strings

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

    //inicializamos glad y veriricar que opengl funciona
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "no esta funcionando glad" << endl;
        return -1;
    }

    //COMPILAR SHADERS
    unsigned int shaderLineas = compilarShader(codigoVertexLinea, codigoFragmentLinea);
    unsigned int shaderPuntos = compilarShader(codigoVertexPunto, codigoFragmentPunto);

    if (shaderLineas == 0 || shaderPuntos == 0) {
        cout << "error creando shaders" << endl;
        return -1;
    }

    // CREAR GESTOR DE PUNTOS
    mostrarMenu();
    GestorPuntos gestorPuntos;

    //  DATOS DE LOS EJES 3D (para referencia visual)
    float verticesEjes[] = {
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
        
        // Limpiar la pantalla con el color de fondo
        glClear(GL_COLOR_BUFFER_BIT);        

        // ==================================================
        // DIBUJAR ESCENA COMPLETA
        // ==================================================

        // OBTENER DATOS ACTUALIZADOS DEL GESTOR
        vector<Punto3D> puntosActivos = gestorPuntos.obtenerPuntosActivos();
        vector<Conexion> conexionesActivas = gestorPuntos.obtenerConexionesActivas();

        // DIBUJAR EJES DE REFERENCIA
        glUseProgram(shaderLineas);
        glBindVertexArray(VAOEjes);

        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 1.0f, 0.0f, 0.0f);
        glDrawArrays(GL_LINES, 0, 2);  // Eje X

        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 1.0f, 0.0f);
        glDrawArrays(GL_LINES, 2, 2);  // Eje Y

        glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_LINES, 4, 2);  // Eje Z

        glBindVertexArray(0);

        // DIBUJAR CONEXIONES ENTRE PUNTOS
        if (!conexionesActivas.empty()) {
            // Preparar datos de vértices para conexiones
            vector<float> verticesConexiones;
            for (const auto& conexion : conexionesActivas) {
                if (conexion.estaActiva() && 
                    conexion.idPuntoA < puntosActivos.size() && 
                    conexion.idPuntoB < puntosActivos.size()) {
                    
                    // Punto A
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoA].posicion.x);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoA].posicion.y);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoA].posicion.z);
                    
                    // Punto B  
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoB].posicion.x);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoB].posicion.y);
                    verticesConexiones.push_back(puntosActivos[conexion.idPuntoB].posicion.z);
                }
            }

            // Dibujar conexiones si hay datos
            if (!verticesConexiones.empty()) {
                glUseProgram(shaderLineas);
                glBindVertexArray(VAOConexiones);
                glBindBuffer(GL_ARRAY_BUFFER, VBOConexiones);
                glBufferData(GL_ARRAY_BUFFER, verticesConexiones.size() * sizeof(float), 
                            verticesConexiones.data(), GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                glEnableVertexAttribArray(0);

                glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 0.7f, 0.7f, 0.7f);
                glDrawArrays(GL_LINES, 0, verticesConexiones.size() / 3);

                glBindVertexArray(0);
            }
        }

        // DIBUJAR PUNTOS COMO PEQUEÑAS CRUCES
        if (!puntosActivos.empty()) {
            glUseProgram(shaderLineas);
            
            for (const auto& punto : puntosActivos) {
                if (punto.estaActivo()) {
                    // Dibujar una pequeña cruz en cada posición de punto
                    float tam = 0.02f; // Tamaño pequeño
                    float x = punto.posicion.x;
                    float y = punto.posicion.y;
                    float z = punto.posicion.z;
                    
                    float cruzVertices[] = {
                        x-tam, y, z,    x+tam, y, z,   // Línea horizontal
                        x, y-tam, z,    x, y+tam, z     // Línea vertical
                    };
                    
                    glBindVertexArray(VAOConexiones);
                    glBindBuffer(GL_ARRAY_BUFFER, VBOConexiones);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(cruzVertices), cruzVertices, GL_DYNAMIC_DRAW);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
                    glEnableVertexAttribArray(0);
                    
                    // Usar el color del punto
                    glUniform3f(glGetUniformLocation(shaderLineas, "colorLinea"), 
                            punto.color.x, punto.color.y, punto.color.z);
                    glDrawArrays(GL_LINES, 0, 4);
                }
            }
            glBindVertexArray(0);
        }

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
    cout << "c - nombrePunto1 nombrePUnto2 - Conectar puntos" << endl;
    cout << "l - Listar puntos" << endl;
    cout << "d nombrePunto - Eliminar punto" << endl;
    cout << "r - Reiniciar escena" << endl;
    cout << "s - Salir del programa" << endl;
    cout << "h - Mostrar este menu" << endl<<endl;
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
    
    cout << "\nconsoloa activa" << endl;
    
    while (consolaActiva) {
        if (getline(cin, comando)) {
            if (!comando.empty()) {
                lock_guard<mutex> lock(mutexComandos);
                colaComandos.push(comando);
            }
        }
    }
}