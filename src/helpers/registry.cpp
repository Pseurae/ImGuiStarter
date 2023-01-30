#include "helpers/registry.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/hotkeys.h"
#include "core/context.h"

#include <GLFW/glfw3.h>
#include <functional>
#include <imgui.h>

static const fs::DialogFilter s_TilemapFilters = { { "Tilemaps", "bin" } };
static const fs::DialogFilter s_TilesetFilters = { { "Tilesets", "png" } };

static fs::path s_CachedPath;
static std::function<void()> s_YesCallback, s_NoCallback;
static std::string s_Message;

void RegisterEvents()
{
}

void RegisterHotKeys()
{
}

static constexpr ImGuiWindowFlags s_PopupFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize;

void DrawPopups()
{
    if (ImGui::BeginPopupModal("###ExamplePopup", NULL, s_PopupFlags))
    {
        ImGui::Text("%s", s_CachedPath.c_str());

        if (ImGui::Button("Ok"))
        {
            if (s_YesCallback)
                s_YesCallback();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel"))
        {
            if (s_NoCallback)
                s_NoCallback();
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
