#include "dac_algorithms/wired_fight_pad_fg.hpp"

namespace DACAlgorithms {
namespace WiredFightPadProFG {


uint8_t fgHatFromDpadValues(bool dLeft, bool dRight, bool dUp, bool dDown) {
    if (dUp && !dLeft && !dRight) return 0;
    else if (dUp && dRight) return 1;
    else if (dRight && !dDown) return 2;
    else if (dRight && dDown) return 3;
    else if (dDown && !dLeft) return 4;
    else if (dDown && dLeft) return 5;
    else if (dLeft && !dUp) return 6;
    else if (dLeft && dUp) return 7;
    else return 15;
}


//bool left_dead = false;
//bool right_dead = false;
int latest_x = 0; // Value of latest direction pressed, reset to 0 when both direction keys are released
int latest_y = 0;
bool old_left = false;
bool old_right = false;
bool old_up = false;
bool old_down = false;

void actuateWFPPFGReport(GpioToButtonSets::F1::ButtonSet buttonSet) {
    bool buttonSetUp = buttonSet.cUp || buttonSet.cLeft || buttonSet.a;

    const bool left = buttonSet.left;
    const bool right = buttonSet.right;
    const bool up = buttonSetUp;
    const bool down = buttonSet.down;

    const uint8_t min_val = 0;
    const uint8_t max_val = 255;
    const uint8_t mid_val = 128;

    uint8_t out_x = mid_val;
    {
        if (left && !old_left) {
            // Pressed
            latest_x = -1;
            out_x = min_val;
        }
        if (right && !old_right) {
            // Pressed
            latest_x = 1;
            out_x = max_val;
        }
        if (!left && !right) {
            latest_x = 0;
        }
        if (left && latest_x == -1) out_x = min_val;
        if (right && latest_x == 1) out_x = max_val;
    }

    uint8_t out_y = mid_val;
    {
        if (up && !old_up) {
            // Pressed
            latest_y = -1;
            out_y = min_val;
        }
        if (down && !old_down) {
            // Pressed
            latest_y = 1;
            out_y = max_val;
        }
        if (!up && !down) {
            latest_y = 0;
        }
        if (up && latest_y == -1) out_y = min_val;
        if (down && latest_y == 1) out_y = max_val;
    }

    bool dLeft = 0;
    bool dRight = 0;
    bool dUp = 0;
    bool dDown = 0;
    //bool dLeft = buttonSet.left && buttonSet.ms;
    //bool dRight = buttonSet.right && buttonSet.ms;
    //bool dUp = buttonSet.up && buttonSet.ms;
    //bool dDown = buttonSet.down && buttonSet.ms;

    USBConfigurations::WiredFightPadPro::hidReport = {
        .y=buttonSet.y,
        .b=buttonSet.r,
        .a=buttonSet.b,
        .x=buttonSet.x,
        .l=buttonSet.z,
        .r=buttonSet.up,
        .zl=buttonSet.ls,
        .zr=buttonSet.ms,
        .minus=buttonSet.mx||buttonSet.startL,
        .plus=buttonSet.startR,
        .pad=0,
        .home=buttonSet.start,
        //.home=buttonSet.start && !(buttonSet.ms), // Start -> Home
        .photo=buttonSet.ms && buttonSet.start, // MS -> Photo
        .pad2=0,
        .hat=fgHatFromDpadValues(dLeft, dRight, dUp, dDown),
        //.xStick=(uint8_t)(!(left ^ right) ? 128 : left ? min_val : max_val), // Neutral SOCD
        //.yStick=(uint8_t)(!(up ^ down) ? 128 : up ? min_val : max_val), // Neutral SOCD + reversed axis
        .xStick=out_x,
        .yStick=out_y,
        //.cxStick=(uint8_t)(!(buttonSet.cLeft ^ buttonSet.cRight) ? 128 : buttonSet.cLeft ? 48 : 208), // Neutral SOCD
        //.cyStick=(uint8_t)(!(buttonSet.cUp ^ buttonSet.cDown) ? 128 : buttonSet.cUp ? 48 : 208), // Neutral SOCD + reversed axis
        .cxStick=mid_val,
        .cyStick=mid_val,
        .pad3=0
    };

    old_left = left;
    old_right = right;
    old_up = up;
    old_down = down;
}

}
}


