#include "GUIBox.h"



#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

GUIBox::GUIBox(float x, float y, float width, float height)
    : GUINode(x, y, width, height)
{
    nodeJustification = LEFT;
}

void GUIBox::Display(float mouseX, float mouseY, glm::vec2 offset)
{
    glColor3f(backgroundColor.r, backgroundColor.g, backgroundColor.b);
    glTranslatef(x, y, 0);
    glCallList(list);

    glColor3f(fontColor.r, fontColor.g, fontColor.b);
    DoRasterString(0., 0., 0., text);
}


void GUIBox::Update(float mouseX, float mouseY, bool click, glm::vec2 offset)
{
    glPushMatrix();

    Display(mouseX, mouseY, offset);

    for (auto element : nodes)
    {
        glPushMatrix();
        float centerOffset = 0.;
        if (nodeJustification == CENTER)
        {
            centerOffset = (width - element->width) / 2.;
            glTranslatef(centerOffset, 0., 0.);
        }
        glTranslatef(0, 0., 0.1);
        element->Update(mouseX, mouseY, click, { offset.x + x + centerOffset, offset.y + y });
        glPopMatrix();
    }

    glPopMatrix();
}


void GUIBox::AddChild(GUINode* node)
{
    this->nodes.push_back(node);
}


void GUIBox::CenterChildren()
{
    nodeJustification = CENTER;
}

