#include "renderer/metal.h"

#if METAL_RENDERER

#include "imgui_internal.h"

#define GLFW_INCLUDE_NONE
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_metal.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

static id <MTLDevice> device;
static id <MTLCommandQueue> commandQueue;
static NSWindow *nswin;
static CAMetalLayer *layer;
static MTLRenderPassDescriptor *renderPassDescriptor;
static id <CAMetalDrawable> drawable;
static id <MTLCommandBuffer> commandBuffer;
static id <MTLRenderCommandEncoder> renderEncoder;

void MetalRenderer::Setup(GLFWwindow *window)
{
    device = MTLCreateSystemDefaultDevice();
    commandQueue = [device newCommandQueue];

    nswin = glfwGetCocoaWindow(window);
    layer = [CAMetalLayer layer];
    layer.device = device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    nswin.contentView.layer = layer;
    nswin.contentView.wantsLayer = YES;

    renderPassDescriptor = [MTLRenderPassDescriptor new];

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplMetal_Init(device);
}

void MetalRenderer::Shutdown(GLFWwindow *window)
{
    ImGui_ImplMetal_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

void MetalRenderer::StartFrame(GLFWwindow *window)
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    layer.drawableSize = CGSizeMake(width, height);
    drawable = [layer nextDrawable];

    commandBuffer = [commandQueue commandBuffer];
    renderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 0.0);
    renderPassDescriptor.colorAttachments[0].texture = drawable.texture;
    renderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
    renderPassDescriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
    renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];

    ImGui_ImplMetal_NewFrame(renderPassDescriptor);
    ImGui_ImplGlfw_NewFrame();
}

void MetalRenderer::EndFrame(GLFWwindow *window)
{
    ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), commandBuffer, renderEncoder);

    [renderEncoder popDebugGroup];
    [renderEncoder endEncoding];

    [commandBuffer presentDrawable:drawable];
    [commandBuffer commit];

    [drawable autorelease];
    [commandBuffer autorelease];
    [renderEncoder autorelease];
}

void *MetalRenderer::ConstructTexture(const unsigned char *data, int width, int height)
{
    MTLTextureDescriptor* textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                 width:(NSUInteger)width
                                                                                                height:(NSUInteger)height
                                                                                             mipmapped:NO];
    textureDescriptor.usage = MTLTextureUsageShaderRead;
#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
    textureDescriptor.storageMode = MTLStorageModeManaged;
#else
    textureDescriptor.storageMode = MTLStorageModeShared;
#endif
    id <MTLTexture> texture = [device newTextureWithDescriptor:textureDescriptor];
    [texture replaceRegion:MTLRegionMake2D(0, 0, (NSUInteger)width, (NSUInteger)height) mipmapLevel:0 withBytes:data bytesPerRow:(NSUInteger)width * 4];
    return (__bridge void *)texture;
}

void MetalRenderer::DestroyTexture(void *texture)
{
    id<MTLTexture> tex = (__bridge id<MTLTexture>)texture;
    [tex release];
}

#endif // METAL_RENDERER

