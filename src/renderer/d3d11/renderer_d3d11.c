#include "renderer/renderer.h"

#include <d3d11.h>
#include <dxgi1_6.h>
#include <stdio.h>
#include "error_code.h"

typedef struct JD3D11Renderer_st
{
    ID3D11Device* device;
    ID3D11DeviceContext* device_context;
    IDXGIFactory6* factory;
    IDXGISwapChain* swapchain;                                // Swap chain
    ID3D11RenderTargetView* render_target_view;               // Backbuffer render target view
    ID3D11Texture2D* depth_stencil_buffer;
    ID3D11DepthStencilState* depth_stencil_state;
    ID3D11DepthStencilState* depth_disabled_stencil_state;    // Disabled depth stencil
    ID3D11DepthStencilView* depth_stencil_view;               // Depth/Stencil view
    D3D11_VIEWPORT viewport;                                  // Viewport
    ID3D11BlendState* blend_state;                            // Color mix settings
    ID3D11RasterizerState* rasterizer_state_solid;            // Solid Rasterizer state
    ID3D11RasterizerState* rasterizer_state_wireframe;        // Wireframe Rasterizer state
} JD3D11Renderer;

typedef struct JRenderer
{
    JD3D11Renderer* data;
} JRenderer;

#if JOJ_DEBUG_MODE
ID3D11Debug* g_debug;
#endif // JOJ_DEBUG_MODE

b8 g_initialized = FALSE;

ErrorCode renderer_init(JRenderer* renderer)
{
    if (g_initialized == TRUE) {
        printf("JRenderer was already initialized.\n");
        return OK;
    }

    if (renderer == NULL) {
        printf("JRenderer is NULL. Allocating memory...\n");
        renderer = (JRenderer*)malloc(sizeof(JRenderer));

        if (renderer == NULL) {
            printf("Failed to allocate memory for JRenderer.\n");
            return FAIL;
        }
    }

    renderer->data = (JD3D11Renderer*)malloc(sizeof(JD3D11Renderer));
    if (renderer->data == NULL) {
        printf("Failed to allocate memory for JD3D11Renderer.\n");
        return FAIL;
    }

    JD3D11Renderer* data = (JD3D11Renderer*)renderer->data;

    data->device = NULL;
    data->device_context = NULL;
    data->factory = NULL;
    data->swapchain = NULL;
    data->render_target_view = NULL;
    data->depth_stencil_buffer = NULL;
    data->depth_stencil_state = NULL;
    data->depth_disabled_stencil_state = NULL;
    data->depth_stencil_view = NULL;
    data->viewport.TopLeftX = 0.0f;
    data->viewport.TopLeftY = 0.0f;
    data->viewport.Width = 800;
    data->viewport.Height = 600;
    data->viewport.MinDepth = 0.0f;
    data->viewport.MaxDepth = 1.0f;
    data->blend_state = NULL;
    data->rasterizer_state_solid = NULL;
    data->rasterizer_state_wireframe = NULL;

    g_initialized = TRUE;

    return OK;
}

void renderer_shutdown(JRenderer* renderer)
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    free(renderer->data);

    g_initialized = FALSE;
}

void renderer_print(JRenderer* renderer)
{
    if (g_initialized == TRUE) {
        printf("Print from JRenderer!\n");
        return;
    }
}