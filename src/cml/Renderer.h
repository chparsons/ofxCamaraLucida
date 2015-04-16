#pragma once

#include "ofMain.h"
#include "cml/Config.h"
#include "cml/Events.h"
#include "cml/OpticalDevice.h"
#include "cml/DepthCamera.h"
#include "cml/Mesh.h"
#include "cml/shaders/render.h"

namespace cml
{
  class Renderer
  {
    public:

      Renderer( 
          cml::Config config,
          OpticalDevice* proj, 
          OpticalDevice* depth
          //OpticalDevice* rgb
          );
      ~Renderer();

      void render(
          cml::Events *ev, 
          Mesh *mesh,
          ofTexture& depth_ftex,
          bool gpu, 
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
      //OpticalDevice* rgb;

      ofShader shader;
      RenderShader render_shader;
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
      //void render_rgb_CS();
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
        V_DEPTH, V_PROJ, /*V_RGB,*/ V_LENGTH
      };
      int _viewpoint;
  };
};


