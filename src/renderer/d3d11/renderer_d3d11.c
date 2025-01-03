#include "renderer/renderer.h"

#include <d3d11.h>
#include <dxgi1_6.h>
#include <stdio.h>
#include "error_code.h"

typedef struct D3D11Renderer_st
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

#if JOJ_DEBUG_MODE
    ID3D11Debug* m_debug;
#endif // JOJ_DEBUG_MODE
} JD3D11Renderer;

b8 g_initialized = FALSE;

ErrorCode renderer_init(JRenderer* renderer)
{
    if (g_initialized == TRUE) {
        printf("JRenderer was already initialized.\n");
        return OK;
    }

    if (renderer == NULL) {
        printf("JRenderer is NULL.");
        return FAIL;
    }

    renderer->data = (JD3D11Renderer*)malloc(sizeof(JD3D11Renderer));
    if (renderer->data == NULL) {
        printf("Failed to allocate memory for JD3D11Renderer.\n");
        return FAIL;
    }

    g_initialized = TRUE;

    return OK;
}

void renderer_shutdown(JRenderer* renderer)
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    if (renderer == NULL) {
        printf("JRenderer is NULL.");
        return;
    }

    if (renderer->data == NULL) {
        printf("Renderer data is NULL.");
        return;
    }

    free(renderer->data);
}

void renderer_print(JRenderer* renderer)
{
    if (g_initialized == TRUE) {
        printf("Print from JRenderer!\n");
        return;
    }
}