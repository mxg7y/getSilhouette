#include "testApp.h"

void testApp::setup(){
	ofSetFrameRate(60);
	ofBackgroundHex(0x000000);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	//GUI
	gui.setup("OpenCV control panel", 0, 0, ofGetWidth(), ofGetHeight());
	gui.addPanel("Background subtraction", 3);

	gui.setWhichColumn(0);
	gui.addToggle("use live video", "use_live", 0);
	gui.addToggle("draw input video", "draw_input", 0);
	gui.addToggle("grab background", "grab", 0);
	gui.addSlider("threshold", "thresh", 80, 0, 200, true);
	vector <string> names;
	names.push_back("abs diff");
	names.push_back("greater than");
	names.push_back("less than");
	gui.addTextDropDown("difference mode", "diff_mode", 0, names);
	
	gui.setWhichColumn(1);
	gui.addDrawableRect("Color Image", &bgSub.color, 320, 240);
	gui.addDrawableRect("Gray Background", &bgSub.background, 320, 240);
	
	gui.setWhichColumn(2);
	gui.addDrawableRect("Grayscale Image", &bgSub.gray, 320, 240);
	gui.addDrawableRect("Gray Diffarence", &bgSub.thresh, 320, 240);
	gui.loadSettings("controlPanelSettings.xml");

	gui.setupEvents();
	gui.enableEvents();
	ofAddListener(gui.createEventGroup("grab"), this, &testApp::grabBackgroundEvent);
	ofAddListener(gui.createEventGroup("draw_input"), this, &testApp::drawInputEvent);
    ofAddListener(gui.createEventGroup("use_live"), this, &testApp::toggleLiveVideoEvent);
	
	vidPlayer.loadMovie("fingers.mov");
	
	if (gui.getValueI("draw_input") == 1) {
		drawInputVideo = true;
	} else {
		drawInputVideo = false;
	}

    if (gui.getValueI("use_live") == 1) {
		vidPlayer.stop();
		vidGrabber.setVerbose(true);
		vidGrabber.initGrabber(320,240);
		bgSub.setup(vidGrabber.getWidth(), vidGrabber.getHeight());
		useLiveVideo = true;
	} else {
		vidGrabber.close();
		vidPlayer.play();
		bgSub.setup(vidPlayer.getWidth(), vidPlayer.getHeight());
		useLiveVideo = false;
	}
	
	for (int i = 0; i < 5000; i++){
		particle myParticle;
		myParticle.setInitialCondition(ofRandom(0,ofGetWidth()),ofRandom(0,ofGetHeight()),0,0);
		particles.push_back(myParticle);
	}
	
	VF.setupField(60, 40, ofGetWidth(), ofGetHeight());
	bForceInward = false;
}

void testApp::grabBackgroundEvent(guiCallbackData & data){
	if( data.isElement("grab") && data.getInt(0) == 1 ){
		bgSub.captureBackground();
		gui.setValueB("grab", false);
	}
}

void testApp::drawInputEvent(guiCallbackData &data) {
	if (data.isElement("draw_input") && data.getInt(0) == 1) {
		drawInputVideo = true;
	} else {
		drawInputVideo = false;
	}
}

void testApp::toggleLiveVideoEvent(guiCallbackData & data) {
	bLearnBakground = true;
	if (useLiveVideo) {
		vidGrabber.close();
		vidPlayer.play();
		bgSub.setup(vidPlayer.getWidth(), vidPlayer.getHeight());
		useLiveVideo = false;
	} else {
		vidPlayer.stop();
		vidGrabber.setVerbose(true);
		vidGrabber.initGrabber(320,240);
		bgSub.setup(vidGrabber.getWidth(), vidGrabber.getHeight());
		useLiveVideo = true;
	}
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

void testApp::update(){
    bool bNewFrame = false;
	
	if (useLiveVideo) {
		vidGrabber.update();
		bNewFrame = vidGrabber.isFrameNew();
	} else {
		vidPlayer.update();
		bNewFrame = vidPlayer.isFrameNew();		
	}
	
	if (bNewFrame){
		if (useLiveVideo) {
			bgSub.update(vidGrabber.getPixels(), vidGrabber.getWidth(), vidGrabber.getHeight());
		} else {
			bgSub.update(vidPlayer.getPixels(), vidPlayer.getWidth(), vidPlayer.getHeight());
		}

		bgSub.setDifferenceMode(gui.getValueI("diff_mode"));;
		bgSub.setThreshold(gui.getValueI("thresh"));
		VF.setFromPixels(bgSub.threshSmall.getPixels(), bForceInward, 0.1f);
	}
	
	for (int i = 0; i < particles.size(); i++){
		particles[i].resetForce();
		// get the force from the vector field: 
		ofVec2f frc;
		frc = VF.getForceFromPos(particles[i].pos.x, particles[i].pos.y);
		particles[i].addForce(frc.x, frc.y);
		particles[i].addDampingForce();
		particles[i].update();
		
	}

	gui.update();	
}

// -------------------------------------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------------------------------------------------------

void testApp::draw(){
	ofFill();
	ofSetHexColor(0xffffff);
	
	glPushMatrix();
	glScalef((float) ofGetWidth() / (float)bgSub.thresh.width, (float) ofGetHeight() / (float)bgSub.thresh.height, 1.0f);
	if (drawInputVideo) {
		bgSub.color.draw(0, 0);
	}
	ofPopMatrix();

	ofSetHexColor(0x999999);
	VF.draw();
	
	ofSetHexColor(0x3399ff);
	for (int i = 0; i < particles.size(); i++){
		particles[i].draw();
	}
	
	gui.draw();	
}

void testApp::keyPressed  (int key){ 
	for (int i = 0; i < particles.size(); i++){
		particles[i].setInitialCondition(ofRandom(0,ofGetWidth()), ofRandom(0,ofGetHeight()), 0,0);
	}
}

void testApp::mouseDragged(int x, int y, int button){
	gui.mouseDragged(x, y, button);
}

void testApp::mousePressed(int x, int y, int button){
	gui.mousePressed(x, y, button);
}

void testApp::mouseReleased(int x, int y, int button){
	gui.mouseReleased();
}