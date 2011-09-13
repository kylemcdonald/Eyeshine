#include "testApp.h"
#include "ofAppGlutWindow.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, 320 * 3, 240 * 2, OF_WINDOW);
	ofRunApp(new testApp());
}
