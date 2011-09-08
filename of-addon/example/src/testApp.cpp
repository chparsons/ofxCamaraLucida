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
	
	init_keys();
	debug_depth_texture = false;
	
	if (!init_kinect())
		return;
	
	opencl.setupFromOpenGL();
	
	camluc.init(ofToDataPath("kinect_calibration.yml"),
				ofToDataPath("projector_calibration.yml"),
				ofToDataPath("camaralucida_config.xml"),
				raw_depth_pix, rgb_pix, 
				ofGetWidth(), ofGetHeight(), 1, 
				&opencl);				
				//NULL);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_hud, this, &testApp::render_hud);
}

void testApp::update()
{
	update_keys();
	
	if (!update_kinect())
		return;
	
	if (kinect.isFrameNew())
		camluc.update(raw_depth_pix, rgb_pix);
}

void testApp::draw()
{	
	camluc.render();
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_texture, this, &testApp::render_texture);
	ofRemoveListener(camluc.render_hud, this, &testApp::render_hud);
	
	kinect.close();
}

void testApp::render_hud(ofEventArgs &args)
{
	ofDrawBitmapString("press 'o' to debug camera as a texture \n 'd' to toggle camara lucida debug, then use 'v' to change viewpoint between camera and projector \n mousedrag to rotate for debug, 'z'+mousedrag to zoom, 'x' to reset the debug transforms \n and 'c'+keyup/down to change depth xoffset", 0, 10);
	
	if (debug_depth_texture)
	{
		kinect.getDepthTextureReference().draw(0, 0, 400, 300);
	}
}

void testApp::render_texture(ofEventArgs &args)
{
	glClearColor(0.5, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float w = 1024;
	float h = 768;
	
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, w, 0, h, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glColor3f(1, 1, 0);
	ofCircle(200, 200, 100);
	
	if (debug_depth_texture)
	{
		glColor3f(1, 1, 1);
		kinect.getDepthTextureReference().draw(0, 0, w, h);
	}
}

bool testApp::init_kinect()
{
	//kinect.enableCalibrationUpdate(false);
	kinect.enableDepthNearValueWhite(false);
	
	kinect.init(false, true, true);
	kinect.setVerbose(false);
	kinect.open();
	//	ktilt = 0;
	//	kinect.setCameraTiltAngle(ktilt);
	
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
	
	switch (key)
	{		
		case 'm':
			if (ofGetWindowPositionX() == 0)
			{
				ofSetWindowPosition(1440,0);
				ofSetFullscreen(true);
			}
			else
			{
				ofSetWindowPosition(0,0);
				ofSetFullscreen(false);
			}
			break;		
						
		case 'o':
			debug_depth_texture = !debug_depth_texture;
			//kinect.toggleCalibrationUpdate();
			break;
	}
	
	if (key == 'd')
	{
		camluc.toggle_debug();
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

void testApp::update_keys()
{
	
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