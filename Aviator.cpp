#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Window dimensions
const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
const char* WINDOW_TITLE = "Endless Runner";

// Game state variables
bool gameRunning = true;

// Plane position (placeholder for more complex mechanics)
float planePositionY = 0.0f;

// Input handling
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Simple up and down movement for the plane
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        planePositionY += 0.05f; // Move up
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        planePositionY -= 0.05f; // Move down
    }
}

// Callback for resizing the window
void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// Initialize OpenGL, GLFW, and GLEW
bool initOpenGL(GLFWwindow*& window) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create the window
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Enable modern OpenGL extensions
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return false;
    }

    // Set OpenGL viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);

    return true;
}

// Render the scene
void render() {
    // Clear the screen
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // Dark blue background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the plane (placeholder)
    // In the future, draw your plane here using a shader and VAO/VBO

    // Swap buffers
}

// Main game loop
void gameLoop(GLFWwindow* window) {
    while (!glfwWindowShouldClose(window) && gameRunning) {
        // Input
        processInput(window);

        // Update (placeholder for game logic updates)

        // Render
        render();

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

// Cleanup resources
void cleanup(GLFWwindow* window) {
    glfwDestroyWindow(window);
    glfwTerminate();
}

// Main function
//int main() {
//    GLFWwindow* window;
//
//    if (!initOpenGL(window)) {
//        return -1;
//    }
//
//    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
//
//    // Start the game loop
//    gameLoop(window);
//
//    // Cleanup resources
//    cleanup(window);
//
//    return 0;
//}
