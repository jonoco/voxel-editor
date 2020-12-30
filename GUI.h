#pragma once

#include <vector>

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "GUINode.h"


class GUI
{
public:
    int height;
    int width;
    float mouseX;
    float mouseY;
    GLdouble orthoWidth = 100;
    GLdouble orthoHeight = 100;
    bool leftClick = false;

    std::vector<GUINode*> nodes;

    GUI( int windowWidth, int windowHeight );

    void AddChild(GUINode* node);
    void Display( );
    void OnKeyboard (unsigned char c, int x, int y);
    void OnMouseButton (int button);
    void OnMouseMove (int x, int y);
};

