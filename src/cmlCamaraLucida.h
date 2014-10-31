#pragma once

#include "ofMain.h"
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

      CamaraLucida();
      CamaraLucida( cml::Config config );
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

      ofTexture& get_float_tex_ref(
          uint16_t *mm_depth_pix ) 
      {
        return depth->get_float_tex_ref(
            mm_depth_pix );
      };

      ofTexture& get_hue_tex_ref(
          uint16_t *mm_depth_pix ) 
      {
        return depth->get_hue_tex_ref(
            mm_depth_pix );
      };

      void log()
      {
        //mesh->log();
      };

      ofEvent<ofEventArgs>& render_texture;
      ofEvent<ofEventArgs>& render_3d;
      ofEvent<ofEventArgs>& render_2d; 

    private:

      bool _gpu;

      void update_cpu( uint16_t *mm_depth_pix );
      void update_gpu( uint16_t *mm_depth_pix );

      void init( cml::Config config );

      ofTexture depth_ftex;

      cml::Events events;
      cml::DepthCamera* depth;
      OpticalDevice* proj;
      OpticalDevice* rgb;
      Renderer* renderer;
      Mesh* mesh;

      Config config;

      bool _wire;
      bool _debug;
      bool _render_help;
      bool pressed[512];

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

