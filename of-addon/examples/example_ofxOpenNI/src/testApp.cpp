//	C‡mara Lœcida
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
	
	init_kinect();
	
	mesh = new cml::Mesh_openni(&depth_generator);
	
	camluc.init(ofToDataPath("camara_lucida/kinect_calibration.yml"),
				ofToDataPath("camara_lucida/projector_calibration.yml"),
				ofToDataPath("camara_lucida/camara_lucida_config.xml"),
				mesh, ofGetWidth(), ofGetHeight(), 1);
	
	ofAddListener(camluc.render_texture, this, &testApp::render_texture);
	ofAddListener(camluc.render_hud, this, &testApp::render_hud);
}

void testApp::update()
{
	update_kinect();
	camluc.update();
}

void testApp::draw()
{	
	depth.draw(0, 0, 400, 300);
	camluc.render();
}

void testApp::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.render_texture, this, &testApp::render_texture);
	ofRemoveListener(camluc.render_hud, this, &testApp::render_hud);
	
	camluc.dispose();
	
	delete mesh;
	mesh = NULL;
}

void testApp::render_hud(ofEventArgs &args)
{
	ofDrawBitmapString("press 'o' to debug camera as a texture \n 'd' to toggle camara lucida debug, then use 'v' to change viewpoint between camera and projector \n mousedrag to rotate, 'z'+mousedrag to zoom, 'x' to reset the debug transformations", 10, 10);
	
	if (debug_depth_texture)
	{
		depth.draw(0, 0, 400, 300);
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
	
	if (debug_depth_texture)
	{
		glColor3f(1, 1, 1);
		depth.draw(0, 0, w, h);
	}
	
	glColor3f(1, 1, 0);
	ofCircle(200, 200, 100);
}

void testApp::init_kinect()
{
	context.setupUsingXMLFile("");
	depth.setup(&context);	
	//user.setup(&context, &depth);
	
	depth_generator = depth.getXnDepthGenerator();
}

void testApp::update_kinect()
{
	context.update();
	depth.update();
	//user.update();
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
			break;
	}
	
	if (key == 'd')
	{
		camluc.toggle_debug();
	}
	if (key == 'f')
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