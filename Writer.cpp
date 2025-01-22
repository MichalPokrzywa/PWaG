#include "Writer.h"
#include <windows.h> // Potrzebne do wglUseFontBitmaps
#include <cstring>   // Do strlen

namespace {
    GLuint base = 0; // Baza list dla czcionek

    void initFont(HDC hdc) {
        if (base == 0) {
            base = glGenLists(256); // Rezerwowanie 256 list dla czcionek
            HFONT font = CreateFont(
                24,                         // Rozmiar czcionki
                0,                          // Szeroko�� (0 = automatycznie)
                0,                          // K�t pochylania (orientacja)
                0,                          // K�t rotacji
                FW_BOLD,                    // Grubo�� czcionki
                FALSE,                      // Kursywa
                FALSE,                      // Podkre�lenie
                FALSE,                      // Przekre�lenie
                ANSI_CHARSET,               // Kodowanie znak�w
                OUT_TT_PRECIS,              // Dok�adno�� kontur�w czcionki
                CLIP_DEFAULT_PRECIS,        // Dok�adno�� przycinania
                ANTIALIASED_QUALITY,        // Jako��
                FF_DONTCARE | DEFAULT_PITCH,// Rodzaj czcionki
                L"Arial"                     // Nazwa czcionki
            );

            SelectObject(hdc, font); // Ustawienie czcionki
            wglUseFontBitmaps(hdc, 0, 256, base); // Tworzenie bitmap dla czcionek
        }
    }
}

void Writer::drawRectangle(float x, float y, float width, float height, float r, float g, float b, float a) {
    glColor4f(r, g, b, a); // Ustawienie koloru z przezroczysto�ci�
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

void Writer::drawText(float x, float y, const std::string& text) {
    HDC hdc = wglGetCurrentDC(); // Pobranie bie��cego kontekstu urz�dzenia
    initFont(hdc); // Inicjalizacja czcionki, je�li jeszcze tego nie zrobiono

    glRasterPos2f(x, y); // Ustawienie pozycji tekstu
    glPushAttrib(GL_LIST_BIT);
    glListBase(base); // Ustawienie bazy list
    glCallLists(text.length(), GL_UNSIGNED_BYTE, text.c_str()); // Rysowanie tekstu
    glPopAttrib();
}

void Writer::showMessage(const std::string& message) {
    // Wy��czenie test�w g��boko�ci i w��czenie mieszania
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Rysowanie ciemnego t�a
    drawRectangle(-1.0f, -1.0f, 2.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.5f);

    // Rysowanie tekstu
    glColor3f(1.0f, 1.0f, 1.0f); // Bia�y tekst
    drawText(-0.2f, 0.0f, message);

    // Przywr�cenie ustawie�
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}
