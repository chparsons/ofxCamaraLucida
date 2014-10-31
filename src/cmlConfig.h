#pragma once

#include "ofMain.h"

namespace cml
{
  struct Config
  {
    string kinect_calib_path = "camara_lucida/kinect_calibration.yml";
    string projector_calib_path = "camara_lucida/projector_calibration.yml";

    int mesh_step = 2;
    int tex_width = 1024;
    int tex_height = 768;
    int tex_nsamples = 1;

    char key_debug = 'd';
    char key_help = 'h';
    char key_scene_zoom = 'z';
    char key_scene_reset = 'x';
    char key_view_prev = 'c';
    char key_view_next = 'v';
  };
};

