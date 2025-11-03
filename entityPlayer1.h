#ifndef ENTITYPLAYER1_H
#define ENTITYPLAYER1_H

#include "player.h"

class entityPlayer1 : public player
{
public:
    entityPlayer1();
    ~entityPlayer1() {};

    //matrix() noexcept { Identity(); }
    //matrix(const matrix& mat) noexcept { Copy(mat); }
    //matrix(matrix&&) = delete;
    //matrix& operator=(const matrix&) = delete;
    //matrix& operator=(matrix&&) = delete;
    //~matrix() {}

    virtual void initPlayerForGame();
    virtual void spawnTransition();

};

#endif // ENTITYPLAYER1_H
