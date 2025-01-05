#include "renderer/renderer.h"

#define COBJMACROS
#include <initguid.h>
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

typedef struct JRenderer_st
{
    JD3D11Renderer* data;
} JRenderer;

#if JOJ_DEBUG_MODE
ID3D11Debug* g_debug;
#endif // JOJ_DEBUG_MODE

JRenderer* g_renderer = NULL;
b8 g_initialized = FALSE;

ErrorCode renderer_init()
{
    if (g_initialized == TRUE) {
        printf("JRenderer was already initialized.\n");
        return OK;
    }

    if (g_renderer == NULL) {
        printf("JRenderer is NULL. Allocating memory...\n");
        g_renderer = (JRenderer*)malloc(sizeof(JRenderer));

        if (g_renderer == NULL) {
            printf("Failed to allocate memory for JRenderer.\n");
            return FAIL;
        }
    }

    g_renderer->data = (JD3D11Renderer*)malloc(sizeof(JD3D11Renderer));
    if (g_renderer->data == NULL) {
        printf("Failed to allocate memory for JD3D11Renderer.\n");
        return FAIL;
    }

    JD3D11Renderer* data = (JD3D11Renderer*)g_renderer->data;

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

    // ---------------------------------------------------
    // Create DXGI infrastructure and D3D device
    // ---------------------------------------------------

    u32 factory_flags = 0;
    u32 create_device_flags = 0;

    factory_flags = DXGI_CREATE_FACTORY_DEBUG;

#if JOJ_DEBUG_MODE
    create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // JOJ_DEBUG_MODE

    if (CreateDXGIFactory2(factory_flags, &IID_IDXGIFactory6, (void**)&data->factory) != S_OK) {
        printf("Failed to create D3D11 DXGIFactory2.\n");
        return FAIL;
    }

    // Level of D3D features supported by hardware
    D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;

    if (D3D11CreateDevice(
        NULL,                            // Default adapter
        D3D_DRIVER_TYPE_HARDWARE,        // D3D driver type (Hardware, Reference or Software)
        NULL,                            // Pointer to software rasterizer - No software device
        create_device_flags,             // Debug or Normal Mode
        NULL,                            // Default feature level (NULL = max supported)
        0,                               // Size of feature level array
        D3D11_SDK_VERSION,               // Direct3D SDK version
        &data->device,                  // Stores D3D device created
        &feature_level,                  // Current Direct3D version in use,
        &data->device_context)          // D3D context device
        != S_OK)
    {
        printf("Failed to create D3D11Device. Creating D3D11 Warp adapter...\n");

        if (D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_WARP,
            NULL, create_device_flags, NULL, 0, D3D11_SDK_VERSION,
            &data->device, &feature_level, &data->device_context) != S_OK)
        {
            printf("Failed to create WARP Adapter.\n");
            return FAIL;
        }
    }

#if JOJ_DEBUG_MODE
    if (ID3D11Debug_QueryInterface(data->device, &IID_ID3D11Debug, (void**)&g_debug) != S_OK) {
        printf("Failed to QueryInterface of ID3D11Debug.\n");
    }
#endif // JOJ_DEBUG_MODE

    IDXGIDevice* dxgi_device = NULL;
    if (IDXGIDevice_QueryInterface(data->device, &IID_IDXGIDevice, (void**)&dxgi_device) != S_OK) {
        printf("Failed to QueryInterface of DXGIDevice.\n");
        return FAIL;
    }

    // Get Adpapter from Direct3D device (d3d11Device)
    IDXGIAdapter* dxgi_adapter = NULL;
    if (IDXGIAdapter_GetParent(dxgi_device, &IID_IDXGIAdapter, (void**)&dxgi_adapter) != S_OK)
    {
        printf("Failed to GetParent of IDXGIAdapter.\n");
        return FAIL;
    }

    // Get pointer to adapter DXGIFactory
    IDXGIFactory2* dxgi_factory = NULL;
    if (IDXGIFactory2_GetParent(dxgi_adapter, &IID_IDXGIFactory2, (void**)&dxgi_factory) != S_OK)
    {
        printf("Failed to GetParent of IDXGIFactory2.\n");
        return FAIL;
    }

    dxgi_device->lpVtbl->Release(dxgi_device);
    dxgi_adapter->lpVtbl->Release(dxgi_adapter);
    dxgi_factory->lpVtbl->Release(dxgi_factory);

    g_initialized = TRUE;

    return OK;
}

void renderer_shutdown()
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    JD3D11Renderer* data = (JD3D11Renderer*)g_renderer->data;

    // Release factory
    if (data->factory)
    {
        data->factory->lpVtbl->Release(data->factory);
        data->factory = NULL;
    }

    // Release device context
    if (data->device_context)
    {
        // Restores to original state
        data->device_context->lpVtbl->ClearState(data->device_context);
        data->device_context->lpVtbl->Release(data->device_context);
        data->device_context = NULL;
    }

    // Release device
    if (data->device)
    {
        data->device->lpVtbl->Release(data->device);
        data->device = NULL;
    }

#if JOJ_DEBUG_MODE
    if (g_debug)
    {
        // Report live objects
        g_debug->lpVtbl->ReportLiveDeviceObjects(g_debug, D3D11_RLDO_DETAIL);
        g_debug->lpVtbl->Release(g_debug);
        g_debug = NULL;
    }
#endif // JOJ_DEBUG_MODE

    free(g_renderer->data);

    g_initialized = FALSE;
}

void renderer_print()
{
    if (g_initialized == TRUE) {
        printf("Print from JRenderer!\n");
        return;
    }
}