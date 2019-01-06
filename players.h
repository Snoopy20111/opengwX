#ifndef PLAYERS_H
#define PLAYERS_H

#include "player.h"

class players
{
public:
    players();
    ~players();

    void run();
    void draw();

    player* getPlayerClosestToPosition(const Point3d& point);
    player* getRandomActivePlayer();

    static player* mPlayer1;
    static player* mPlayer2;
    static player* mPlayer3;
    static player* mPlayer4;
};

#endif // PLAYERS_H
