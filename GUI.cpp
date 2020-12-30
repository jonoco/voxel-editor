#include "GUI.h"


GUI::GUI(int windowWidth, int windowHeight)
{
    height = windowHeight;
    width = windowWidth;
    mouseX = 0.;
    mouseY = 0.;
}


void GUI::AddChild(GUINode* node)
{
    node->windowHeight = height;
    node->windowWidth = width;
    this->nodes.push_back(node);
}


void GUI::Display()
{
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glm::mat4 projection = glm::ortho(0., orthoWidth, 0., orthoHeight);
    glMultMatrixf(glm::value_ptr(projection));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float windowWidth = width / orthoWidth;
    float windowHeight = height / orthoHeight;
    float mouseXPos = mouseX / windowWidth;
    float mouseYPos = 100. - (mouseY / windowHeight);
   
    for (auto element : nodes)
    {
        element->Update(mouseXPos, mouseYPos, leftClick);
    }

    // clear button event
    leftClick = false;
}


void GUI::OnKeyboard(unsigned char c, int x, int y)
{
    //
}


void GUI::OnMouseButton(int button)
{
    leftClick = ((button & 4) != 0);
}


void GUI::OnMouseMove(int x, int y)
{
    mouseX = x;
    mouseY = y;
}

