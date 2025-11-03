#ifndef CONTROLS_H
#define CONTROLS_H

#include "point3d.h"

#include "SDL.h"

class controls
{
public:

    controls();
    ~controls();

    Point3d getLeftStick(int player);
    Point3d getRightStick(int player);
    bool getTriggerButton(int player);
    bool getStartButton(int player);
    bool getBackButton(int player);
    bool getPauseButton(int player);

private:

    // Keyboard
    /*
    Point3d readKeyboardLeftStick(int player);
    Point3d readKeyboardRightStick(int player);
    bool readKeyboardTrigger(int player);
    bool readKeyboardStart(int player);
    bool readKeyboardBack(int player);
    bool readKeyboardPause(int player);
    */

    // Xbox controller
    Point3d readXboxControllerLeftStick(int player);
    Point3d readXboxControllerRightStick(int player);
    bool readXboxControllerTrigger(int player);
    bool readXboxStart(int player);
    bool readXboxBack(int player);
    bool readXboxPause(int player);

    SDL_Joystick* mControllers[4];

    int mNumJoysticks;

};

#endif // CONTROLS_H

