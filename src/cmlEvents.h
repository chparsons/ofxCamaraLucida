#pragma once

#include "ofMain.h"

namespace cml
{
    struct Events
    {
        ofEvent<ofEventArgs> render_texture;
        ofEvent<ofEventArgs> render_3d;
        ofEvent<ofEventArgs> render_2d;
        ofEventArgs void_args;
    };
};
