#include "testApp.h"

int main() {
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 752, 480, OF_WINDOW);
	ofRunApp(new testApp());
}
