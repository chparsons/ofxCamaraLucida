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
#include "ofxXmlSettings.h"
#include "cmlConfig.h"
#include "cmlEvents.h"
#include "cmlOpticalDevice.h"
#include "cmlDepthCamera.h"
#include "cmlCalibration.h"
#include "cmlMesh.h"
#include "cmlRenderer.h"

namespace cml
{
  class CamaraLucida
  {
    public:

      CamaraLucida( string cfg_path );

      ~CamaraLucida();

      void dispose();
      void render();
      void update( uint16_t *mm_depth_pix );

      void wireframe(bool v) { _wire = v; };
      bool wireframe() { return _wire; };

      void gpu(bool v) { _gpu = v; };
      bool gpu() { return _gpu; };

      void debug( bool val );
      bool debug();
      void toggle_debug();

      float tex_width();
      float tex_height();

      float depth_width()
      {
        return depth->config().width;
      };

      float depth_height()
      {
        return depth->config().height;
      };

      OpticalDevice::Config depth_config() 
      {
        return depth->config();
      };

      ofEvent<ofEventArgs>& render_texture;
      ofEvent<ofEventArgs>& render_3d;
      ofEvent<ofEventArgs>& render_2d;

      void log()
      {
        //mesh->log();
      };

    private:

      bool _gpu;

      void update_cpu( uint16_t *mm_depth_pix );
      void update_gpu( uint16_t *mm_depth_pix );

      void init( string cfg_path );

      ofTexture depth_ftex;

      cml::Events events;
      cml::DepthCamera* depth;
      OpticalDevice* proj;
      OpticalDevice* rgb;
      Renderer* renderer;
      Mesh* mesh;

      Config* config;
      ofxXmlSettings xml;
      string cfg_path;

      bool _wire;
      bool _debug;
      bool _render_help;
      bool pressed[512];

      struct Key
      {
        char debug;
        char help;
        char view_next;
        char view_prev;
        char scene_reset;
        char scene_zoom;
        char xoff_inc;
        char xoff_dec; 
      };
      Key key;

      void init_keys();

      void keyPressed(ofKeyEventArgs &args);
      void keyReleased(ofKeyEventArgs &args);

      void mousePressed(ofMouseEventArgs &args);
      void mouseDragged(ofMouseEventArgs &args);

      void render_screenlog();
      void render_help();

      void init_events();
      void dispose_events();
  };
};

