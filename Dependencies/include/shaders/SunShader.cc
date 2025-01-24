#include "SunShader.h"
#include "glPrerequisites.h"

SunShader::SunShader() {
    const char* VERTEX_FILE = "Dependencies/include/shaders/sun.vert";
    const char* FRAGMENT_FILE = "Dependencies/include/shaders/sun.frag";
    ShaderProgram::init(VERTEX_FILE, FRAGMENT_FILE);
}

SunShader::~SunShader() {
    // Tutaj mo�esz doda� kod czyszcz�cy zasoby, je�li jest to konieczne
}

void SunShader::bindAttributes() {
    bindAttribute(0, "position"); // Powi�zanie atrybutu pozycji
    bindAttribute(1, "texCoords"); // Powi�zanie atrybutu wsp�rz�dnych tekstury
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