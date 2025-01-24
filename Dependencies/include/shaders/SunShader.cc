#include "SunShader.h"
#include "glPrerequisites.h"

SunShader::SunShader() {
    const char* VERTEX_FILE = "Dependencies/include/shaders/sun.vert";
    const char* FRAGMENT_FILE = "Dependencies/include/shaders/sun.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

SunShader::~SunShader() {
    // Tutaj mo¿esz dodaæ kod czyszcz¹cy zasoby, jeœli jest to konieczne
}

void SunShader::bindAttributes() {
    bindAttribute(0, "position"); // Powi¹zanie atrybutu pozycji
    bindAttribute(1, "texCoords"); // Powi¹zanie atrybutu wspó³rzêdnych tekstury
}

void SunShader::getAllUniformLocations() {
    location_sunColor = getUniformLocation("sunColor");
    location_sunIntensity = getUniformLocation("sunIntensity");
}

void SunShader::loadSunColor(const glm::vec3& color) {
    loadVector3f(location_sunColor, color);
}

void SunShader::loadSunIntensity(float intensity) {
    loadFloat(location_sunIntensity, intensity);
}