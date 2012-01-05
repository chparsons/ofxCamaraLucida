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
	
	mesh = new cml::Mesh_freenect(raw_depth_pix);
												 
	camluc.init(ofToDataPath("camara_lucida/kinect_calibration.yml"),
				ofToDataPath("camara_lucida/projector_calibration.yml"),
				ofToDataPath("camara_lucida/camara_lucida_config.xml"),
				mesh, tex_width, tex_height, 1);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
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
	
	glScalef(1, -1, 1);
	glTranslatef(-0.3, 0.3, 1);
	glColor3f(1, 1, 1);
	glutWireTeapot(0.1);
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_texture, this, &testApp::render_texture);
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
		//kinect.getDepthTextureReference().draw(0, 0, 200, 150);
	}
}

void testApp::render_texture(ofEventArgs &args)
{
	glClearColor(0.5, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, tex_width, tex_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, tex_width, 0, tex_height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	if (debug_hue_texture)
	{
		mesh->debug_hue_texture(0, 0, tex_width, tex_height);
	}
	else if (debug_depth_texture)
	{
		glColor3f(1, 1, 1);
		kinect.getDepthTextureReference().draw(0, 0, tex_width, tex_height);
	}
	
	glColor3f(1, 1, 0);
	ofCircle(800, 200, 60);
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

void testApp::debug()
{
	
}