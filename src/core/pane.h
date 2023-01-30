#pragma once

#include <string>
#include <imgui.h>

class Pane
{
public:
    static constexpr ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse;

    explicit Pane() = default;
    virtual ~Pane() = 0;
    virtual void Draw() = 0;
private:
};
