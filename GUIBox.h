#pragma once

#include <vector>

#include "GUINode.h"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"
#include "glm/detail/_noise.hpp"

class GUIBox :
    public GUINode
{
public:
    std::vector<GUINode*> nodes;

    Justify nodeJustification;

    GUIBox(float x, float y, float width, float height);


    void Display(float mouseX, float mouseY, glm::vec2 offset = { 0., 0. }) override;
    void Update(float mouseX, float mouseY, bool click, glm::vec2 offset) override;
    void AddChild(GUINode* node);
    void CenterChildren();
};

