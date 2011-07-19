#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "CamaraLucida.h"

class app : public ofBaseApp 
{
public:
		
	CamaraLucida camluc;
		
	void update_texture(ofEventArgs &args);
	void render_texture(ofEventArgs &args);
	void render_hud(ofEventArgs &args);
		
	bool debug_depth_texture;
	
	
	//	kinect
	
	ofxKinect kinect;
	uint16_t *raw_depth_pix;
	uint8_t *rgb_pix;
	
	bool init_kinect();
	bool update_kinect();
	
	
	//	ui
	
	bool pressed[512];
	
	void init_keys();
	void update_keys();
	
	
	// app
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void resized(int w, int h);
	
	void debug();
	
};
