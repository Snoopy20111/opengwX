#include "particle.h"
#include "game.h"
#include "entityblackhole.h"
#include "enemies.h"

#include <atomic>
#include <mutex>
#include <cstdio>

#define VIRTUAL_SCREEN_WIDTH 800 // FIX ME
#define VIRTUAL_SCREEN_HEIGHT 600 // FIX ME

#define _DISTANCE 320
#define _MIN_DISTANCE .001
#define _MAX_DISTANCE 800

static SDL_Thread* mRunThread = NULL;
static std::atomic_bool mRunFlag { false };

particle::PARTICLE* particle::mParticles = NULL;
int particle::mNumParticles = 0;
int particle::mIndex = 0;

static std::mutex m;

static int runThread(void *ptr)
{
    while(1)
    {
        while (!mRunFlag)
        {
            SDL_Delay(1);
        };
        mRunFlag = false;

        std::unique_lock<std::mutex> lock(m);

        int i;
        if (particle::mParticles)
        {
            for (i=0; i<particle::mNumParticles; i++)
            {
                if (particle::mParticles[i].timeToLive > 0)
                {
                    // This particle is active

                    particle::PARTICLE* particle = &particle::mParticles[i];

                    if (--particle->timeToLive < 0)
                    {
                        // This particle died
                        particle->timeToLive = 0;
                    }
                    else
                    {
                        // Evaluate against attractors
                        if (particle->gravity)
                        {
                            Point3d speed = game::mAttractors.evaluateParticle(particle);
                            particle->speedX += speed.x;
                            particle->speedY += speed.y;
                        }

                        // Evaluate against black holes
                        for (int i=0; i<NUM_ENEMIES; i++)
                        {
                            if ((theGame->mEnemies->mEnemies[i]->getType() == entity::ENTITY_TYPE_BLACKHOLE) && (theGame->mEnemies->mEnemies[i]->getState() == entity::ENTITY_STATE_RUNNING))
                            {
                                entityBlackHole* blackHole = static_cast<entityBlackHole*>(theGame->mEnemies->mEnemies[i]);
                                if (blackHole->mActivated)
                                {
                                    if (mathutils::calculate2dDistance(particle->posStream[0], blackHole->getPos()) < blackHole->getRadius()*1.01)
                                    {
                                        // kill this particle
                                        particle->timeToLive *= .7;
                                        continue;
                                    }
                                }
                            }
                        }

                        // Add drag
                        particle->speedX *= particle->drag;
                        particle->speedY *= particle->drag;

                        Point3d speedClamp(particle->speedX, particle->speedY, 0);
                        speedClamp = mathutils::clamp2dVector(speedClamp, 2);
                        particle->speedX = speedClamp.x;
                        particle->speedY = speedClamp.y;

                        // Move the particle
                        particle->posStream[0].x += particle->speedX;
                        particle->posStream[0].y += particle->speedY;
                        particle->posStream[0].z = 0;

                        if (particle->gridBound)
                        {
                            // Keep it within the grid

						    Point3d hitPoint;
                            Point3d speed(particle->speedX, particle->speedY);
						    if (game::mGrid.hitTest(particle->posStream[0], 0, &hitPoint, &speed))
						    {
                                particle->hitGrid = true;
							    particle->posStream[0] = hitPoint;

                                particle->speedX = speed.x;
                                particle->speedY = speed.y;
						    }

                        }

                        // Shift the position stream
                        for (int p=NUM_POS_STREAM_ITEMS-2; p >= 0; p--)
                        {
                            particle->posStream[p+1] = particle->posStream[p];
                        }

                    }
                }
            }
        }

    }

    return 0;
}

particle::particle()
{
    mNumParticles = 3000; // PERFORMANCE: The larger this number is, the larger the performance hit!
    mIndex = 0;

    mParticles = new PARTICLE[mNumParticles];
    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            mParticles[i].timeToLive = 0;
        }
    }

    // Thread stuff
    mRunThread = SDL_CreateThread(runThread, "runThread", NULL);
    if (!mRunThread)
    {
#ifdef USE_SDL
		printf("Couldn't create particle run thread: %s\n", SDL_GetError());
#else
        OutputDebugString(L"Couldn't create particle run thread\n");
#endif
    }
}

particle::~particle()
{
    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            mParticles[i].timeToLive = 0;
        }

        delete [] mParticles;
    }
    mParticles = NULL;
}

void particle::emitter(Point3d* position, Point3d* angle, float speed, float spread, int num, vector::pen* color, int timeToLive,
                       BOOL gravity, BOOL gridBound, float drag, BOOL glowPass)
{

    // Emit a number of random thrust particles from the nozzle
    for (int p=0; p<num; p++)
    {
        Point3d speedVertex, speedVector;

        speedVertex.x = 0;
        speedVertex.y = speed * mathutils::frandFrom0To1();
        speedVertex.z = 0;

        matrix mat;
        mat.Identity();
        mat.Rotate(0,
                   0,
                   angle->y + ((mathutils::frandFrom0To1()*spread)-spread/2));

        mat.TransformVertex(speedVertex, &speedVector);

        assignParticle(position,
                       speedVector.x, speedVector.y, speedVector.z,
                       timeToLive, color, gravity, gridBound, drag, glowPass);
    }
}

void particle::assignParticle(Point3d* position,
                              float aSpeedX, float aSpeedY, float aSpeedZ,
                              int aTime, vector::pen* aColor, BOOL gravity, BOOL gridBound, float drag, BOOL glowPass)
{
    PARTICLE* particle = &mParticles[mIndex++];
    if (mIndex >= mNumParticles) mIndex = 0;

    if (particle)
    {
        Point3d pos = *position;

        if (gridBound)
        {
			Point3d hitPoint;
			if (game::mGrid.hitTest(pos, 0, &hitPoint))
			{
				pos = hitPoint;
			}
        }

        for (int p=0; p<NUM_POS_STREAM_ITEMS; p++)
        {
            particle->posStream[p] = pos;
        }

        particle->speedX = aSpeedX;
        particle->speedY = aSpeedY;
        particle->speedZ = aSpeedZ;
        particle->color = *aColor;
        particle->timeToLive = aTime * mathutils::frandFrom0To1();
        particle->fadeStep = particle->timeToLive ? 1.0 / particle->timeToLive : 1.0f;
        particle->gravity = gravity;
        particle->gridBound = gridBound;
        particle->drag = drag;
        particle->glowPass = glowPass;
        particle->hitGrid = false;
    }
}

void particle::draw()
{
    std::unique_lock<std::mutex> lock(m);
    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            if (mParticles[i].timeToLive > 0)
            {
                // This particle is active
                PARTICLE* particle = &mParticles[i];

                float speedNormal = mathutils::calculate2dDistance(Point3d(0,0,0), Point3d(particle->speedX, particle->speedY, particle->speedZ));
                float a = particle->color.a * (speedNormal * 0.8f);

                if (a < 0.05f)
                {
                    // This particle died
                    particle->timeToLive = 0;
                    continue;
                }

                float width = speedNormal * 8.0f;
                if (width > 4.0f) width = 4.0f;
                else if (width < 2.0f) width = 2.0f;

                if (scene::mPass == scene::RENDERPASS_BLUR)
                {
                    width *= 4.0f;
                }

                glLineWidth(width);

                // This is SO inefficient
                // I really should be doing all the particles in one GL_LINES block but the
                // width of the lines needs to change per-particle, and you can't do a call to
                // glLineWidth() inside a GL_LINES block :-(

                glBegin(GL_LINES);

				float aa = (a > 1.0f) ? 1.0f : a;

                for (int p=0; (p < NUM_POS_STREAM_ITEMS - 1) && (aa > 0.0f); p++)
                {
                    //glColor4f(particle->color.r, particle->color.g, particle->color.b, aa); // RGBA

                    const Point3d& from = particle->posStream[p];
                    const Point3d& to = particle->posStream[p + 1];

                    glColor4f(particle->color.r, particle->color.g, particle->color.b, aa); // RGBA
                    glVertex3d(from.x, from.y, 0);
                    aa -= 0.1f;

                    glColor4f(particle->color.r, particle->color.g, particle->color.b, aa); // RGBA

                    if ((from.x == to.x) && (from.y == to.y))
					{
						glVertex3d(to.x + 0.1f, to.y + 0.1f, 0);
					} else {
						glVertex3d(to.x, to.y, 0);
					}

                    aa -= 0.1f;
                }

        	    glEnd();
            }
        }
    }
}

void particle::run()
{
    mRunFlag = true;
}

void particle::killAll()
{
    std::unique_lock<std::mutex> lock(m);

    if (mParticles)
    {
        for (int i=0; i<mNumParticles; i++)
        {
            mParticles[i].timeToLive = 0;
        }
    }
}
