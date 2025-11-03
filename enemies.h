#ifndef ENEMIES_H
#define ENEMIES_H

#include "defines.h"
#include "point3d.h"
#include "grid.h"
#include "particle.h"
#include "camera.h"
#include "attractor.h"
#include "controls.h"
#include "mathutils.h"
#include "entity.h"
#include "entityline.h"
#include "player.h"


constexpr int numEnemyWanderer    = 100;
constexpr int numEnemyGrunt       = 200;
constexpr int numEnemySpinner     = 100;
constexpr int numEnemyTinySpinner = 100;
constexpr int numEnemyWeaver      = 200;
constexpr int numEnemySnake       = 50;
constexpr int numEnemyBlackHole   = 8;
constexpr int numEnemyRepulsor    = 4;
constexpr int numEnemyMayfly      = 400;
constexpr int numEnemyProton      = 200;


constexpr int NUM_ENEMIES = numEnemyWanderer + numEnemyGrunt + numEnemySpinner + numEnemyTinySpinner + numEnemyWeaver + numEnemySnake + numEnemyBlackHole + numEnemyRepulsor + numEnemyMayfly + numEnemyProton;
constexpr int NUM_LINES = 250;

class enemies
{
public:
    enemies();
    ~enemies();

    void run();
    void draw();

    entity* getUnusedEnemyOfType(const entity::EntityType& type);

    int getNumActiveEnemiesOfType(const entity::EntityType& type);

    entity* hitTestEnemiesAtPosition(const Point3d& point, float radius, bool includeSpawning=false);

    void disableAllEnemies();
    void disableAllLines();

    entityLine* getUnusedLine();
    void explodeEntity(entity& e);

    entity* mEnemies[NUM_ENEMIES];
    entityLine* mLines[NUM_LINES];

};

#endif // ENEMIES_H
