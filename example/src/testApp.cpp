#include "testApp.h"

void testApp::setup() 
{
    ofSetLogLevel(OF_LOG_VERBOSE);
    ofSetWindowPosition(0,0);
    ofSetFrameRate(60);
    ofBackground(50);

    kinect.setDepthMode( FREENECT_DEPTH_11BIT );
    //kinect.setRegistration(false);
    //kinect.setDepthClipping(500, 4000); //mm (50cm - 4mt)
    kinect.enableDepthNearValueWhite(false);
    // ir:false, rgb:false, texture:true
    kinect.init(false, false, true);
    kinect.open();

    string config = ofToDataPath("camara_lucida/config.xml");

    _d2m = new cml::Depth2Mesh_freenect();
    _cml = new cml::CamaraLucida( config, _d2m );

    ofAddListener(_cml->render_texture, this, &testApp::render_texture);
    ofAddListener(_cml->render_3d, this, &testApp::render_3d);
    ofAddListener(_cml->render_2d, this, &testApp::render_2d);

}

void testApp::update() 
{	
    if ( !kinect.isConnected() )
        return;

    kinect.update();

    if ( kinect.isFrameNew() )
        return;

    _d2m->update( kinect.getRawDepthPixels() );
}

void testApp::draw() 
{    
    _cml->render(); 
}

void testApp::render_texture(ofEventArgs &args)
{
    glClearColor(0.5, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float w = _cml->tex_width();
    float h = _cml->tex_height();

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1, 1, 1);

    kinect.drawDepth(0, 0, w, h);

    glColor3f(1, 1, 0);
    ofCircle(800, 200, 60);
}

void testApp::render_3d(ofEventArgs &args)
{
    glTranslatef(-0.3, 0.3, 1);
    glColor3f(1, 1, 1);
    glutWireTeapot(0.1);
}

void testApp::render_2d(ofEventArgs &args)
{
    ofSetColor(255, 255, 255);
    kinect.drawDepth(10, 15, 200, 150);
    ofDrawBitmapString("press h for help",10,10);
}

void testApp::exit() 
{
    ofLog(OF_LOG_VERBOSE, "exit!");

    ofRemoveListener(_cml->render_texture, this, &testApp::render_texture);
    ofRemoveListener(_cml->render_3d, this, &testApp::render_3d);
    ofRemoveListener(_cml->render_2d, this, &testApp::render_2d);

    _cml->dispose();
    _d2m->dispose();

    kinect.close();
}

//void testApp::init_keys()
//{
//for (int i = 0; i < 512; i++) 
//pressed[i] = false;
//}

void testApp::keyPressed (int key) 
{
    switch (key) {

        case 'w':
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
            break;

    }
}

void testApp::mouseDragged(int x, int y, int button)
{}

void testApp::mousePressed(int x, int y, int button)
{}

void testApp::mouseReleased(int x, int y, int button)
{}

void testApp::windowResized(int w, int h)
{}
