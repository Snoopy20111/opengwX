#ifndef ENTITYGRUNT_H
#define ENTITYGRUNT_H

#include "entity.h"

class game;

class entityGrunt : public entity
{
public:
    entityGrunt(const game& gameRef);

    virtual void run();
    virtual void spawnTransition();

    float mAnimationIndex;

    const game& mGame;
};

#endif // ENTITYGRUNT_H
