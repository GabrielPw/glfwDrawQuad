#include <Windows.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void frameBufferSize_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

float triangleVertices[] = {
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left 
};
unsigned int indices[] = {  // note that we start from 0!
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};

void checkIfShaderRunSucessfully(GLuint shader) {

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}

void checkIfProgramWasCreatedSucessfully(GLuint shaderProgram) {

    int success;
    char infoLog[512];

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "\n(Shader PROGRAM Error.) " << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
}


void createVertexShader(GLuint vertexShader) {

    const char* vertexSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
        "}\0";

    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    checkIfShaderRunSucessfully(vertexShader);
}

void createFragmentShader(GLuint fragmentShader) {

    const char* vertexShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\0";

    glShaderSource(fragmentShader, 1, &vertexShaderSource, NULL);
    glCompileShader(fragmentShader);

    checkIfShaderRunSucessfully(fragmentShader);
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int WIDTH = 800, HEIGHT = 600;
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Aprendendo OpenGL/GLFW", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, frameBufferSize_callback);
    glfwMakeContextCurrent(window);

    gladLoadGL(glfwGetProcAddress);

    /*
        Notes about VAO (Vertex Array Objects):
        You can imagine that real graphics programs use many different shaders and vertex layouts to take
        care of a wide variety of needs and special effects.Changing the active shader program is easy enough with a call to glUseProgram,
        but it would be quite inconvenient if you had to set up all of the attributes again every time.
        Luckily, OpenGL solves that problem with Vertex Array Objects (VAO). VAOs store all of the links between the attributes and your VBOs with raw vertex data.
    */

    // Creating our VAO.
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Lets create a VBO where the vertex data about our triangle will be stored.
    unsigned int VBO;
    glGenBuffers(1, &VBO); // generate 1 buffer.
    // we need to activate the buffer in order to use it.
    glBindBuffer(GL_ARRAY_BUFFER, VBO); // here we activate our vbo setted as GL_ARRAY_BUFFER.
    // Now that our VBO are active, we can copy the vertex data of our triangle to it.

    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); // Copying the triangle vertices data to our vbo (that we had defined as GL_ARRAY_BUFFER)


    
    // at this point we already have the necessary to render our triangle, but now we gonna draw a square.
    // we'll do this by drawing two connected triangle. these triangles will have some common vertices, so we can use EBO to avoid create duplicated vertices
    // and save some memory/performance for our GPU.

    // Creating an EBO.
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    

    // At this point, the data of our triangle was passed to the bufffers of our GPU, so now we have to explain to GPU how this data will be used.

    // Creating a Vertex Shader and Fragment Shader.

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    createVertexShader(vertexShader); 
    createFragmentShader(fragmentShader);

    // after the creation of our shaders, lets link them into a program.
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram(); // create a program and return its ID.
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // checking if program was created successfuly.
    checkIfProgramWasCreatedSucessfully(shaderProgram);

    // after create our program object and linked the shaders with it, we no longer will need the shaders, once they are already saved in our program object.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Linking vertex data and attributes.
    GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");

    /*
    The first parameter specifies which vertex attribute we want to configure. 
    Remember that we specified the location of the position vertex attribute in the vertex shader with layout (location = 0).
    This sets the location of the vertex attribute to 0 and since we want to pass data to this vertex attribute, we pass in 0.
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Limpa a tela.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);       

        // Renderizar.
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}