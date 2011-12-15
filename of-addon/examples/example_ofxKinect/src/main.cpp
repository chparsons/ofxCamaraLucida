#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() 
{
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 1024, 640, OF_WINDOW);
	ofRunApp(new testApp());
}
