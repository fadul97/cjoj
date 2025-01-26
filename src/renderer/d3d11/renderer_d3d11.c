#include "renderer/renderer.h"

#define COBJMACROS
#include <initguid.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <stdio.h>
#include "error_code.h"
#include "platform/platform.h"

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
void log_hardware_info();
#endif // JOJ_DEBUG_MODE

JRenderer* g_renderer = NULL;
b8 g_initialized = FALSE;
u32 g_4xmsaa_quality = 0;                     // Default quality
b8 g_vsync = TRUE;                            // Vertical sync
u32 g_buffer_count = 2;

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

    g_initialized = TRUE;

#ifdef JOJ_DEBUG_MODE
    log_hardware_info();
#endif // JOJ_DEBUG_MODE

    dxgi_device->lpVtbl->Release(dxgi_device);
    dxgi_adapter->lpVtbl->Release(dxgi_adapter);
    dxgi_factory->lpVtbl->Release(dxgi_factory);

    if (data->device->lpVtbl->CheckMultisampleQualityLevels(data->device, DXGI_FORMAT_R8G8B8A8_UNORM,
        4, &g_4xmsaa_quality) != S_OK)
    {
        // TODO: Better ErrorCode
        printf("Failed to check multi sample quality levels. Setting it to 0.\n");
        g_4xmsaa_quality = 0;
    }

    // ------------------------------------------------------------------------------------------------------
    //                                          PIPELINE SETUP
    // ------------------------------------------------------------------------------------------------------

    // ---------------------------------------------------
    // SwapChain
    // ---------------------------------------------------

    u16 width, height;
    window_get_size(&width, &height);

    HWND hwnd = (HWND)platform_get_handle();
    WindowMode window_mode = platform_get_window_mode();

    // Describe Swap Chain
    DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
    swap_chain_desc.BufferDesc.Width = (u32)width;                                          // Back buffer width
    swap_chain_desc.BufferDesc.Height = (u32)height;                                        // Back buffer height
    swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;                                  // Refresh rate in hertz 
    swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;                                 // Denominator is an int
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;                         // Color format - RGBA 8 bits
    swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;     // Default value for Flags
    swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;                     // Default mode for scaling

    // No MSAA
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SampleDesc.Quality = 0;

    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;                          // Use surface as Render Target

    // Check value
    swap_chain_desc.BufferCount = g_buffer_count;                                           // Number of buffers (Front + Back)

    swap_chain_desc.OutputWindow = hwnd;                                                    // Window ID
    swap_chain_desc.Windowed = (window_mode == WINDOWED);                                   // Fullscreen or windowed 
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;                             // Discard surface after presenting
    swap_chain_desc.Flags = 0;                                                              // Use Back buffer size for Fullscreen

    // Create Swap Chain
    if (IDXGIFactory6_CreateSwapChain(data->factory, data->device, &swap_chain_desc,
        &data->swapchain) != S_OK)
    {
        printf("Failed to create SwapChain.\n");
        return FAIL;
    }
    
    // ---------------------------------------------------
    // Render Target View
    // ---------------------------------------------------

    // Get backbuffer surface of a Swap Chain
    ID3D11Texture2D* backbuffer = NULL;
    if (IDXGISwapChain_GetBuffer(data->swapchain, 0, &IID_ID3D11Texture2D,
        (void**)&backbuffer) != S_OK)
    {
        printf("Failed to Get backbuffer surface of a Swap Chain.\n");
        return FAIL;
    }

    // Create render target view for backbuffer
    if (ID3D11Device_CreateRenderTargetView(data->device, (ID3D11Resource*)backbuffer, NULL,
        &data->render_target_view) != S_OK)
    {
        printf("Failed to create Render Target View.\n");
        return FAIL;
    }

    // ---------------------------------------------------
    // Depth/Stencil Buffer
    // ---------------------------------------------------

    // Describe Depth/Stencil Buffer
    D3D11_TEXTURE2D_DESC depth_stencil_tex2d_desc = { 0 };
    depth_stencil_tex2d_desc.Width = (u32)width;                               // Depth/Stencil buffer width
    depth_stencil_tex2d_desc.Height = (u32)height;                             // Depth/Stencil buffer height
    depth_stencil_tex2d_desc.MipLevels = 0;                                    // Number of mipmap levels
    depth_stencil_tex2d_desc.ArraySize = 1;                                    // Number of textures in array
    depth_stencil_tex2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;           // Color format - Does it need to be the same format of swapChainDesc?

    // No MSAA
    depth_stencil_tex2d_desc.SampleDesc.Count = 1;                         // Samples per pixel (antialiasing)
    depth_stencil_tex2d_desc.SampleDesc.Quality = 0;                       // Level of image quality

    depth_stencil_tex2d_desc.Usage = D3D11_USAGE_DEFAULT;                      // Default - GPU will both read and write to the resource
    depth_stencil_tex2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;             // Where resource will be bound to the pipeline
    depth_stencil_tex2d_desc.CPUAccessFlags = 0;                               // CPU will not read not write to the Depth/Stencil buffer
    depth_stencil_tex2d_desc.MiscFlags = 0;                                    // Optional flags

    // Create Depth/Stencil Buffer
    if (ID3D11Device_CreateTexture2D(data->device, &depth_stencil_tex2d_desc, NULL,
        &data->depth_stencil_buffer) != S_OK)
    {
        printf("Failed to create Depth/Stencil Buffer.\n");
        return FAIL;
    }

    // ---------------------------------------------------
    // Depth/Stencil State - Depth Enabled
    // ---------------------------------------------------

    // Describe Depth Stencil
    D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = { 0 };

    // Set up the description of the stencil state.
    depth_stencil_desc.DepthEnable = TRUE;
    depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

    depth_stencil_desc.StencilEnable = TRUE;
    depth_stencil_desc.StencilReadMask = 0xFF;
    depth_stencil_desc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depth_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depth_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depth_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depth_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create Depth Stencil State
    if (ID3D11Device_CreateDepthStencilState(data->device, &depth_stencil_desc,
        &data->depth_stencil_state) != S_OK)
    {
        printf("Failed to create Depth Stencil State.\n");
        return FAIL;
    }

    // Set Depth Stencil State
    ID3D11DeviceContext_OMSetDepthStencilState(data->device_context, data->depth_stencil_state, 1);

    // ---------------------------------------------------
    // Depth/Stencil State - Depth Disabled
    // ---------------------------------------------------

    // Describe Depth Stencil
    D3D11_DEPTH_STENCIL_DESC depth_disabled_stencil_desc = { 0 };
    ZeroMemory(&depth_disabled_stencil_desc, sizeof(depth_disabled_stencil_desc));

    // Create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    depth_disabled_stencil_desc.DepthEnable = FALSE;
    depth_disabled_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_disabled_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
    depth_disabled_stencil_desc.StencilEnable = TRUE;
    depth_disabled_stencil_desc.StencilReadMask = 0xFF;
    depth_disabled_stencil_desc.StencilWriteMask = 0xFF;
    depth_disabled_stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_disabled_stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depth_disabled_stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_disabled_stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depth_disabled_stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depth_disabled_stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depth_disabled_stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depth_disabled_stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create Depth Stencil State
    if (ID3D11Device_CreateDepthStencilState(data->device, &depth_disabled_stencil_desc,
        &data->depth_disabled_stencil_state) != S_OK)
    {
        printf("Failed to create Depth Disabled Stencil State.\n");
        return FAIL;
    }

    // ---------------------------------------------------
    // Depth/Stencil View
    // ---------------------------------------------------

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc = { 0 };
    ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

    // Set up the depth stencil view description.
    depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

    // Create Depth/Stencil View
    if (ID3D11Device_CreateDepthStencilView(data->device, (ID3D11Resource*)data->depth_stencil_buffer,
        &depth_stencil_view_desc, &data->depth_stencil_view) != S_OK)
    {
        printf("Failed to create Depth Stencil View.\n");
        return FAIL;
    }

    // Bind render target and depth stencil to the Output Merger stage
    ID3D11DeviceContext_OMSetRenderTargets(data->device_context, 1, &data->render_target_view,
        data->depth_stencil_view);

    // ---------------------------------------------------
    // Viewport
    // ---------------------------------------------------

    // Describe Viewport
    data->viewport.TopLeftX = 0.0f;
    data->viewport.TopLeftY = 0.0f;
    data->viewport.Width = (f32)width;
    data->viewport.Height = (f32)height;
    data->viewport.MinDepth = 0.0f;
    data->viewport.MaxDepth = 1.0f;

    // Set Viewport
    ID3D11DeviceContext_RSSetViewports(data->device_context, 1, &data->viewport);

    // ---------------------------------------------
    // Blend State
    // ---------------------------------------------

    // Describe blend state
    D3D11_BLEND_DESC blend_desc = { 0 };
    blend_desc.AlphaToCoverageEnable = FALSE;                                // Highlight the silhouette of sprites
    blend_desc.IndependentBlendEnable = FALSE;                               // Use the same mix for all render targets
    blend_desc.RenderTarget[0].BlendEnable = TRUE;                           // Enable blending
    blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;             // Source mixing factor
    blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;        // Target of RGB mixing is inverted alpha
    blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;                 // Addition operation in color mixing
    blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;        // Alpha blend source is the alpha of the pixel shader
    blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;   // Fate of Alpha mixture is inverted alpha
    blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;            // Addition operation in color mixing
    blend_desc.RenderTarget[0].RenderTargetWriteMask = 0x0F;                 // Components of each pixel that can be overwritten

    // Create blend state
    if (ID3D11Device_CreateBlendState(data->device, &blend_desc, &data->blend_state) != S_OK)
    {
        printf("Failed to create Blend State.\n");
        return FAIL;
    }

    // Bind blend state to the Output Merger stage
    ID3D11DeviceContext_OMSetBlendState(data->device_context, data->blend_state, NULL, 0xFFFFFFFF);

    // ---------------------------------------------------
    // Rasterizer
    // ---------------------------------------------------

    // Describe rasterizer
    D3D11_RASTERIZER_DESC rasterizer_desc = { 0 };
    ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
    rasterizer_desc.FillMode = D3D11_FILL_SOLID;
    rasterizer_desc.CullMode = D3D11_CULL_NONE;
    rasterizer_desc.DepthClipEnable = TRUE;

    // Create Solid rasterizer state
    if (ID3D11Device_CreateRasterizerState(data->device, &rasterizer_desc, &data->rasterizer_state_solid) != S_OK)
    {
        printf("Failed to create Solid Rasterizer State.\n");
        return FAIL;
    }

    rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
    rasterizer_desc.CullMode = D3D11_CULL_BACK;

    // Create Wireframe rasterizer state
    if (ID3D11Device_CreateRasterizerState(data->device, &rasterizer_desc, &data->rasterizer_state_wireframe) != S_OK)
    {
        printf("Failed to create Wireframe Rasterizer State.\n");
        return FAIL;
    }

    // Set Solid rasterizer state as default
    ID3D11DeviceContext_RSSetState(data->device_context, data->rasterizer_state_solid);

    // ---------------------------------------------------
    //	Release Resources
    // ---------------------------------------------------

    // Release backbuffer
    backbuffer->lpVtbl->Release(backbuffer);

    return OK;
}

void renderer_shutdown()
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    JD3D11Renderer* data = (JD3D11Renderer*)g_renderer->data;

    // Release wireframe rasterizer state
    if (data->rasterizer_state_wireframe)
    {
        data->rasterizer_state_wireframe->lpVtbl->Release(data->rasterizer_state_wireframe);
        data->rasterizer_state_wireframe = NULL;
    }

    // Release solid rasterizer state
    if (data->rasterizer_state_solid)
    {
        data->rasterizer_state_solid->lpVtbl->Release(data->rasterizer_state_solid);
        data->rasterizer_state_solid = NULL;
    }

    // Release blend state
    if (data->blend_state)
    {
        data->blend_state->lpVtbl->Release(data->blend_state);
        data->blend_state = NULL;
    }

    // Release depth stencil view
    if (data->depth_stencil_view)
    {
        data->depth_stencil_view->lpVtbl->Release(data->depth_stencil_view);
        data->depth_stencil_view = NULL;
    }

    // Release Depth Disabled Stencil State
    if (data->depth_disabled_stencil_state)
    {
        data->depth_disabled_stencil_state->lpVtbl->Release(data->depth_disabled_stencil_state);
        data->depth_disabled_stencil_state = NULL;
    }

    // Release Depth Stencil State
    if (data->depth_stencil_state)
    {
        data->depth_stencil_state->lpVtbl->Release(data->depth_stencil_state);
        data->depth_stencil_state = NULL;
    }

    // Release Depth Stencil Buffer
    if (data->depth_stencil_buffer)
    {
        data->depth_stencil_buffer->lpVtbl->Release(data->depth_stencil_buffer);
        data->depth_stencil_buffer = NULL;
    }

    // Release render target view
    if (data->render_target_view)
    {
        data->render_target_view->lpVtbl->Release(data->render_target_view);
        data->render_target_view = NULL;
    }

    // Release swap chain
    if (data->swapchain)
    {
        // Direct3D is unable to close when full screen
        data->swapchain->lpVtbl->SetFullscreenState(data->swapchain, FALSE, NULL);
        data->swapchain->lpVtbl->Release(data->swapchain);
        data->swapchain = NULL;
    }

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

void renderer_start_frame(f32 r, f32 g, f32 b, f32 a)
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    f32 bgcolor[4] = { r, g, b, a };

    JD3D11Renderer* data = (JD3D11Renderer*)g_renderer->data;

    ID3D11DeviceContext_ClearRenderTargetView(data->device_context, data->render_target_view, bgcolor);
    ID3D11DeviceContext_ClearDepthStencilView(data->device_context, data->depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void renderer_end_frame()
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    JD3D11Renderer* data = (JD3D11Renderer*)g_renderer->data;

    if (g_vsync)
    {
        // Lock to screen refresh rate
        if (IDXGISwapChain_Present(data->swapchain, 1, 0) != S_OK)
        {
            printf("Failed to present SwapChain.\n");
        }
    }
    else
    {
        // Present as fast as possible
        if (IDXGISwapChain_Present(data->swapchain, 0, 0) != S_OK)
        {
            printf("Failed to present SwapChain.\n");
        }
    }
}

void renderer_print()
{
    if (g_initialized == TRUE) {
        printf("Print from JRenderer!\n");
        return;
    }
}

#if JOJ_DEBUG_MODE
void log_hardware_info()
{
    if (g_initialized == FALSE) {
        printf("JRenderer NOT initialized.\n");
        return;
    }

    JD3D11Renderer* data = (JD3D11Renderer*)g_renderer->data;

    const u32 bytes_in_megabyte = 1048576U;

    // ---------------------------------------------------
    // Video adapter (Graphics card)
    // ---------------------------------------------------

    IDXGIAdapter* adapter = NULL;
    if (data->factory->lpVtbl->EnumAdapters(data->factory, 0, &adapter) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_ADAPTER_DESC desc;
        IDXGIAdapter_GetDesc(adapter, &desc);

        char graphics_card[128];
        size_t converted_chars = 0;
        wcstombs_s(&converted_chars, graphics_card, sizeof(graphics_card), desc.Description, _TRUNCATE);

        printf("---> Graphics card: %s.\n", graphics_card);
    }

    IDXGIAdapter4* adapter4 = NULL;
    if (SUCCEEDED(adapter->lpVtbl->QueryInterface(adapter, &IID_IDXGIAdapter4, (void**)&adapter4)))
    {
        DXGI_QUERY_VIDEO_MEMORY_INFO mem_info;
        adapter4->lpVtbl->QueryVideoMemoryInfo(adapter4, 0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &mem_info);

        printf("---> Video memory (free): %lluMB.\n", mem_info.Budget / bytes_in_megabyte);

        // FIXME: Memory used is wrong
        printf("---> Video memory(used) : %lluMB.\n", mem_info.CurrentUsage / bytes_in_megabyte);

        adapter4->lpVtbl->Release(adapter4);
    }

    // ---------------------------------------------------
    // Max feature Level supported by GPU
    // ---------------------------------------------------

    // Instructions block
    {
        printf("---> Feature Level: 11_0.\n");
    }

    // ---------------------------------------------------
    // Video output (monitor)
    // ---------------------------------------------------

    IDXGIOutput* output = NULL;
    if (adapter->lpVtbl->EnumOutputs(adapter, 0, &output) != DXGI_ERROR_NOT_FOUND)
    {
        DXGI_OUTPUT_DESC desc;
        IDXGIAdapter_GetDesc(output, &desc);

        char device_name[32];
        size_t converted_chars = 0;
        wcstombs_s(&converted_chars, device_name, sizeof(device_name), desc.DeviceName, _TRUNCATE);

        printf("---> Monitor: %s.\n", device_name);
    }

    // ----------------------------------------------------
    // Video mode (resolution)
    // ----------------------------------------------------

    // Get screen dimensions
    u32 dpi = GetDpiForSystem();
    u32 screen_width = GetSystemMetricsForDpi(SM_CXSCREEN, dpi);
    u32 screen_height = GetSystemMetricsForDpi(SM_CYSCREEN, dpi);

    // Get screen update frequency
    DEVMODE dev_mode = { 0 };
    dev_mode.dmSize = sizeof(DEVMODE);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dev_mode);
    u32 refresh = dev_mode.dmDisplayFrequency;

    printf("---> Resolution: %dx%d %d Hz.\n", screen_width, screen_height, refresh);

    // Release used DXGI interfaces
    if (adapter) adapter->lpVtbl->Release(adapter);
    if (output) output->lpVtbl->Release(output);

}
#endif // JOJ_DEBUG_MODE