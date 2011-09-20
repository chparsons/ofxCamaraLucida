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

#include "ofxXmlSettings.h"
#include "ofVbo.h"
#include "ofVectorMath.h"
#include "ofColor.h"
#include "cmlCalibration.h"

namespace cml 
{
	class Mesh
	{
	public:
		
		virtual ~Mesh();
		
		void init(ofxXmlSettings *xml_config,
				  cml::Calibration *calib,
				  int tex_width, int tex_height);
		
		void update();
		void render();
		
		virtual ofVec3f coord_sys() = 0;
		
		virtual void keyPressed(ofKeyEventArgs &args);
		virtual void keyReleased(ofKeyEventArgs &args);
		
	protected:
		
		// abstract stuff...
		// remember to call dispose_pts in derived classes destructors!
		
		virtual void init_pts() = 0;
		virtual void dispose_pts() = 0;
		virtual void update_pts() = 0;
		
		virtual float* pts0x() = 0;
		virtual int sizeof_pts() = 0;
		
		///
		
		ofxXmlSettings *xml_config;
		cml::Calibration *calib;
		
		int tex_width, tex_height;
		
		int mesh_step;
		int mesh_w, mesh_h;
		
		int vbo_length;
		int ibo_length;	
		
		bool pressed[512];
		void init_keys();

	private:
		
		ofVbo vbo;
		uint* ibo;
		ofFloatColor* vbo_color;
		ofVec2f* vbo_texcoords;
	};
};