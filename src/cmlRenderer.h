/*
 * Camara Lucida
 * www.camara-lucida.com.ar
 *
 * Copyright (C) 2011  Christian Parsons
 * www.chparsons.com.ar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ofMain.h"
#include "cmlConfig.h"
#include "cmlEvents.h"
#include "cmlOpticalDevice.h"
#include "cmlMesh.h"

namespace cml
{
  class Renderer
  {
    public:

      Renderer( 
          cml::Config* config,
          OpticalDevice* proj, 
          OpticalDevice* depth, 
          OpticalDevice* rgb
          );
      ~Renderer();

      void render(
          cml::Events *ev, 
          Mesh *mesh,
          bool wireframe );

      void dispose();

      void reset_scene();
      void next_view();
      void prev_view();
      string get_viewpoint_info();

      void mouseDragged(int x,int y,bool zoom);
      void mousePressed(int x, int y);

      void debug( bool val ) { _debug = val; };

    private:

      bool _debug;

      OpticalDevice* proj;
      OpticalDevice* depth;
      OpticalDevice* rgb;

      ofShader shader;
      ofFbo fbo;

      // gl

      void gl_ortho();
      void gl_projection();
      void gl_viewpoint();

      // debug

      void render_proj_ppal_point();
      void render_world_CS();
      void render_depth_CS();
      void render_proj_CS();
      void render_rgb_CS();
      void render_axis(float s);

      // scene control

      void gl_scene_control();
      void init_gl_scene_control();

      ofVec2f pmouse; 
      ofVec3f rot_pivot;
      float tZ, rotX, rotY, rotZ;
      float tZini, rotXini, rotYini, rotZini;
      float tZ_delta, rot_delta;

      // ui

      enum ViewpointType
      {
        V_DEPTH, V_PROJ, V_RGB, V_LENGTH
      };
      int _viewpoint;
  };
};


