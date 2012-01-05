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

#include "testApp.h"

void testApp::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetWindowPosition(0,0);
	
	if (!init_kinect())
		return;
	
	init_keys();
	
	debug_depth_texture = false;
	debug_hue_texture = true;
	
	tex_width = 1024;
	tex_height = 768;
	
	step = 2;
	
	mesh = new cml::Mesh_freenect(raw_depth_pix);
	mesh->enable_render(false);
	
	camluc.init(ofToDataPath("camara_lucida/kinect_calibration.yml"),
				ofToDataPath("camara_lucida/projector_calibration.yml"),
				ofToDataPath("camara_lucida/camara_lucida_config.xml"),
				mesh, tex_width, tex_height, 1 );
	
	ofAddListener(camluc.render_hud, this, &testApp::render_hud);
}

void testApp::update()
{
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update();
}

void testApp::draw()
{	
	camluc.render();
	
	ofVec3f v0, v1, v2;
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glBegin(GL_TRIANGLES);
	
	float v_thres = 0.1;
	float near_thres = 0.8;
	float far_thres = 4.0;
	
	for (int y = 0; y < kinect.height; y += step)
	{
		for (int x = 0; x < kinect.width; x += step)
		{
			v0.set( mesh->raw_depth_to_p3d(x, y) );
			if (v0.z < near_thres || v0.z > far_thres)
				continue;
			
			int x1 = x;
			int y1 = y + step;
			v1.set( mesh->raw_depth_to_p3d(x1, y1) );
			if (v1.z < near_thres || v1.z > far_thres)
				continue;
			
			int x2 = x + step;
			int y2 = y;
			v2.set( mesh->raw_depth_to_p3d(x2, y2) );
			if (v2.z < near_thres || v2.z > far_thres)
				continue;
			
			if (v0.distance(v1) > v_thres ||
				v0.distance(v2) > v_thres ||
				v1.distance(v2) > v_thres)
			{
				continue;
			}
			
			render_vertex(v0);
			render_vertex(v1);
			render_vertex(v2);
		}
	}	
	glEnd();
	
	//	glPushMatrix();
	//	glScalef(1, -1, 1);
	//	glTranslatef(-0.3, 0.3, 1);
	//	glColor3f(1, 1, 1);
	//	glutWireTeapot(0.1);
	//	glPopMatrix();
}

void testApp::render_vertex(const ofVec3f& v)
{
	float hue = ofMap(v.z, 0.8, 5.0, 0.95, 0.15, true);
	ofFloatColor c = ofFloatColor::fromHsb(hue * 255., 
										   255., 255., 255.);
	glColor3f(c.r/255., c.g/255., c.b/255.);
	glVertex3f(v.x, v.y, v.z);
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_hud, this, &testApp::render_hud);
	
	camluc.dispose();
	kinect.close();
	
	delete mesh;
	mesh = NULL;
}

void testApp::render_hud(ofEventArgs &args)
{
	if (pressed[key::keyboard_help])
	{
		int roff = 200;
		int toff = roff+50;
		
		glColor3f(1, 1, 1);
		ofDrawBitmapString(key::get_help()+"\n\n"+camluc.get_keyboard_help()+"\n"+mesh->get_keyboard_help(), 
						   toff, toff+12);	
		
		glColor4f(0, 0, 0, 0.1);
		ofRect(roff, roff, ofGetWidth()-roff*2, ofGetHeight()-roff*2);
		
		glColor3f(1, 1, 1);
	}				
	if (debug_depth_texture)
	{
		kinect.getDepthTextureReference().draw(0, 0, 200, 150);
	}
}

bool testApp::init_kinect()
{
	//kinect.enableCalibrationUpdate(false);
	kinect.enableDepthNearValueWhite(false);
	
	kinect.init(false, true, true);
	kinect.setVerbose(false);
	kinect.open();
	
	return update_kinect();
}

bool testApp::update_kinect()
{
	if (!kinect.isConnected())
		return false;
	
	kinect.update();
	raw_depth_pix = kinect.getRawDepthPixels();
	rgb_pix = kinect.getPixels();
	
	return true;
}

void testApp::keyPressed(int key)
{
	pressed[key] = true;
	
	if (key == key::fullscreen)
	{
		ofToggleFullscreen();
	}
	else if (key == key::projector)
	{
		if (ofGetWindowPositionX() == 0)
		{
			ofSetWindowPosition(1440,0);
		}
		else
		{
			ofSetWindowPosition(0,0);
		}
	}
	else if (key == key::debug_camaralucida)
	{
		camluc.toggle_debug();
	}
	else if (key == key::debug_depth_texture)
	{
		debug_depth_texture = !debug_depth_texture;
		//kinect.toggleCalibrationUpdate();
	}
	else if (key == key::debug_hue_texture)
	{
		debug_hue_texture = !debug_hue_texture;
	}
	else if (key == key::print_mesh)
	{
		mesh->print();
	}
}

void testApp::keyReleased(int key)
{
	pressed[key] = false;
}

void testApp::init_keys()
{
	for (int i = 0; i < 512; i++) 
		pressed[i] = false;
}

void testApp::mouseMoved(int x, int y )
{
	
}

void testApp::mouseDragged(int x, int y, int button)
{
	
}

void testApp::mousePressed(int x, int y, int button)
{
	
}

void testApp::mouseReleased(int x, int y, int button)
{
	
}

void testApp::resized(int w, int h)
{
	
}

