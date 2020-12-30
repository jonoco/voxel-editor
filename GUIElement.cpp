#include "GUIElement.h"


GUIElement::GUIElement(float x, float y, float width, float height)
    : GUINode(x, y, width, height)
{
    defaultBackgroundColor = { .1, .1, .1 };
    hoverBackgroundColor = { .2, .2, .2 };
    hovering = false;
    animationLength = 100;
}


void GUIElement::Animate()
{
    if (!anim) return;

    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    const float deltaTime = currentTime - anim->startTime;
    float t = deltaTime > animationLength ? 1. : deltaTime / animationLength;
    if (t >= 1.)
    {
        backgroundColor = anim->finalBackgroundColor;
        delete anim;
        anim = nullptr;
    }
    else
    {
        const float easing = ease(t, anim->easing);
        backgroundColor = glm::lerp(anim->startBackgroundColor, anim->finalBackgroundColor, easing);
    }
}


void GUIElement::Display(float mouseX, float mouseY, glm::vec2 offset)
{
    glPushMatrix();
        glColor3f(backgroundColor.r, backgroundColor.g, backgroundColor.b);

        glTranslatef(x, y, 0);
        glCallList(list);

        glColor3f(fontColor.r, fontColor.g, fontColor.b);
        DoRasterString(0., 0., 0., text);
    glPopMatrix();
}


void GUIElement::Update(float mouseX, float mouseY, bool click, glm::vec2 offset)
{
    // handle hover animation
    if (IsHovering(mouseX, mouseY, offset) != hovering)
    {
        hovering = IsHovering(mouseX, mouseY, offset);
        hovering ? DoHover() : HaltHover();
    }

    // check for click
    if (click && hovering) OnClick();

    // try to update properties
    Animate();

    Display(mouseX, mouseY, offset);
}


void GUIElement::OnClick()
{
    if (callback) callback();
}


bool GUIElement::IsHovering(float mouseX, float mouseY, glm::vec2 offset)
{
    return
        mouseX > x + offset.x &&
        mouseX < x + offset.x + width &&
        mouseY > y + offset.y &&
        mouseY < y + offset.y + height;
}


/*
 * Start a hover animation
 */
void GUIElement::DoHover()
{
    // check if animation is already playing
    if (!anim)
    {
        anim = new Anim{
            glutGet(GLUT_ELAPSED_TIME),
            animationLength,
            OUT_CUBIC,
            defaultBackgroundColor,
            hoverBackgroundColor
        };
    }
    else
    {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        float deltaTime = currentTime - anim->startTime;
        float adjustedLength = animationLength - deltaTime;

        delete anim;
        anim = new Anim{
            currentTime,
            adjustedLength,
            OUT_CUBIC,
            defaultBackgroundColor,
            hoverBackgroundColor
        };
    }
}


/*
 * Start an end-hover animation
 */
void GUIElement::HaltHover()
{
    // check if animation is already playing
    if (!anim)
    {
        anim = new Anim{
            glutGet(GLUT_ELAPSED_TIME),
            animationLength,
            IN_SINE,
            hoverBackgroundColor,
            defaultBackgroundColor
        };
    }
    else
    {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        float deltaTime = currentTime - anim->startTime;
        float adjustedLength = animationLength - deltaTime;

        delete anim;
        anim = new Anim{
            currentTime,
            adjustedLength,
            IN_SINE,
            hoverBackgroundColor,
            defaultBackgroundColor
        };
    }
}

