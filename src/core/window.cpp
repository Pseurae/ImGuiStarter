#include "core/window.h"
#include "core/context.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/hotkeys.h"
#include "helpers/logger.h"
#include "imgui_internal.h"
#include <cstdio>
#include <functional>
#include <string_view>
#include <iostream>
#include <format>

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <cstddef>
#include <cstdlib>
#include <string>

#include "core/pane.h"
#include "helpers/registry.h"
#include "renderer/renderer.h"

#include "helpers/texture.h"

static Texture *texture;

static void glfw_error_callback(int error, const char* description)
{
    TS_LOG_FATAL("[GLFW Error %d]: %s", error, description);
}

Window::Window(const std::string &title, int width, int height) : m_Title(title), m_Width(width), m_Height(height)
{
    Logger::Init();

    this->InitGLFW();
    Renderer::Init();
    this->InitImGui();

    TS_LOG_INFO("Created window ('%s' %i %i)", title.c_str(), width, height);

    RegisterEvents();
    RegisterHotKeys();
    texture = new Texture("test.png");
}

Window::~Window()
{ 
    this->ExitImGui();
    this->ExitGLFW();
}

void Window::InitGLFW()
{
    if (!glfwInit())
    {
        TS_LOG_FATAL("Failed to initialize GLFW.");
        std::abort();
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#if defined(OS_MACOS)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);
    if (m_Window == nullptr)
    {
        TS_LOG_FATAL("Failed to create GLFW Window.");
        std::abort();
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    glfwSetWindowUserPointer(m_Window, this);

    glfwSetKeyCallback(m_Window, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        Window *win = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (action == GLFW_PRESS || action == GLFW_REPEAT) 
        {
            HotKeyManager::AddKey(key);
#if defined(OS_MACOS)
            int temp = mods & (GLFW_MOD_SUPER | GLFW_MOD_CONTROL);
            if (temp == GLFW_MOD_SUPER)
            {
                mods &= ~GLFW_MOD_SUPER;
                mods |= GLFW_MOD_CONTROL;
            }
            else if (temp == GLFW_MOD_CONTROL)
            {
                mods &= ~GLFW_MOD_CONTROL;
                mods |= GLFW_MOD_SUPER;
            }
#endif
            HotKeyManager::SetMods(mods);
            win->m_HandleEvent = true;
        }
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *window, int button, int action, int mods) {
        Window *win = static_cast<Window *>(glfwGetWindowUserPointer(window));
        win->m_HandleEvent = true;
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *window, double xpos, double ypos) {
        Window *win = static_cast<Window *>(glfwGetWindowUserPointer(window));
        win->m_HandleEvent = true;
    });

    int win_w, win_h, fb_w, fb_h;
    glfwGetWindowSize(m_Window, &win_w, &win_h);
    glfwGetFramebufferSize(m_Window, &fb_w, &fb_h);

    m_DPIScaling = std::max((float)fb_w / win_w, (float)fb_h / win_h);

    glfwShowWindow(m_Window);

    TS_LOG_INFO("GLFW initialized.");
}

void Window::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGuiIO &io = ImGui::GetIO();

    Renderer::Setup(m_Window);

    TS_LOG_INFO("ImGui initialized.");
}

void Window::ExitGLFW()
{
    glfwDestroyWindow(this->m_Window);
    glfwTerminate();
    TS_LOG_INFO("GLFW shutdown.");
}

void Window::ExitImGui()
{
    Renderer::Shutdown(m_Window);
    ImGui::DestroyContext();
    TS_LOG_INFO("ImGui shutdown.");
}

void Window::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        if (glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) || 
            !glfwGetWindowAttrib(m_Window, GLFW_VISIBLE))
        {
            glfwWaitEvents();
        }
        else
        {
            glfwPollEvents();
            if (!m_HandleEvent || !glfwGetWindowAttrib(m_Window, GLFW_FOCUSED))
            {
                glfwWaitEventsTimeout(1.0f / 10.0f);
            }
            m_HandleEvent = false;
        }

        this->PreFrame();
        this->Frame();
        this->PostFrame();
    }
}

void Window::PreFrame()
{
    Renderer::StartFrame(m_Window);
    ImGui::NewFrame();
}

static constexpr ImGuiWindowFlags s_WindowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse;

void Window::Frame()
{
    ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);

    MenuBar menu_bar = Context::GetMenuBar();

    ImGui::Begin("Window", NULL, s_WindowFlags | (menu_bar.empty() ? 0 : ImGuiWindowFlags_MenuBar));

    if (!menu_bar.empty())
    {
        ImGui::BeginMenuBar();
        for (auto &[name, callback] : Context::GetMenuBar())
        {
            if (ImGui::BeginMenu(name.c_str()))
            {
                callback();
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenuBar();
    }

    for (Pane *pane : m_Panes)
    {
        pane->Draw();
    }

    if (!ImGui::GetTopMostPopupModal())
        HotKeyManager::ProcessHotKeys();

    HotKeyManager::ClearKeys();

    Context::ProcessPopups();
    DrawPopups();

    ImGui::End();
}

void Window::PostFrame()
{
    ImGui::Render();
    Renderer::EndFrame(m_Window);
}
