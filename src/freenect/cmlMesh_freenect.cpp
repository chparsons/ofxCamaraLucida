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

#include "cmlMesh_freenect.h"

namespace cml 
{
	Mesh_freenect::Mesh_freenect(uint16_t *raw_depth_pix) 
	: Mesh()
	{
		this->raw_depth_pix = raw_depth_pix;
		_coord_sys = ofVec3f(-1., -1., 1.);
	}
	
	Mesh_freenect::~Mesh_freenect()
	{
		delete[] _zlut;
		delete[] hue_px;
		delete[] hue_lut;
		hue_tex.clear();
		raw_depth_pix = NULL;
	}
	
	ofVec3f Mesh_freenect::coord_sys()
	{
		return _coord_sys;
	}
	
	void Mesh_freenect::init_pts()
	{
		init_data();
		
		pts3d = new ofVec3f[vbo_length];
		normals = new ofVec3f[vbo_length];
		
		memset( pts3d, 0, vbo_length*sizeof(ofVec3f) );	
		memset( normals, 0, vbo_length*sizeof(ofVec3f) );
	}

	void Mesh_freenect::dispose_pts()
	{
		delete[] (ofVec3f*)pts3d;
		pts3d = NULL;
		
		delete[] (ofVec3f*)normals;
		normals = NULL;
	}

	void Mesh_freenect::update_pts()
	{
		for (int i = 0; i < vbo_length; i++)
		{
			int x2d, y2d;
			int depth_idx = get_raw_depth_idx(i, x2d, y2d);
			ushort raw_depth = raw_depth_pix[depth_idx];
			
			float z = _zlut[raw_depth];
			float x = (x2d + depth_xoff - calib->cx_d) * z / calib->fx_d;
			float y = (y2d - calib->cy_d) * z / calib->fy_d;
			
			((ofVec3f*)pts3d)[i].x = x;
			((ofVec3f*)pts3d)[i].y = y;
			((ofVec3f*)pts3d)[i].z = z;
			
			// calc normals? mmm...
		}
	}
	
	//// dynamic buffers mesh impl
	
	float* Mesh_freenect::pts0x()
	{
		return &((ofVec3f*)pts3d)[0].x;
	}

	int Mesh_freenect::sizeof_pts()
	{
		return sizeof(ofVec3f);
	}
	
	float* Mesh_freenect::normals0x()
	{
		return &((ofVec3f*)normals)[0].x;
	}
	
	int Mesh_freenect::sizeof_normals()
	{
		return sizeof(ofVec3f);
	}
	
	////
	
	void Mesh_freenect::init_data()
	{
		_raw_depth_size = 2048; //11bits
		_base_depth_raw = 1024.; //5mts~
		_base_depth_mts = k1 * tanf(( (float)_base_depth_raw / k2) + k3) - k4;
		
		depth_xoff = xml_config->getValue("depth_xoff", -8); 
		
		xml_config->pushTag("debug_keys");
			xml_config->pushTag("depth_xoff");
				key_depth_xoff_inc = xml_config->getValue("inc", "b")[0]; 
				key_depth_xoff_dec = xml_config->getValue("dec", "n")[0]; 
			xml_config->popTag();
		xml_config->popTag();		
		
		_zlut = new float[_raw_depth_size];
		for (int i = 0; i < _raw_depth_size; i++) 
		{
			_zlut[i] = raw_depth_to_meters(i);
		}
		
		hue_px = new uint8_t[ calib->depth_width * calib->depth_height * 3 ];
		hue_lut = new ofFloatColor[_raw_depth_size];
		set_hue_lut();
	}
	
	//http://openkinect.org/wiki/Imaging_Information
	//http://nicolas.burrus.name/index.php/Research/KinectCalibration

	float Mesh_freenect::raw_depth_to_meters(uint16_t raw_depth)
	{
		if (raw_depth < _base_depth_raw)
		{
			return k1 * tanf(( (float)raw_depth / k2) + k3) - k4; // calculate in meters
		}
		return _base_depth_mts;
	}

	uint16_t Mesh_freenect::get_raw_depth_size()
	{
		return _raw_depth_size;
	}
	
	uint16_t Mesh_freenect::get_base_depth_raw()
	{
		return _base_depth_raw;
	}

	float Mesh_freenect::get_base_depth_mts()
	{
		return _base_depth_mts;
	}

	float Mesh_freenect::z_mts(uint16_t raw_depth)
	{
		return _zlut[raw_depth];
	}

	float Mesh_freenect::z_mts(int x, int y, bool tex_coords)
	{
		if (tex_coords)
		{
			x = (int) ( (float)x / tex_width * calib->depth_width );
			y = (int) ( (float)y / tex_height * calib->depth_height );
		}
		
		int depth_idx = y * calib->depth_width + x;
		uint16_t raw_depth = raw_depth_pix[depth_idx];
		
		return _zlut[raw_depth];
	}

	// TODO return vecs by ref
	
	ofVec3f Mesh_freenect::raw_depth_to_p3d(int x_depth, int y_depth)
	{
		ofVec3f p3d;
		p3d.z = z_mts(x_depth, y_depth);
		p3d.x = (x_depth + depth_xoff - calib->cx_d) * p3d.z / calib->fx_d;
		p3d.y = (y_depth - calib->cy_d) * p3d.z / calib->fy_d;
		return p3d;
	}

	ofVec3f Mesh_freenect::raw_depth_to_p3d(uint16_t raw_depth, int x_depth, int y_depth)
	{
		ofVec3f p3d;
		p3d.z = _zlut[raw_depth];
		p3d.x = (x_depth + depth_xoff - calib->cx_d) * p3d.z / calib->fx_d;
		p3d.y = (y_depth - calib->cy_d) * p3d.z / calib->fy_d;
		return p3d;
	}

	ofVec2f Mesh_freenect::p3d_to_depth(const ofVec3f& p3d)
	{
		ofVec2f d2d;
		d2d.x = (p3d.x * calib->fx_d / p3d.z) + calib->cx_d - depth_xoff;
		d2d.y = (p3d.y * calib->fy_d / p3d.z) + calib->cy_d;
		return d2d;
	}

	ofVec2f Mesh_freenect::raw_depth_to_rgb(uint16_t raw_depth, int x_depth, int y_depth)
	{
		//	P3D' = R.P3D + T
		//	P2D_rgb.x = (P3D'.x * fx_rgb / P3D'.z) + cx_rgb
		//	P2D_rgb.y = (P3D'.y * fy_rgb / P3D'.z) + cy_rgb
		
		ofVec2f rgb2d;
		
		float xoff = -8;
		ofVec3f p3d = raw_depth_to_p3d(raw_depth, x_depth, y_depth);
		
		ofVec3f p3d_rgb = calib->RT_rgb * p3d;
		
		rgb2d.x = (p3d_rgb.x * calib->fx_rgb / p3d_rgb.z) + calib->cx_rgb;
		rgb2d.y = (p3d_rgb.y * calib->fy_rgb / p3d_rgb.z) + calib->cy_rgb;
		
		CLAMP(rgb2d.x, 0, calib->rgb_width-1);
		CLAMP(rgb2d.y, 0, calib->rgb_height-1);
		
		return rgb2d;
	}
	
	void Mesh_freenect::debug_hue_texture(int x, int y, int width, int height)
	{
		if ( ! hue_tex.isAllocated() )
		{
			hue_tex.allocate(tex_width, tex_height, GL_RGB);
		}
		
		for (int i = 0; i < vbo_length; i++)
		{
			int depth_idx = get_raw_depth_idx(i);
			ushort raw_depth = raw_depth_pix[depth_idx];
			
			ofFloatColor hue = hue_lut[raw_depth];
			
			hue_px[depth_idx * 3 + 0] = hue.r;
			hue_px[depth_idx * 3 + 1] = hue.g;
			hue_px[depth_idx * 3 + 2] = hue.b;
		}
		
		hue_tex.loadData(hue_px, 
						 calib->depth_width, calib->depth_height, 
						 GL_RGB);
		
		glColor3f(1, 1, 1);
		hue_tex.draw(x, y, width, height);
	}
	
	void Mesh_freenect::set_hue_lut(float depth_near, float depth_far,
									float hue_near, float hue_far, 
									bool clamp)
	{
		for (int i = 0; i < _raw_depth_size; i++) 
		{
			if (_zlut[i] < _base_depth_mts)
			{
				float hue = ofMap(_zlut[i], depth_near, depth_far, 
								  hue_near, hue_far, clamp);

				hue_lut[i] = ofFloatColor::fromHsb(hue * 255., 
												   255., 255., 255.);
			}
			else
			{
				hue_lut[i] = ofFloatColor(0, 0, 0);
			}
		}
	}

	
	//
	
	
	string Mesh_freenect::get_keyboard_help()
	{
		return Mesh::get_keyboard_help() + " \n mesh freenect keys for debug mode: \n "+string(1, key_depth_xoff_inc)+" increment depth_xoff \n "+string(1, key_depth_xoff_dec)+" decrement depth_xoff";
	}
	
	void Mesh_freenect::print()
	{
		cout << "### Mesh_freenect ConvertProjectiveToRealWorld" << endl;
		
		for (int i = 0; i < vbo_length; i++)
		{
			int x2d, y2d;
			int depth_idx = get_raw_depth_idx(i, x2d, y2d);
			ushort raw_depth = raw_depth_pix[depth_idx];
			
			float z = _zlut[raw_depth];
			float x = (x2d + depth_xoff - calib->cx_d) * z / calib->fx_d;
			float y = (y2d - calib->cy_d) * z / calib->fy_d;
			
			cout << "### \t 2d " << x2d << ", " << y2d << ", depth " << raw_depth << "\t 3d: " << x << ", " << y << ", " << z << endl;
		}
	}
	
	void Mesh_freenect::keyPressed(ofKeyEventArgs &args)
	{		
		Mesh::keyPressed(args);
		
		if (args.key == key_depth_xoff_inc)
		{
			depth_xoff++;
			ofLog(OF_LOG_VERBOSE, "cml::Mesh_freenect.depth_xoff = "+ofToString(depth_xoff));
		}
		else if (args.key == key_depth_xoff_dec)
		{
			depth_xoff--;
			ofLog(OF_LOG_VERBOSE, "cml::Mesh_freenect.depth_xoff = "+ofToString(depth_xoff));
		}
	}
};