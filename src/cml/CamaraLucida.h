#pragma once

#include "ofMain.h"
#include "cml/Config.h"
#include "cml/Events.h"
#include "cml/OpticalDevice.h"
#include "cml/DepthCamera.h"
#include "cml/CalibrationParser.h"
#include "cml/Mesh.h"
#include "cml/Renderer.h"

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
      void update( uint16_t *depth_pix_mm );

      void wireframe(bool v) { _wire = v; };
      bool wireframe() { return _wire; };

      void gpu(bool v) { _gpu = v; };
      bool gpu() { return _gpu; };

      void debug( bool val );
      bool debug();
      void toggle_debug();

      DepthCamera* depth_camera()
      {
        return depth;
      };

      OpticalDevice* projector()
      {
        return proj;
      };

      float tex_width()
      {
        return _tex_width; 
      };

      float tex_height()
      {
        return _tex_height; 
      };

      float depth_width()
      {
        return _depth_width;
      };

      float depth_height()
      {
        return _depth_height;
      }; 

      //ofTexture& get_hue_tex_ref( uint16_t *depth_pix_mm ) 
      //{
        //return depth->get_hue_tex_ref( depth_pix_mm );
      //};

      //void log()
      //{
        //mesh->log();
      //};

      ofEvent<ofEventArgs>& render_texture;
      ofEvent<ofEventArgs>& render_3d;
      ofEvent<ofEventArgs>& render_2d; 

    private:

      bool _gpu;

      void update_cpu( uint16_t *depth_pix_mm );
      void update_gpu( uint16_t *depth_pix_mm );

      void init( cml::Config config );

      ofTexture* depth_ftex_mm;
      float _tex_width, _tex_height;
      float _depth_width, _depth_height;

      cml::Events events;
      DepthCamera* depth;
      OpticalDevice* proj;
      //OpticalDevice* rgb;
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

