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
//		'c' + key up/down: change depht_xoffset
//		'z' + mouse_drag: zoom in/out
//		mouse_drag: rotate

#pragma once

#include <iostream.h>
#include <algorithm>
#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "cv.h"
#include "MSAOpenCL.h"

class CamaraLucida
{
public:
		
	CamaraLucida()
	{
		_debug = false;
		_inited = false;
		_not_init_alert = false;
	};
	~CamaraLucida();
	
	void init(string kinect_calibration_filename, 
			  string proj_calibration_filename,
			  string config_filename,
			  uint16_t *raw_depth_pix, uint8_t *rgb_pix,
			  int tex_width, int tex_height, int tex_num_samples, 
			  MSA::OpenCL* opencl = NULL);
	
	// updates FBO with off-screen texture created by render_texture()
	void update(uint16_t *raw_depth_pix, uint8_t *rgb_pix);
	
	void render();
	
	float z_mts(uint16_t raw_depth);
	float z_mts(uint16_t *raw_depth_pix, int x, int y, bool tex_coords = false);
	
	ofVec3f raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth);
	ofVec3f raw_depth_to_p3d(uint16_t *raw_depth_pix, int x_depth, int y_depth);
	ofVec2f p3d_to_depth(const ofVec3f& p3d);
	ofVec2f raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth);
	
	ofEvent<ofEventArgs> render_texture;
	ofEvent<ofEventArgs> render_hud;
	
	void toggle_debug();
	
	//void set_base_depth(uint16_t raw_depth);
	uint16_t get_base_depth_raw();
	float get_base_depth_mts();
	
	static const float k1 = 0.1236;
	static const float k2 = 2842.5;
	static const float k3 = 1.1863;
	static const float k4 = 0.0370;
	static const uint16_t raw_depth_size = 2048; //11bits
	
private:
	
	static const char key_view_type = 'v';
	static const char key_reset_view = 'x';
	static const char key_change_depth_xoff = 'c'; // + key up/down
	static const char key_zoom = 'z';
	
	uint16_t _base_depth_raw;
	float _base_depth_mts;
	
	bool _debug;
	bool _inited;
	bool _not_init_alert;
	bool inited();
	
	ofxXmlSettings config;
	
	// events
	
	ofEventArgs void_event_args;	
	void init_events();
	void dispose_events();
	
	void keyPressed(ofKeyEventArgs &args);
	void keyReleased(ofKeyEventArgs &args);
	void mouseDragged(ofMouseEventArgs &args);
	void mousePressed(ofMouseEventArgs &args);

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
	
	//	vbo
	
	int mesh_step;
	int mesh_w, mesh_h;
	
	void update_mesh(const uint16_t *raw_depth_pix);
	void render_mesh();
	
	void init_vbo();
	void dispose_vbo();
	
	ofVbo vbo;
	uint* ibo;
	float4* vbo_3d;
	ofColor* vbo_color;
	ofVec2f* vbo_texcoords;
	int vbo_length;
	int ibo_length;	
	
	
	// fbo
	
	// XXX should use ofFbo version after this commit
	// https://github.com/openframeworks/openFrameworks/commit/bbb55436d33734cf01da63f3385096d6956d257d#libs/openFrameworks/gl/ofFbo.cpp
	
	ofFbo fbo;
	void init_fbo(int tex_width, int tex_height, int tex_num_samples);
	
	// open cl
	
	// kernel mockup
	void update_vertex(int vbo_idx, float4* vbo_buff, 
					   const ushort* raw_depth_buff, 
					   const int vbo_length, const int mesh_step, 
					   const int mesh_w, const int mesh_h,
					   const int kinect_w, const int kinect_h,
					   const float cx_d, const float cy_d, 
					   const float fx_d, const float fy_d, 
					   const int xoffset);
	
	bool using_opencl;
	
	void init_cl(uint16_t *raw_depth_pix, MSA::OpenCL* opencl);
	void update_cl(uint16_t *raw_depth_pix);
	
	MSA::OpenCLKernel	*kernel_vertex_update;
	
	MSA::OpenCLBuffer	cl_buff_vbo_3d;
	MSA::OpenCLBuffer	cl_buff_raw_depth;
	//MSA::OpenCLBuffer	cl_buff_ibo;
	
	
	//	scene control
	
	void init_gl_scene_control();
	void reset_gl_scene_control();
	
	ofVec3f rot_pivot;
	float tZ, rotX, rotY, rotZ;
	float tZini, rotXini, rotYini, rotZini;
	float tZ_delta, rot_delta;
	
	
	//	ui
	
	void init_keys();
	void update_keys();
	
	ofVec2f pmouse;
	bool pressed[512];
	
	enum ViewpointType
	{
		V_DEPTH, V_PROJ, V_TYPE_LENGTH //RGB,
	};
	int view_type;
	
	
	
	// camara lucida
	
	
	ofVec3f proj_loc, proj_fwd, proj_up, proj_trg;
	
	void load_data(string kinect_calibration_filename, 
				   string proj_calibration_filename);
		
	float raw_depth_to_meters(uint16_t raw_depth);
	
	void convertKKopencv2opengl(CvMat* opencvKK, float width, float height, float near, float far, float *openglKK);
	void convertRTopencv2opengl(CvMat* opencvR, CvMat* opencvT, float *openglRT);
	
	float proj_RT[16];
	float proj_KK[16];
	float rgb_KK[16];
	float depth_KK[16];
	float drgb_RT[16];
	
	float fx_d, fy_d, cx_d, cy_d;
	float fx_p, fy_p, cx_p, cy_p;
	float fx_rgb, fy_rgb, cx_rgb, cy_rgb;
	
	int d_width, d_height;
	int p_width, p_height;
	int rgb_width, rgb_height;

	int depth_xoff;
	float near, far;
	
	void init_zlut();
	float _zlut[raw_depth_size];
	
	void printM(float* M, int rows, int cols, bool colmajor = true);
	void printM(CvMat* M, bool colmajor = true);
	
	CvMat* rgb_int;
	CvMat* depth_int;
	
	CvMat* drgb_R;
	CvMat* drgb_T;
//	ofxVec3f T_rgb;
	ofMatrix4x4 RT_rgb;
		
	CvMat* proj_int;
	CvMat* proj_R;
	CvMat* proj_T;
}; 
