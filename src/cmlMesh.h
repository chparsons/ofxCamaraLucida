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

#include "ofxXmlSettings.h"
#include "ofVbo.h"
#include "ofVectorMath.h"
#include "ofColor.h"
#include "cmlMeshData.h"
#include "cmlCalibration.h"

namespace cml 
{
	class Mesh
	{
	public:
		
		Mesh(uint16_t *raw_depth_pix);
		~Mesh();
		
		virtual void init(cml::MeshData *meshdata,
				cml::Calibration *calib,
				ofxXmlSettings *xml);
		
		virtual void update();
		
		virtual void render();
		
        //TODO xxx_pts() dont need to be public,
        //could be protected, just to be overriden by mesh extensions
		virtual void init_pts();
		virtual void update_pts();
		virtual void dispose_pts();
		
		virtual float* pts0x();
		virtual int sizeof_pts();
		
		virtual float* normals0x();
		virtual int sizeof_normals();
		
		virtual ofVec3f coord_sys();
		
		/// utils
		
		float z_mts(uint16_t raw_depth);
		float z_mts(int x_depth, int y_depth);
		
		ofVec3f raw_depth_to_p3d(
				uint16_t raw_depth, 
				int x_depth, int y_depth);

		ofVec3f raw_depth_to_p3d(
				int x_depth, int y_depth);

		ofVec2f raw_depth_to_rgb(
				uint16_t raw_depth, 
				int x_depth, int y_depth);

		ofVec2f p3d_to_depth(
				const ofVec3f& p3d);
		
		/// getters
		
		uint16_t get_raw_depth_size();
		uint16_t get_base_depth_raw();
		float get_base_depth_mts();
		
		int get_step();
		int get_width();
		int get_height();
		int get_tex_width();
		int get_tex_height();
		int get_depth_width();
		int get_depth_height();
		
		/// normals
		
		ofTexture& get_normals_tex_ref();
		void render_normals();
		
		/// hue tex
		
		ofTexture& get_hue_tex_ref();
		void set_hue_lut(float depth_near = 0.8, 
				float depth_far = 5.0,
				float hue_near = 0.95, 
				float hue_far = 0.15,
				bool clamp = false);
		
		/// ui
		
		virtual void print();
		virtual string get_keyboard_help();
		virtual void keyPressed(ofKeyEventArgs &args);
		virtual void keyReleased(ofKeyEventArgs &args);
		
	protected:
		
		void *pts3d;
		void *normals;
		
		ofVbo vbo;
		int mesh_length;
		int ibo_length;	
		
		uint16_t *raw_depth_pix;
		float *_zlut;
		
		void unproject_depth(
				int x_depth, int y_depth, 
				float z, float& x, float& y);

		float raw_depth_to_meters(uint16_t raw_depth);
		
		int to_depth_idx(int mesh_idx, 
						 int &x_depth, int &y_depth);
		
		cml::Calibration *calib;
		ofxXmlSettings *xml;
		
		int depth_xoff;
		int key_depth_xoff_inc;
		int key_depth_xoff_dec;
		
	private:
		
		void init_data();
		void init_zlut();
		
		cml::MeshData *meshdata;
		
		uint* ibo;
		ofFloatColor* vbo_color;
		ofVec2f* vbo_texcoords;
		
		/// normals
		
		void init_normals();
		void print_normals();
		
		// gpu
		bool _normals_shader_enabled;
		ofShader normals_shader;
		ofFbo normals_fbo;
		ofFloatPixels normals_fpix;
		void update_normals_shader();
		
		// cpu
		ofTexture normals_cpu_tex;
		uint8_t *normals_cpu_pix;
		void update_normals_cpu();
		
		/// hue
		
		void init_hue_tex();
		ofFloatColor *hue_lut;
		ofTexture hue_tex;
		uint8_t *hue_pix;
		
	
		/// conversion utils
	

		// depth
		
		void get_depth_coord(int depth_idx, 
				int& x_depth, 
				int& y_depth);
		
		int get_depth_idx(int x_depth, int y_depth);
		
		void to_depth_coord(int x_mesh, int y_mesh,
				int& x_depth, int& y_depth);

		
		// mesh
		
		void get_mesh_coord(int mesh_idx, 
				int &x_mesh, int &y_mesh);

		int get_mesh_idx(int x_mesh, int y_mesh);
		
		void to_mesh_coord(int x_depth, int y_depth, 
				int& x_mesh, int& y_mesh);

		int to_mesh_idx(int depth_idx);
		
		///
		
		ofVec3f _coord_sys;
		
		uint16_t _raw_depth_size;
		uint16_t _base_depth_raw;
		float _base_depth_mts;
		
		static const float k1 = 0.1236;
		static const float k2 = 2842.5;
		static const float k3 = 1.1863;
		static const float k4 = 0.0370;
		
		/// ui
		
		bool pressed[512];
		void init_keys();
		
	};
};
