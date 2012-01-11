//	Camara Lucida
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

#include "cmlMesh.h"

namespace cml 
{
	class Mesh_freenect : public Mesh
	{
	public:
		
		Mesh_freenect(uint16_t *raw_depth_pix);
		Mesh_freenect();
		~Mesh_freenect();
		
		ofVec3f coord_sys();
		
		float z_mts(uint16_t raw_depth);
		float z_mts(int x, int y, bool tex_coords = false);
		
		float raw_depth_to_meters(uint16_t raw_depth);
		
		ofVec3f raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth);
		ofVec3f raw_depth_to_p3d(int x_depth, int y_depth);
		ofVec2f p3d_to_depth(const ofVec3f& p3d);
		ofVec2f raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth);
		
		uint16_t get_raw_depth_size();
		uint16_t get_base_depth_raw();
		float get_base_depth_mts();
		
		void set_hue_lut(float depth_near = 0.8, float depth_far = 5.0,
						 float hue_near = 0.95, float hue_far = 0.15,
						 bool clamp = false);
		
		void debug_hue_texture(int x, int y, int width, int height);
		void keyPressed(ofKeyEventArgs &args);
		
		void print();
		string get_keyboard_help();
		
	protected:
		
		// Mesh implementation...
		
		void init_pts();
		void dispose_pts();
		void update_pts();
		
		float* pts0x();
		int sizeof_pts();
		
		//
		
		uint16_t *raw_depth_pix;
		int depth_xoff;
		
		ofVec3f _coord_sys;
		
		int key_depth_xoff_inc;
		int key_depth_xoff_dec;
		
	private:
		
		ofVec3f *pts3d;
		
		//
		
		static const float k1 = 0.1236;
		static const float k2 = 2842.5;
		static const float k3 = 1.1863;
		static const float k4 = 0.0370;
		
		uint16_t _raw_depth_size;
		uint16_t _base_depth_raw;
		float _base_depth_mts;
		float *_zlut;
		
		ofFloatColor *hue_lut;
		ofTexture hue_tex;
		uint8_t *hue_px;
		
		void init_data();
		
	};
};