#include "app.h"

void app::setup()
{
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	init_keys();
	debug_depth_texture = false;
	
	if (!init_kinect())
		return;
	
	camluc.setup(ofToDataPath("kinect_calibration.yml").c_str(),
				 ofToDataPath("projector_calibration.yml").c_str(),
				 raw_depth_pix, rgb_pix);
	
	ofAddListener(camluc.update_texture, this, &app::update_texture);
	ofAddListener(camluc.render_texture, this, &app::render_texture);
}

void app::update()
{
	update_keys();
	
	if (!update_kinect())
		return;
	
	if (debug_depth_texture)
	{
		camluc.update(raw_depth_pix, rgb_pix,
					  kinect.getDepthTextureReference());
	}
	else 
	{
		camluc.update(raw_depth_pix, rgb_pix);
	}
}

void app::draw()
{	
	if (debug_depth_texture)
	{
		camluc.render(kinect.getDepthTextureReference());
	}
	else 
	{
		camluc.render();
	}
}

void app::exit()
{
	ofLog(OF_LOG_VERBOSE, "exit!");
	
	ofRemoveListener(camluc.update_texture, this, &app::update_texture);
	ofRemoveListener(camluc.render_texture, this, &app::render_texture);
	
	kinect.close();
}

void app::update_texture(ofEventArgs &args)
{
	
}

void app::render_texture(ofEventArgs &args)
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
}

bool app::init_kinect()
{
	kinect.update_calibration = debug_depth_texture;
	kinect.enableDepthNearValueWhite(false);
	
	kinect.init(false, true, true);
	kinect.setVerbose(false);
	kinect.open();
	//	ktilt = 0;
	//	kinect.setCameraTiltAngle(ktilt);
	
	return update_kinect();
}

bool app::update_kinect()
{
	if (!kinect.isConnected())
		return false;
	
	kinect.update();
	raw_depth_pix = kinect.getRawDepthPixels();
	rgb_pix = kinect.getPixels();
	
	return true;
}

void app::keyPressed(int key)
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
			kinect.update_calibration = debug_depth_texture;
			break;
	}
	
	if (key == 'd')
	{
		camluc.toggle_debug();
	}
}

void app::keyReleased(int key)
{
	pressed[key] = false;
}

void app::init_keys()
{
	for (int i = 0; i < 512; i++) 
		pressed[i] = false;
}

void app::update_keys()
{
	
}

void app::mouseMoved(int x, int y )
{
	
}

void app::mouseDragged(int x, int y, int button)
{
	
}

void app::mousePressed(int x, int y, int button)
{
	
}

void app::mouseReleased(int x, int y, int button)
{
	
}

void app::resized(int w, int h)
{
	
}

void app::debug()
{
	
}