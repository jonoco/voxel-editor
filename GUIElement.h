#pragma once

#include "GUINode.h"

#include "Easing.h"

class GUIElement :
    public GUINode
{
public:
    struct Anim
    {
        int startTime;
        float length;
        EASING easing;

        glm::vec3 startBackgroundColor;
        glm::vec3 finalBackgroundColor;
    };

    Anim* anim;              // current animation
    float animationLength;
    glm::vec3 hoverBackgroundColor;
    glm::vec3 defaultBackgroundColor;
    bool hovering;

    GUIElement(float x, float y, float width, float height);
 
    void Display(float mouseX, float mouseY, glm::vec2 offset = { 0., 0. }) override;
    void Update(float mouseX, float mouseY, bool click, glm::vec2 offset) override;
    void OnClick() override;

protected:
    void Animate();
    void AddAnimation(Anim newAnim);
    bool IsHovering(float mouseX, float mouseY, glm::vec2 offset);
    void DoHover();
    void HaltHover();
};



