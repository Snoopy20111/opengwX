#ifndef ENTITY_H
#define ENTITY_H

#include "mathutils.h"
#include "defines.h"
#include "point3d.h"
#include "vector.h"
#include "model.h"

class entity
{
public:

    typedef enum
    {
        ENTITY_TYPE_UNDEF=0,
        ENTITY_TYPE_PLAYER1,
        ENTITY_TYPE_PLAYER2,
        ENTITY_TYPE_PLAYER3,
        ENTITY_TYPE_PLAYER4,
        ENTITY_TYPE_PLAYER_MISSILE,
        ENTITY_TYPE_GRUNT,
        ENTITY_TYPE_WANDERER,
        ENTITY_TYPE_WEAVER,
        ENTITY_TYPE_SPINNER,
        ENTITY_TYPE_TINYSPINNER,
        ENTITY_TYPE_MAYFLY,
        ENTITY_TYPE_SNAKE,
        ENTITY_TYPE_SNAKE_SEGMENT,
        ENTITY_TYPE_BLACKHOLE,
        ENTITY_TYPE_REPULSOR,
        ENTITY_TYPE_REPULSOR_SHIELD,
        ENTITY_TYPE_PROTON,
        ENTITY_TYPE_LINE,
        ENTITY_NUM_TYPES // Must be last
    }EntityType;

    typedef enum
    {
        ENTITY_STATE_INACTIVE=0,
        ENTITY_STATE_SPAWN_TRANSITION,
        ENTITY_STATE_SPAWNING,
        ENTITY_STATE_RUN_TRANSITION,
        ENTITY_STATE_RUNNING,
        ENTITY_STATE_DESTROY_TRANSITION,
        ENTITY_STATE_DESTROYED,
        ENTITY_STATE_INDICATE_TRANSITION,
        ENTITY_STATE_INDICATING
    }EntityState;

    entity();
    entity(const entity& ent) = delete;
    entity(entity&&) = delete;
    entity& operator=(const entity&) = delete;
    entity& operator=(entity&&) = delete;
    virtual ~entity() {};

    static entity* createEntity(EntityType _entity); // Static class factory

    EntityType getType() const noexcept { return mType; }

    Point3d getPos() const noexcept { return mPos; }
    void setPos(const Point3d& pos) noexcept { mPos = pos; }

    Point3d getSpeed() const noexcept { return mSpeed; }
    void setSpeed(const Point3d& speed) noexcept  { mSpeed = speed; }

    Point3d getDrift() const noexcept { return mDrift; }
    void setDrift(const Point3d& drift) noexcept  { mDrift = drift; }

    float getAngle() const noexcept { return mAngle; }
    void setAngle(const float& angle) { mAngle = mathutils::wrapRadians(angle); }

    float getRotationRate() const noexcept { return mRotationRate; }
    void setRotationRate(const float& rate) noexcept  { mRotationRate = rate; }

    Point3d getScale() const noexcept { return mScale; }
    void setScale(const Point3d& scale) noexcept { mScale = scale; }
    void setScale(float scale) noexcept { mScale = scale; }

    vector::pen getPen() const noexcept { return mPen; }
    void setPen(const vector::pen& pen) noexcept { mPen = pen; }

    bool getEnabled() const noexcept { return mState != ENTITY_STATE_INACTIVE; }
    void setEnabled(const bool& enabled) noexcept { mState = (enabled) ? ENTITY_STATE_SPAWN_TRANSITION : ENTITY_STATE_INACTIVE; }

    EntityState getState() const noexcept { return mState; }
    void setState(const EntityState& state) noexcept { mState = state; }

    int getStateTimer() const noexcept { return mStateTimer; }
    void setStateTimer(int stateTimer) noexcept { mStateTimer = stateTimer; }

    model* getModel() noexcept { return &mModel; }

    const int getScoreValue() const noexcept { return mScoreValue; }

    const float getRadius() const noexcept { return mRadius; }

    const float getAggression() const noexcept { return mAggression; }

    void setEdgeBounce(BOOL bounce) noexcept { mEdgeBounce = bounce; }

    void setGridBound(BOOL gridBound) noexcept { mGridBound = gridBound; }

    virtual void runTransition();
    virtual void run();
    virtual void spawnTransition();
    virtual void spawn();
    virtual void destroyTransition();
    virtual void destroy();
    virtual void indicateTransition();
    virtual void indicating();

    virtual void hit(entity* aEntity);
    virtual entity* hitTest(const Point3d& pos, float radius);

    virtual void draw();

    virtual entity* getParent() noexcept { return this; }

    int getGenId() const noexcept { return mGenId; }
    void incGenId() noexcept { ++mGenId; }

protected:

    EntityType mType;
    Point3d mPos;
    Point3d mSpeed;
    Point3d mDrift;
    float mAngle;
    float mRotationRate;
    Point3d mScale;
    float mRadius;
    BOOL mEdgeBounce;
    BOOL mGridBound;
    vector::pen mPen;

    float mAggression;

    EntityState mState;

    int mSpawnTime;
    int mDestroyTime;
    int mIndicateTime;
    float mStateTimer;

    int mScoreValue;

    int mGenId;

    model mModel;
};

#endif // ENTITY_H
