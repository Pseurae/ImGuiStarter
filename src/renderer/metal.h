#pragma once

#include "renderer/renderer.h"

class MetalRenderer final : public RendererImpl
{
    virtual void Setup(GLFWwindow *window) override;
    virtual void Shutdown(GLFWwindow *window) override;
    virtual void StartFrame(GLFWwindow *window) override;
    virtual void EndFrame(GLFWwindow *window) override;
    virtual void *ConstructTexture(const unsigned char *data, int width, int height) override;
    virtual void DestroyTexture(void *texture) override;
};


