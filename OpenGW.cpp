#include "defines.h"
#include "scene.h"
#include "game.h"

#include "blur.h"
#include "sincos.h"

#include <SDL.h>
#include <GL/glu.h>

#include <cstdio>
#include <memory>

#define filterWidth 5
#define filterHeight 5
#define imageWidth 320
#define imageHeight 240

//declare image buffers
static const int blurBufferWidth = 500;
static const int blurBufferHeight = 250;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} ColorRGBA;

static ColorRGBA blurBuffer[blurBufferWidth][blurBufferHeight];

static SDL_Window* window;
static SDL_GLContext context;

static void OGLCreate();
static void OGLDestroy();
static void OGLSize(int cx, int cy);
static void OGLPaint();

std::unique_ptr<scene> oglScene;

static BOOL oglInited = FALSE;

//our OpenGL texture handles
static unsigned int texOffscreen;

static void createOffscreens();
static void drawOffscreens();

#define CONTEXT_PRIMARY 0
#define CONTEXT_GLOW    1

static int mWidth, mHeight;

static Uint32 lastTime;
static Uint32 fpsTime;
static int frameCount;
static int fps;

static bool handleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		switch(e.type) {
			case SDL_QUIT:
				printf("Quit\n");
				return false;
		}
	}

	return true;
}

static void run() {
	while (handleEvents()) {
		OGLPaint();
	}
}

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_JOYSTICK) < 0) {
		printf("SDL_Init failed: %s\n", SDL_GetError());
		return 0;
	}

	Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL;
	if (0) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}

	window = SDL_CreateWindow("opengw", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		800, 600, flags);

	if (window) {
		srand(SDL_GetTicks());
		make_sin_cos_tables();

		oglScene.reset(new scene);

		OGLCreate();

		run();

		OGLDestroy();

		SDL_DestroyWindow(window);
	}

	SDL_Quit();

	return 0;
}

static void OGLCreate()
{
	context = SDL_GL_CreateContext(window);

	if (context == NULL) {
		printf("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
	}

    OGLSize(800, 600);

    // Do stuff with the context here if needed...
    createOffscreens();

	if (SDL_GL_SetSwapInterval(1) == -1) {
		printf("SDL_GL_SetSwapInterval failed: %s\n", SDL_GetError());
	}

	if (SDL_GL_MakeCurrent(window, context) < 0) {
		printf("SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
	}

    oglInited = TRUE;
}

static void OGLDestroy()
{
    oglInited = FALSE;

	SDL_GL_MakeCurrent(nullptr, 0);
	SDL_GL_DeleteContext(context);
}

static void OGLSize(int cx, int cy)
{
	oglScene->size(cx, cy);
    mWidth = cx;
    mHeight = cy;
}

static void createOffscreens()
{
    // new array
    char* colorBits = new char[ blurBufferWidth * blurBufferHeight * 3 ];

    // texture creation..
    glGenTextures(1, &texOffscreen);
    glBindTexture(GL_TEXTURE_2D, texOffscreen);

    glTexImage2D(GL_TEXTURE_2D, 0, 3, blurBufferWidth, blurBufferHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, colorBits);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, colorBits);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // clean up
    delete[] colorBits;

    glBindTexture(GL_TEXTURE_2D, 0);
}

static void drawOffscreens()
{
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT,(int*)viewport);

    if (theGame->mSettings.mEnableGlow)
    {
        // Draw to the blur texture
        {
            glViewport(0, 0, blurBufferWidth, blurBufferHeight);

            oglScene->draw(scene::RENDERPASS_BLUR);

            // Transfer image to the blur texture
            glBindTexture(GL_TEXTURE_2D, texOffscreen);
            glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, blurBufferWidth, blurBufferHeight, 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    }

    // Draw the scene normally
    glViewport(viewport[0],viewport[1],viewport[2],viewport[3]);
    oglScene->draw(scene::RENDERPASS_PRIMARY);

    if (theGame->mSettings.mEnableGlow)
    {
        ////////////////////////////////////////////////
        // Do blur

        // Bind the blur texture and copy the screen bits to it
        glBindTexture(GL_TEXTURE_2D, texOffscreen);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer);

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
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, blurBufferWidth, blurBufferHeight, GL_RGB, GL_UNSIGNED_BYTE, blurBuffer);

        ////////////////////////////////////////////////
        // Draw the blur texture on top of the existing scene

	    // Glowy blending effect
	    glDisable(GL_DEPTH_TEST);
	    glEnable(GL_BLEND);
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

        glEnable( GL_TEXTURE_2D );
        glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

        if (game::mGameMode == game::GAMEMODE_ATTRACT || game::mGameMode == game::GAMEMODE_CREDITED)
            glColor4f(1, 1, 1, 1);
        else
            glColor4f(1, 1, 1, 1);

        // Draw it on the screen
        glBegin( GL_QUADS );
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
    }
}

static void OGLPaint()
{
    if (!oglInited) return;

    static BOOL inpaint = FALSE;
    if (inpaint) return;
    inpaint = TRUE;

    oglScene->run();

    // ****************************************

    ++frameCount;

    Uint32 newTime = SDL_GetTicks();
    lastTime = newTime;


    if ((newTime - fpsTime) > 1000)
    {
        fpsTime = newTime;
        fps = (fps + frameCount) / 2;
        frameCount = 0;

		char buf[32];
		snprintf(buf, sizeof(buf), "OpenGW SDL2 - FPS %d", fps);
		SDL_SetWindowTitle(window, buf);
    }

    // ****************************************

	SDL_GL_MakeCurrent(window, context);

	drawOffscreens();

	SDL_GL_SwapWindow(window);
	SDL_GL_MakeCurrent(nullptr, 0);

	inpaint = FALSE;
}
