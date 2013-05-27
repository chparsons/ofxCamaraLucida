#include "testApp.h"

void testApp::setup() 
{
  ofSetLogLevel(OF_LOG_VERBOSE);
  ofSetWindowPosition(0,0);
  ofSetFrameRate(60);
  ofBackground(50);


  kinect.setRegistration(false);
  //kinect.setDepthMode( FREENECT_DEPTH_11BIT );
  kinect.setDepthClipping(500, 4000); //mm (50cm - 4mt)
  kinect.enableDepthNearValueWhite(false);
  // ir:false, rgb:false, texture:true
  kinect.init(false, false, true);
  kinect.open();


  string cfg = ofToDataPath("camara_lucida/config.xml");

  depthmap = new cml::Depthmap_freenect();
  cml = new cml::CamaraLucida( cfg, depthmap );

  ofAddListener( cml->render_texture, 
      this, &testApp::render_texture );

  ofAddListener( cml->render_3d, 
      this, &testApp::render_3d );

  ofAddListener( cml->render_2d, 
      this, &testApp::render_2d );

}

void testApp::update() 
{	
  if ( ! kinect.isConnected() )
    return;

  kinect.update();

  if ( kinect.isFrameNew() )
  { 
    depthmap->update( 
        kinect.getRawDepthPixels() );
  }
}

void testApp::draw() 
{    
  cml->render(); 
}

void testApp::render_texture(ofEventArgs &args)
{
  glClearColor(0.5, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float w = cml->tex_width();
  float h = cml->tex_height();

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
  glScalef( 1., -1., 1. );	
  glTranslatef( -0.3, 0.3, 1. );
  glColor3f(1, 1, 1);
  glutWireTeapot(0.1);
}

void testApp::render_2d(ofEventArgs &args)
{
  ofSetColor(255, 255, 255);

  kinect.drawDepth(0, 0, 200, 150);
  //kinect.drawDepth(0, -100, 1024, 768);

  ofDrawBitmapString("press h for help",10,10);
}

void testApp::exit() 
{
  ofLog(OF_LOG_VERBOSE, "exit!");

  ofRemoveListener( cml->render_texture, 
      this, &testApp::render_texture );

  ofRemoveListener( cml->render_3d, 
      this, &testApp::render_3d );

  ofRemoveListener( cml->render_2d, 
      this, &testApp::render_2d );

  cml->dispose();
  depthmap->dispose();

  kinect.close();
}

void testApp::keyPressed (int key) 
{
  switch (key) {

    case 'w':
      cml->wireframe( ! cml->wireframe() );
      break;

    case 'e':
      kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
      break;

    case 'f':
      ofToggleFullscreen();
      break;

    case 'p':
      ofSetWindowPosition( ofGetWindowPositionX() == 0 ? 1440 : 0, 0 );
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
