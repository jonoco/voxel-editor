#pragma once

#include <functional>

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"



class GUINode
{
protected:
    enum Justify
    {
        LEFT,
        CENTER,
        RIGHT,
        BOTTOM,
        TOP
    };

public:
    glm::vec3 backgroundColor;
    glm::vec3 fontColor;
    float height;
    float width;
    float x;
    float y;
    int windowHeight;
    int windowWidth;

    char text[128];
    int textWidth;
    Justify textJustification;
    Justify textAlignment;

    GLuint list;

    std::function<void()> callback;

    GUINode( float x, float y, float width, float height );

    virtual void Display(float mouseX, float mouseY, glm::vec2 offset = { 0., 0. });
    virtual void Update(float mouseX, float mouseY, bool click = false, glm::vec2 offset = {0., 0.});
    virtual void OnClick() {}

    void SetColor(glm::vec3 background, glm::vec3 font);
    void SetText(const char* text);
    void DoRasterString(float x, float y, float z, char* s);
    void DoStrokeString(float x, float y, float z, float ht, char* s);

    void JustifyLeft()   { textJustification = LEFT; }
    void JustifyCenter() { textJustification = CENTER; }
    void JustifyRight()  { textJustification = RIGHT; }
    void AlignBottom()   { textAlignment = BOTTOM; }
    void AlignCenter()   { textAlignment = CENTER; }
    void AlignTop()      { textAlignment = TOP; }

    void SetCallback(std::function<void()> f) { callback = f; }
};

