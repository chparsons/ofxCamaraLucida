#pragma once

#include "ofMain.h"

namespace cml
{
  struct Config
  {
    string kinect_intrinsics_file = "camara_lucida/kinect_intrinsics.yml";
    string projector_intrinsics_file = "camara_lucida/projector_intrinsics.yml";
    string projector_kinect_extrinsics_file = "camara_lucida/projector_kinect_extrinsics.yml";

    int mesh_res = 2;
    int tex_width = 1024;
    int tex_height = 768;
    int tex_nsamples = 1;

    char key_debug = 'd';
    char key_help = 'h';
    char key_scene_zoom = 'z';
    char key_scene_reset = 'x';
    char key_view_next = 'v';
  };
};

