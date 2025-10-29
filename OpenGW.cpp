// OpenGW.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "defines.h"
#include "OpenGW.h"
#include "scene.h"
#include "game.h"

#include "blur.h"
#include "sincos.h"


#define filterWidth 5 
#define filterHeight 5 
#define imageWidth 320 
#define imageHeight 240

//declare image buffers 
constexpr int blurBufferWidth = 500;
constexpr int blurBufferHeight = 250;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} ColorRGBA;

ColorRGBA blurBuffer[blurBufferWidth][blurBufferHeight];

#define MAX_LOADSTRING 100

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
LPDIRECT3D8             g_pD3D = NULL; // Used to create the D3DDevice
LPDIRECT3DDEVICE8       g_pd3dDevice = NULL; // Our rendering device
LPDIRECT3DVERTEXBUFFER8 g_pVB = NULL; // Buffer to hold vertices

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
    FLOAT x, y, z; // The vertex position
    DWORD color;   // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

FLOAT fSecsPerTick;
LARGE_INTEGER qwTime, qwLastTime, qwElapsedTime, qwAppTime, qwElapsedAppTime;
FLOAT fTime, fElapsedTime, fAppTime, fElapsedAppTime;

/////////////////////////////////////////////////////////////////////////////////////////
scene oglScene;
BOOL oglInited = FALSE;

//our OpenGL texture handles
unsigned int texOffscreen;

void createOffscreens();
void drawOffscreens();

#define CONTEXT_PRIMARY 0
#define CONTEXT_GLOW    1

int mWidth, mHeight;

static DWORD lastTime;
static DWORD fpsTime;
static int frameCount;
static int fps;

/////////////////////////////////////////////////////////////////////////////////////////

void OGLDestroy()
{
    oglInited = FALSE;
    // Other destruct stuff
}

// ***************************************************************************

void OGLSize(int cx, int cy)
{
    oglScene.size(cx, cy);
    mWidth = cx;
    mHeight = cy;
}

// ***************************************************************************

void createOffscreens()
{
    // new array
    char* colorBits = new char[blurBufferWidth * blurBufferHeight * 3];

    // texture creation..
    //glGenTextures(1, &texOffscreen);
    //glBindTexture(GL_TEXTURE_2D, texOffscreen);

    //glTexImage2D(GL_TEXTURE_2D, 0, 3, blurBufferWidth, blurBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBits);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBits);

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // clean up
    delete[] colorBits;

    //glBindTexture(GL_TEXTURE_2D, 0);

}

void drawOffscreens()
{
    int viewport[4];
    //glGetIntegerv(GL_VIEWPORT,(int*)viewport);

    if (theGame.mSettings.mEnableGlow)
    {
        // Draw to the blur texture
        {
            //glViewport(0, 0, blurBufferWidth, blurBufferHeight);
            oglScene.draw(scene::RENDERPASS_BLUR);

            // Transfer image to the blur texture
            //glBindTexture(GL_TEXTURE_2D, texOffscreen);
            //glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, blurBufferWidth, blurBufferHeight, 0);
            //glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Draw the scene normally
    //glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
    oglScene.draw(scene::RENDERPASS_PRIMARY);

    if (theGame.mSettings.mEnableGlow)
    {
        ////////////////////////////////////////////////
        // Do blur

        // Bind the blur texture and copy the screen bits to it
        //glBindTexture(GL_TEXTURE_2D, texOffscreen);
        //glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer);

        if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED)
        {
            superFastBlur((unsigned char*)&blurBuffer[0][0], blurBufferWidth, blurBufferHeight, 8);
            superFastBlur((unsigned char*)&blurBuffer[0][0], blurBufferWidth, blurBufferHeight, 8);
        }
        else
        {
            superFastBlur((unsigned char*)&blurBuffer[0][0], blurBufferWidth, blurBufferHeight, 4);
            superFastBlur((unsigned char*)&blurBuffer[0][0], blurBufferWidth, blurBufferHeight, 4);
        }

        // Bind the blur result back to our texture
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer);

        ////////////////////////////////////////////////
        // Draw the blur texture on top of the existing scene

        // Glowy blending effect
        //glDisable(GL_DEPTH_TEST);
        //glEnable(GL_BLEND);
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        //glEnable( GL_TEXTURE_2D );
        //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

        /*if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED)
            glColor4f(1, 1, 1, 1);
        else
            glColor4f(1, 1, 1, 1);*/

            // Draw it on the screen
            /*
            glBegin(GL_QUADS);
            glTexCoord2d(0.0,0.0); glVertex2d(-1.0, -1.0);
            glTexCoord2d(1.0,0.0); glVertex2d(1.0, -1.0);
            glTexCoord2d(1.0,1.0); glVertex2d(1.0, 1.0);
            glTexCoord2d(0.0,1.0); glVertex2d(-1.0, 1.0);
            if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED)
            {
                glTexCoord2d(0.0,0.0); glVertex2d(-1.0, -1.0);
                glTexCoord2d(1.0,0.0); glVertex2d(1.0, -1.0);
                glTexCoord2d(1.0,1.0); glVertex2d(1.0, 1.0);
                glTexCoord2d(0.0,1.0); glVertex2d(-1.0, 1.0);
            }
            glEnd();

            glBindTexture(GL_TEXTURE_2D, 0);
            glDisable( GL_TEXTURE_2D );
            */
    }
}

// ***************************************************************************

VOID InitTime()
{
    // Get the frequency of the timer
    LARGE_INTEGER qwTicksPerSec;
    QueryPerformanceFrequency(&qwTicksPerSec);
    fSecsPerTick = 1.0f / (FLOAT)qwTicksPerSec.QuadPart;

    // Save the start time
    QueryPerformanceCounter(&qwTime);
    qwLastTime.QuadPart = qwTime.QuadPart;

    qwAppTime.QuadPart = 0;
    qwElapsedTime.QuadPart = 0;
    qwElapsedAppTime.QuadPart = 0;
}

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D()
{
    // Create the D3D object.
    if (NULL == (g_pD3D = Direct3DCreate8(D3D_SDK_VERSION)))
        return E_FAIL;

    // Set up the structure used to create the D3DDevice.
    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory(&d3dpp, sizeof(d3dpp));
    d3dpp.BackBufferWidth = 640;
    d3dpp.BackBufferHeight = 480;
    d3dpp.BackBufferFormat = D3DFMT_LIN_X8R8G8B8;
    d3dpp.BackBufferCount = 1;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.FullScreen_PresentationInterval = D3DPRESENT_INTERVAL_ONE_OR_IMMEDIATE;

    // Create the Direct3D device.
    if (FAILED(g_pD3D->CreateDevice(0, D3DDEVTYPE_HAL, NULL,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &d3dpp, &g_pd3dDevice)))
        return E_FAIL;

    // Set the projection matrix
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 4.0f / 3.0f, 1.0f, 200.0f);
    g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

    // Set the view matrix
    D3DXVECTOR3 vEyePt = D3DXVECTOR3(0.0f, 0.0f, -7.0f);
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    D3DXMATRIX  matView;
    D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUp);
    g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chunk of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it.
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
    // Initialize three vertices for rendering a triangle
    CUSTOMVERTEX g_Vertices[] =
    {
        {  0.0f, -1.1547f, 0.0f, 0xffffff00 }, // x, y, z, color
        { -1.0f,  0.5777f, 0.0f, 0xff00ff00 },
        {  1.0f,  0.5777f, 0.0f, 0xffff0000 },
    };

    // Create the vertex buffer. Here we are allocating enough memory
    // (from the default pool) to hold all our 3 custom vertices. We also
    // specify the FVF, so the vertex buffer knows what data it contains.
    if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX),
        D3DUSAGE_WRITEONLY,
        D3DFVF_CUSTOMVERTEX,
        D3DPOOL_MANAGED, &g_pVB)))
        return E_FAIL;

    // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
    // gain access to the vertices. This mechanism is required because the
    // vertex buffer may still be in use by the GPU. This can happen if the
    // CPU gets ahead of the GPU. The GPU could still be rendering the previous
    // frame.

    CUSTOMVERTEX* pVertices;
    if (FAILED(g_pVB->Lock(0, 0, (BYTE**)&pVertices, 0)))
        return E_FAIL;
    memcpy(pVertices, g_Vertices, 3 * sizeof(CUSTOMVERTEX));
    g_pVB->Unlock();

    return S_OK;
}

VOID UpdateTime()
{

    QueryPerformanceCounter(&qwTime);
    qwElapsedTime.QuadPart = qwTime.QuadPart - qwLastTime.QuadPart;
    qwLastTime.QuadPart = qwTime.QuadPart;
    qwElapsedAppTime.QuadPart = qwElapsedTime.QuadPart;
    qwAppTime.QuadPart += qwElapsedAppTime.QuadPart;

    // Store the current time values as floating point
    fTime = fSecsPerTick * ((FLOAT)(qwTime.QuadPart));
    fElapsedTime = fSecsPerTick * ((FLOAT)(qwElapsedTime.QuadPart));
    fAppTime = fSecsPerTick * ((FLOAT)(qwAppTime.QuadPart));
    fElapsedAppTime = fSecsPerTick * ((FLOAT)(qwElapsedAppTime.QuadPart));
}

//-----------------------------------------------------------------------------
// Name: Update()
// Desc: Updates the world for the next frame
//-----------------------------------------------------------------------------
VOID Update()
{
    //D3DXMATRIX matRotate;
    //D3DXMATRIX matWorld;
    //g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorld);
    //FLOAT fZRotate = -fElapsedTime * D3DX_PI * 0.5f;
    //D3DXMatrixRotationYawPitchRoll(&matRotate, 0.0f, 0.0f, fZRotate);
    //D3DXMatrixMultiply(&matWorld, &matWorld, &matRotate);
    //g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
    //Calculate background color
    const float blue = get_sin(fElapsedAppTime) + 1 * 255 * 0.5f;

    // Clear the backbuffer to a blue color
    g_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL,
        D3DCOLOR_XRGB(0, 0, static_cast<int>(blue)), 1.0f, 0L);

    // Begin the scene
    g_pd3dDevice->BeginScene();

    // Draw the triangles in the vertex buffer. This is broken into a few
    // steps. We are passing the vertices down a "stream", so first we need
    // to specify the source of that stream, which is our vertex buffer. Then
    // we need to let D3D know what vertex shader to use. Full, custom vertex
    // shaders are an advanced topic, but in many cases the vertex shader is
    // just the FVF, so that D3D knows what type of vertices we are dealing
    // with. Finally, we call DrawPrimitive() which does the actual rendering
    // of our geometry (in this case, just one triangle).
    g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    g_pd3dDevice->SetStreamSource(0, g_pVB, sizeof(CUSTOMVERTEX));
    g_pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

    //Draw
    //g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    

    if (!oglInited) return;
    static BOOL inpaint = FALSE;
    if (inpaint) return;
    inpaint = TRUE;

    oglScene.run();

    // ****************************************
    // FPS Counter
    // Will reimplement when I know how to draw text
    /*
    ++frameCount;

    DWORD newTime = GetTickCount();
    lastTime = newTime;

    if ((newTime - fpsTime) > 1000)
    {
        fpsTime = newTime;
        fps = (fps + frameCount) / 2;
        frameCount = 0;

        wchar_t  s[256];
        wsprintf(s, L"FPS = %d", fps);
        ::SetWindowText(hWnd, s);
    }
    */
    // ****************************************
    drawOffscreens();

    inpaint = FALSE;

    // End the scene
    g_pd3dDevice->EndScene();

    OGLDestroy();
}

// ***************************************************************************

void OGLCreate()
{
    // Size viewport
    RECT rc{ 0, 0, 640, 480 };      // Replace with SDL or DirectX function?
    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    OGLSize(width, height);

    createOffscreens();

    oglInited = TRUE;
}

//-----------------------------------------------------------------------------
// Name: main()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
void __cdecl main()
{
    // Initialize Direct3D
    if (FAILED(InitD3D()))
        return;

    // Initialize the vertex buffer
    InitVB();

    InitTime();

    // Seed random number table
    srand(GetTickCount());

    // Create sin/cos tables
    make_sin_cos_tables();

    OGLCreate();

    while (TRUE)
    {
        // What time is it?
        UpdateTime();
        // Update the world
        Update();
        // Render the scene
        Render();
        // Present the backbuffer contents to the display
        g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
    }
}