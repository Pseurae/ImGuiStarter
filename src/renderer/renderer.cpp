#include "renderer/renderer.h"
#include <memory>

#include "renderer/metal.h"
#include "renderer/opengl.h"

std::unique_ptr<RendererImpl> Renderer::s_Renderer;

void Renderer::Init()
{
#if defined(OPENGL_RENDERER)
    s_Renderer = std::make_unique<OpenGLRenderer>();
#elif defined(METAL_RENDERER)
    s_Renderer = std::make_unique<MetalRenderer>();
#else
    TS_LOG_FATAL("No renderer selected.");
    std::abort();
#endif
}

void Renderer::Setup(GLFWwindow *window)
{
    s_Renderer->Setup(window);
}

void Renderer::Shutdown(GLFWwindow *window)
{
    s_Renderer->Shutdown(window);
}

void Renderer::StartFrame(GLFWwindow *window)
{
    s_Renderer->StartFrame(window);
}

void Renderer::EndFrame(GLFWwindow *window)
{
    s_Renderer->EndFrame(window);
}

void *Renderer::ConstructTexture(const unsigned char *data, int width, int height)
{
    return s_Renderer->ConstructTexture(data, width, height);
}

void Renderer::DestroyTexture(void *texture)
{
    s_Renderer->DestroyTexture(texture);
}
