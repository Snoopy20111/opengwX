#ifndef ENTITYPLAYER1_H
#define ENTITYPLAYER1_H

#include "player.h"

class game;

class entityPlayer1 : public player
{
public:
    entityPlayer1(const game& gameRef);

    virtual void initPlayerForGame();
    virtual void spawnTransition();

    const game& mGame;
};

#endif // ENTITYPLAYER1_H
