#include "GUINode.h"


GUINode::GUINode(float x, float y, float width, float height)
{
    this->height = height;
    this->width = width;
    this->x = x;
    this->y = y;

    this->backgroundColor = { .1, .1, .1 };
    this->fontColor = { .9, .9, .9 };

    sprintf_s(text, "");
    textJustification = CENTER;
    textAlignment = CENTER;

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
        glBegin(GL_QUADS);

        glVertex2f( 0., 0. );
        glVertex2f( width, 0. );
        glVertex2f( width, height );
        glVertex2f( 0., height );

        glEnd();
    glEndList();
} 


/*
 * Displays the node, and relays the offset and normalized mouse positions
 */
void GUINode::Display(float mouseX, float mouseY, glm::vec2 offset)
{
    glPushMatrix();
        glColor3f(backgroundColor.r, backgroundColor.g, backgroundColor.b);
        glTranslatef(x, y, 0);
        glCallList(list);

        glColor3f(fontColor.r, fontColor.g, fontColor.b);
        DoRasterString(0., 0., 0., text);
    glPopMatrix();
}


void GUINode::Update(float mouseX, float mouseY, bool click, glm::vec2 offset)
{
    Display(mouseX, mouseY, offset);
}


void GUINode::SetColor(const glm::vec3 background, const glm::vec3 font)
{
    backgroundColor = background;
    fontColor = font;
}


void GUINode::SetText(const char* text)
{
    sprintf_s(this->text, text);
    textWidth = 0;

    char c;
    for (; (c = *text) != '\0'; text++)
    {
        textWidth++;
    }
}


void GUINode::DoRasterString(float x, float y, float z, char* s)
{
    switch (textJustification)
    {
    case CENTER:
        x += (width - (float)textWidth) / 2.;
        break;
    case RIGHT:
        x += width - (float)textWidth;
        break;
    }

    switch (textAlignment)
    {
    case TOP:
        y += height - 1.5;
        break;
    case CENTER:
        y += abs(height - 1.5) / 2.;
        break;
    }

    glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

    char c;			// one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
    }
}


void GUINode::DoStrokeString(float x, float y, float z, float ht, char* s)
{
    glPushMatrix();
    glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    float sf = ht / (119.05f + 33.33f);
    glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
    char c;			// one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

