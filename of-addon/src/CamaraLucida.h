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

#pragma once

#include <iostream.h>
#include <algorithm>
#include "ofMain.h"
#include "cv.h"
#include "MSAOpenCL.h"


class CamaraLucida
{
public:
		
	CamaraLucida()
	{
		near = 0.08;
		far = 12.0;
		depth_xoff = 7;
		mesh_step = 1;
		_debug = false;
	};
	~CamaraLucida();
		
	
	void setup(const char* kinect_calibration_filename, 
			   const char* proj_calibration_filename,
			   uint16_t *raw_depth_pix, uint8_t *rgb_pix,
			   MSA::OpenCL* opencl = NULL);
	
	void setup(const char* kinect_calibration_filename, 
			   const char* proj_calibration_filename,
			   uint16_t *raw_depth_pix, uint8_t *rgb_pix,
			   ofFbo::Settings s, MSA::OpenCL* opencl = NULL);
	
	// updates FBO with off-screen texture created by render_texture()
	void update(uint16_t *raw_depth_pix, uint8_t *rgb_pix);
	
	// updates FBO with a passed custom texture (usable for debugging)
	void update(uint16_t *raw_depth_pix, uint8_t *rgb_pix,
				const ofTexture tex);
	
	void render();
	
	ofEvent<ofEventArgs> update_texture;
	ofEvent<ofEventArgs> render_texture;
	ofEvent<ofEventArgs> render_hud;
	
	void toggle_debug();
	
	
private:
	
	bool _debug;
	void _update(uint16_t *raw_depth_pix);
	
	// events
	
	ofEventArgs void_event_args;	
	void init_events();
	void dispose_events();
	
	void keyPressed(ofKeyEventArgs &args);
	void keyReleased(ofKeyEventArgs &args);
	void mouseDragged(ofMouseEventArgs &args);
	void mousePressed(ofMouseEventArgs &args);

	// gl
	
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
	void update_vbo();
	void dispose_vbo();
	
	ofVbo vbo;
	uint* ibo;
	float4* vbo_3d;
	ofColor* vbo_color;
	ofVec2f* vbo_texcoords;
	int vbo_length;
	int ibo_length;	
	
	
	// fbo
	
	ofFbo fbo;
	ofTexture texture;
	void init_fbo(ofFbo::Settings s);
	void update_fbo();
	void update_fbo(const ofTexture tex);
	
	// open cl
	
	// kernel mockup
	void update_vertex(int vbo_idx, float4* vbo_buff, uint* ibo_buff, 
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
	MSA::OpenCLBuffer	cl_buff_ibo;
	MSA::OpenCLBuffer	cl_buff_raw_depth;
	
	
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
		V_WORLD, V_PROJ, V_DEPTH, V_TYPE_LENGTH //RGB,
	};
	int view_type;
	
	
	// color utils
	
	void RGBtoHSV( float r, float g, float b, float *h, float *s, float *v );
	void HSVtoRGB( float h, float s, float v, float *r, float *g, float *b );
	
	
	// camara lucida
	
	
	ofVec3f proj_loc, proj_fwd, proj_up, proj_trg;
	
	void load_data(const char* kinect_calibration_filename, 
				   const char* proj_calibration_filename);
		
	void raw_depth_to_p3d(uint16_t raw_depth, int row, int col, float *vec3);
	void p3d_to_rgb(int x_d, int y_d, uint16_t raw_depth, float *rgb2d);
		
	float raw_depth_to_meters(uint16_t raw_depth);
	//float raw_depth_baseline(uint16_t raw_depht);
	//float fix_depth_geometry(uint16_t orig_depth, int x_d, int y_d);
	
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
	
	void init_z_lut();
	float z_lut[2048];
	
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
