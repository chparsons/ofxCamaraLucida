#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxCamaraLucida.h"
#include "cmlDepthmap_freenect.h"
#include <GLUT/glut.h>

class testApp : public ofBaseApp 
{
  public:

    void setup();
    void update();
    void draw();
    void exit();

    void keyPressed(int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);

    ofxKinect kinect;

    cml::CamaraLucida *cml;
    cml::Depthmap_freenect *depthmap;

    void render_texture(ofEventArgs &args);
    void render_3d(ofEventArgs &args);
    void render_2d(ofEventArgs &args);
};
