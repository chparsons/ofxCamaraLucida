//	Cámara Lúcida
//	www.camara-lucida.com.ar
//
//	Copyright (C) 2011  Christian Parsons
//	www.chparsons.com.ar
//
//	This program is free software: you can redistribute it and/or modify
//	it under the terms of the GNU General Public License as published by
//	the Free Software Foundation, either version 3 of the License, or
//	(at your option) any later version.
//
//	This program is distributed in the hope that it will be useful,
//	but WITHOUT ANY WARRANTY; without even the implied warranty of
//	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	GNU General Public License for more details.
//
//	You should have received a copy of the GNU General Public License
//	along with this program.  If not, see <http://www.gnu.org/licenses/>.


//	reserved user input on debug mode (toggle_debug)
//		'x' reset view
//		'v' switch view projector, depth cam, world
//		'z' + mouse_drag: zoom in/out
//		mouse_drag: rotate


#pragma once

#include <iostream.h>
#include <algorithm>
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "cv.h"
#include "cmlCalibration.h"
#include "cmlMesh.h"

namespace cml 
{
	class CamaraLucida
	{
	public:
			
		CamaraLucida()
		{
			_debug = false;
			_inited = false;
			_not_init_alert = false;
		};
		~CamaraLucida(){};
		
		void init(string kinect_calibration_filename,
				  string proj_calibration_filename,
				  string xml_config_filename,
				  cml::Mesh *mesh,
				  int tex_width, int tex_height, 
				  int tex_num_samples);
		
		void dispose();
		
		void update();
		void render();
		
		ofEvent<ofEventArgs> render_texture;
		ofEvent<ofEventArgs> render_hud;
		
		void toggle_debug();
		
	private:
		
		cml::Mesh *mesh;
		ofxXmlSettings xml_config;
		
		static const char key_view_type = 'v';
		static const char key_reset_view = 'x';
		static const char key_zoom = 'z';
		
		bool _debug;
		bool _inited;
		bool _not_init_alert;
		bool inited();
		
		
		// camara lucida
		
		
		ofVec3f proj_loc, proj_fwd, proj_up, proj_trg;
		ofVec3f rgb_loc, rgb_fwd, rgb_up, rgb_trg;
		
		void load_data(string kinect_calibration_filename, 
					   string proj_calibration_filename);
		
		void convertKKopencv2opengl(CvMat* opencvKK, float width, float height, float near, float far, float *openglKK);
		void convertRTopencv2opengl(CvMat* opencvR, CvMat* opencvT, float *openglRT);
		
		cml::Calibration calib;
		
		float proj_KK[16];
		float proj_RT[16];
		
		float depth_KK[16];
		float rgb_KK[16];
		float rgb_RT[16];
		
		void printM(float* M, int rows, int cols, bool colmajor = true);
		void printM(CvMat* M, bool colmajor = true);
		
		//depth is @ origin
		CvMat* depth_int;
		
		CvMat* rgb_int;
		CvMat* rgb_R;
		CvMat* rgb_T;
		
		CvMat* proj_int;
		CvMat* proj_R;
		CvMat* proj_T;
		
		
		// gl
		
		void gl_ortho();
		void gl_projection();
		void gl_viewpoint();
		
		// gl debug
		
		void gl_scene_control();
		void render_ppal_point();
		void render_world_CS();
		void render_proj_CS();
		void render_rgb_CS();
		void render_axis(float s = 0.1);
		void render_screenlog();
		string view_type_str();
		
		
		// fbo
		
		// XXX should use ofFbo version after this commit
		// https://github.com/openframeworks/openFrameworks/commit/bbb55436d33734cf01da63f3385096d6956d257d#libs/openFrameworks/gl/ofFbo.cpp
		
		ofFbo fbo;
		void init_fbo(int tex_width, int tex_height, int tex_num_samples);
		
		
		//	scene control
		
		void init_gl_scene_control();
		void reset_gl_scene_control();
		
		ofVec3f rot_pivot;
		float tZ, rotX, rotY, rotZ;
		float tZini, rotXini, rotYini, rotZini;
		float tZ_delta, rot_delta;
		
		
		// ui
		
		ofEventArgs void_event_args;	
		
		void init_keys();
		void init_events();
		void dispose_events();
		
		void keyPressed(ofKeyEventArgs &args);
		void keyReleased(ofKeyEventArgs &args);
		void mouseDragged(ofMouseEventArgs &args);
		void mousePressed(ofMouseEventArgs &args);
		
		bool pressed[512];
		ofVec2f pmouse;
		
		enum ViewpointType
		{
			V_DEPTH, V_PROJ, V_RGB, V_TYPE_LENGTH
		};
		int view_type;
	}; 
};