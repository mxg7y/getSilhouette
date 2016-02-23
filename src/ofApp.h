#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxControlPanel.h"
#include "BackgroundSubtraction.h"
#include "ofFileUtils.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	
	void grabBackgroundEvent(guiCallbackData & data);
	void drawInputEvent(guiCallbackData & data);
	void toggleLiveVideoEvent(guiCallbackData & data);
};

