#include "testApp.h"

int main() {
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 752, 480 * 3, OF_WINDOW);
	ofRunApp(new testApp());
}
