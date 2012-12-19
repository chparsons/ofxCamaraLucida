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
                OpticalDevice* rgb,
                bool *debug 
            );
            ~Renderer();

            void render( cml::Events *ev, Mesh *mesh );
            void dispose();
            
            void reset_scene();
            void next_view();
            void prev_view();

	        void mouseDragged(int x, int y, bool zoom);
	        void mousePressed(int x, int y);

        private:

            bool *debug;

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
            void render_proj_CS();
            void render_rgb_CS();
            void render_axis(float s);
            void render_screenlog();
            string _viewpoint_str();

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


