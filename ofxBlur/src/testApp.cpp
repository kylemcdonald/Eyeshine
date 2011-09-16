#include "testApp.h"

float Gaussian(float x, float mean, float variance) {
	x -= mean;
  return (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
}

void GaussianRow(int elements, vector<float>& row, float variance = .2) {
	row.resize(elements);
	for(int i = 0; i < elements; i++) {
		float x = ofMap(i, 0, elements - 1, -1, 1);
		row[i] = Gaussian(x, 0, variance);
	}
}

string generateBlurSource(int radius) {
	int level = (radius / 2) - 1;
	int rowSize = 1 + (level + 1) * 4;
	
	// generate row
	vector<float> row;
	GaussianRow(rowSize, row);
	
	cout << rowSize << " " << row.size() << endl;
	
	// normalize row and coefficients
	vector<float> coefficients;
	float sum = 0;
	for(int i = 0; i < row.size(); i++) {
		sum += row[i];
	}
	for(int i = 0; i < row.size(); i++) {
		row[i] /= sum;
	}
	int center = row.size() / 2;
	coefficients.push_back(row[center]);
	for(int i = center + 1; i < row.size(); i += 2) {
		float weightSum = row[i] + row[i + 1];
		coefficients.push_back(weightSum);
	}
	
	// generate offsets
	vector<float> offsets;
	for(int i = center + 1; i < row.size(); i += 2) {		
		int left = i - center;
		int right = left + 1;
		float leftVal = row[i];
		float rightVal = row[i + 1];
		float weightSum = leftVal + rightVal;
		float weightedAverage = (left * leftVal + right * rightVal) / weightSum;
		offsets.push_back(weightedAverage);
	}
	
	stringstream src;
	src << "uniform sampler2DRect source;\n";
	src << "uniform vec2 direction;\n";
	src << "void main(void) {\n";
	src << "\tvec2 tc = gl_TexCoord[0].st;\n";
	src << "\tgl_FragColor = " << coefficients[0] << " * texture2DRect(source, tc);\n";
	for(int i = 1; i < coefficients.size(); i++) {
		int curOffset = i - 1;
		src << "\tgl_FragColor += " << coefficients[i] << " * \n";
		src << "\t\t(texture2DRect(source, tc - (direction * " << offsets[i - 1] << ")) + \n";
		src << "\t\ttexture2DRect(source, tc + (direction * " << offsets[i - 1] << ")));\n";
	}
	src << "}\n";
	
	return src.str();
}

string generateCombineSource(int reductions, float reductionFactor) {
	vector<string> combineNames;
	for(int i = 0; i < reductions; i++) {
		combineNames.push_back("s" + ofToString(i));
	}
	stringstream src;
	src << "uniform sampler2DRect " << ofJoinString(combineNames, ",") << ";\n";
	src << "const float scaleFactor = " << reductionFactor << ";\n";
	src << "void main(void) {\n";
	src << "\tvec2 tc = gl_TexCoord[0].st;\n";
	for(int i = 0; i < reductions; i++) {
		src << "\tgl_FragColor " << (i == 0 ? "=" : "+=");
		src << " texture2DRect(" << combineNames[i] << ", tc);";
		src << (i + 1 != reductions ? " tc *= scaleFactor;" : "");
		src << "\n";
	}
	src <<
	"\tgl_FragColor /= " << reductions << ".;\n"<<
	"}\n";
	return src.str();
}

void testApp::setup() {
	cam.initGrabber(1280, 720);
	
	reductions = 4;
	reductionFactor = 3./4.;
	int radius = 16;
	
	string blurSource = generateBlurSource(radius);
	cout << "loading:" << endl << blurSource << endl;
	blurShader.setupShaderFromSource(GL_FRAGMENT_SHADER, blurSource);
	blurShader.linkProgram();
	
	if(reductions > 1) {
		string combineSource = generateCombineSource(reductions, reductionFactor);
		cout << "loading:" << endl << combineSource << endl;
		combineShader.setupShaderFromSource(GL_FRAGMENT_SHADER, combineSource);
		combineShader.linkProgram();
	}
	
	int w = cam.getWidth();
	int h = cam.getHeight();
	ofFbo::Settings settings;
	for(int i = 0; i < reductions; i++) {
		settings.width = w;
		settings.height = h;
		
		ofFbo* curPing = new ofFbo();
		ofFbo* curPong = new ofFbo();
		
		curPing->allocate(settings);
		curPong->allocate(settings);
		
		ping.push_back(curPing);
		pong.push_back(curPong);
		
		w *= reductionFactor;
		h *= reductionFactor;
	}
}

void testApp::update() {	
	cam.update();

	ping[0]->begin();
	ofSetColor(255);
	cam.draw(0, 0);
	ofSetCircleResolution(64);
	ofCircle(mouseX, mouseY, 100);
	ping[0]->end();

	for(int i = 0; i < ping.size(); i++) {
		ofFbo* curPing = ping[i];
		ofFbo* curPong = pong[i];
		
		// resample previous result into ping
		if(i > 0) {
			curPing->begin();
			int w = curPing->getWidth();
			int h = curPing->getHeight();
			ofSetColor(255);
			ping[i - 1]->draw(0, 0, w, h);
			curPing->end();
		}
		
		// horizontal blur ping into pong
		curPong->begin();
		blurShader.begin();
		blurShader.setUniformTexture("source", ping[i]->getTextureReference(), 0);
		blurShader.setUniform2f("direction", 1, 0);
		ping[i]->draw(0, 0);
		blurShader.end();
		curPong->end();
		
		// vertical blur pong into ping
		curPing->begin();
		blurShader.begin();
		blurShader.setUniformTexture("source", pong[i]->getTextureReference(), 0);
		blurShader.setUniform2f("direction", 0, 1);
		pong[i]->draw(0, 0);
		blurShader.end();
		curPing->end();
	}		
}

void testApp::draw() {
	ofBackground(0);
	ofSetColor(255);
	
	if(reductions > 1) {
		combineShader.begin();
		for(int i = 0; i < reductions; i++) {
			string name = "s" + ofToString(i);
			combineShader.setUniformTexture(name.c_str(), ping[i]->getTextureReference(), i);
		}
		ofEnableNormalizedTexCoords();
		int w = ping[0]->getWidth();
		int h = ping[0]->getHeight();
		glBegin(GL_QUADS);
		glTexCoord2i(0, 0); glVertex2i(0, 0);
		glTexCoord2i(w, 0); glVertex2i(w, 0);
		glTexCoord2i(w, h); glVertex2i(w, h);
		glTexCoord2i(0, h); glVertex2i(0, h);
		glEnd();
		ofDisableNormalizedTexCoords();
		combineShader.end();
	} else {
		ping[0]->draw(0, 0);
	}
	
	//ofTranslate(0, cam.getHeight());
	
	ofSetColor(255);
	timer.tick();
	ofDrawBitmapString(ofToString((int) timer.getPeriodMilliseconds()) + " ms", 10, 20);
}
