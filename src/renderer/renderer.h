#pragma once

#include <memory>
struct GLFWwindow;

class RendererImpl
{
public:
    virtual ~RendererImpl() = default;
    virtual void Setup(GLFWwindow *window) = 0;
    virtual void Shutdown(GLFWwindow *window) = 0;
    virtual void StartFrame(GLFWwindow *window) = 0;
    virtual void EndFrame(GLFWwindow *window) = 0;
    virtual void *ConstructTexture(const unsigned char *data, int width, int height) = 0;
    virtual void DestroyTexture(void *texture) = 0;
};

class Renderer
{
public:
    static void Init();
    static void Setup(GLFWwindow *window);
    static void Shutdown(GLFWwindow *window);
    static void StartFrame(GLFWwindow *window);
    static void EndFrame(GLFWwindow *window);
    static void *ConstructTexture(const unsigned char *data, int width, int height);
    static void DestroyTexture(void *texture);

    static std::unique_ptr<RendererImpl> s_Renderer;
};

