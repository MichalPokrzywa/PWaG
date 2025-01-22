#ifndef WRITER_H
#define WRITER_H

#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <string>

class Writer {
public:
    // Rysowanie prostok¹ta na ekranie
    static void drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a);

    // Rysowanie tekstu w ustalonej pozycji
    static void drawText(float x, float y, const std::string& text);

    // Wyœwietlenie wiadomoœci z ciemnym t³em
    static void showMessage(const std::string& message);
};

#endif // WRITER_H
