#include "bomb.h"
#include "game.h"
#include "enemies.h"

bomb::bomb(void)
{
    mNumRings = 20;

    mRings = new RING[mNumRings];
    if (mRings)
    {
        for (int i=0; i<mNumRings; i++)
        {
            mRings[i].timeToLive = 0;
        }
    }
}

bomb::~bomb(void)
{
    if (mRings)
    {
        for (int i=0; i<mNumRings; i++)
        {
            mRings[i].timeToLive = 0;
        }

        delete [] mRings;
    }
    mRings = NULL;
}

void bomb::run()
{
    for (int i=0; i<mNumRings; i++)
    {
        if (mRings[i].timeToLive > 0)
        {
            mRings[i].radius += mRings[i].speed;
            mRings[i].thickness += .03;
            --mRings[i].timeToLive;

            if (mRings[i].radius > 100)
            {
                mRings[i].timeToLive = 0;
            }

            // Push the grid out
            attractor::Attractor* att = game::mAttractors.getAttractor();
            if (att)
            {
                att->strength = 200;
                att->radius = mRings[i].radius;

                att->pos = mRings[i].pos;
                att->enabled = TRUE;
                att->attractsParticles = TRUE;
            }

            // Look for any enemies within the blast radius and destroy them
            for (int j=0; j<NUM_ENEMIES; j++)
            {
                if ((theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_INACTIVE)
                    && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_INDICATING)
                    && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_INDICATE_TRANSITION)
                    && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_DESTROY_TRANSITION)
                    && (theGame->mEnemies->mEnemies[j]->getState() != entity::ENTITY_STATE_DESTROYED))
                {
                    float distance = mathutils::calculate2dDistance(mRings[i].pos, theGame->mEnemies->mEnemies[j]->getPos());
                    if ((distance > mRings[i].radius-10) && (distance < mRings[i].radius))
                    {
                        // Destroy it
                        theGame->mEnemies->mEnemies[j]->hit(NULL);
                    }
                }
            }

        }
    }
}

void bomb::draw()
{
    for (int i=0; i<mNumRings; i++)
    {
        if (mRings[i].timeToLive > 0)
        {
            RING* ring = &mRings[i];

            // Fade out
            ring->pen.a -= ring->fadeStep;
            if (ring->pen.a <= 0)
            {
                ring->pen.a = 0;
            }
            else if (ring->pen.a > 1)
            {
                ring->pen.a = 1;
            }

            glLineWidth(ring->pen.lineRadius);
            glColor4f(ring->pen.r, ring->pen.g, ring->pen.b, ring->pen.a);
            glBegin(GL_LINES);

            for (float a=0; a<360; a+=.5)
            {
                float angle = mathutils::DegreesToRads(a);

                Point3d from(0, ring->radius, 0);
                from = mathutils::rotate2dPoint(from, angle);

                Point3d to(0, ring->radius + ring->thickness, 0);
                to = mathutils::rotate2dPoint(to, angle);

                from += ring->pos;
                to += ring->pos;

                glVertex3d(from.x, from.y, 0);
                glVertex3d(to.x, to.y, 0);
            }

            glEnd();

        }
    }
}

void bomb::startBomb(Point3d pos, float radius, float thickness, float speed, int timeToLive, vector::pen pen)
{
    RING* ring = NULL;
    for (int i=0; i<mNumRings; i++)
    {
        if (mRings[i].timeToLive <= 0)
        {
            ring = &mRings[i];
        }
    }

    if (ring)
    {
        ring->pos = pos;
        ring->radius = radius;
        ring->thickness = thickness;
        ring->speed = speed;
        ring->timeToLive = timeToLive;
        ring->fadeStep = 1.0f / ring->timeToLive;
        ring->pen = pen;

        Point3d angle(0,0,0);
        float spread = 2*PI;
        int num = 100;
        int timeToLive = ring->timeToLive;
        pen.lineRadius=5;
        pen.a = .3;
        game::mParticles.emitter(&pos, &angle, speed, spread, num, &pen, timeToLive, FALSE, FALSE, 1, TRUE);

    }
}

bool bomb::isBombing()
{
    for (int i=0; i<mNumRings; i++)
    {
        if (mRings[i].timeToLive > 0)
        {
            return true;
        }
    }
    return false;
}

