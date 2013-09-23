#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"


class testApp : public ofBaseApp {
public:

	void setup();
	void update();
	void draw();
	void exit();

	void drawPointCloud();

	void keyPressed(int key);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	ofxKinect kinect;
  ofxCvGrayscaleImage grayImage, bgImage, diffImage, warpedImage; // grayscale depth image


  ofImage savedFrontGradient, savedBackGradient;

  int nearThreshold, farThreshold, nearThreshold2, farThreshold2;
  int panelsDiff;
	int kinectAngle;
  bool saveGradients, checkAngle;
  int activePointIn;
	int nearTY, farTY, nearT2Y, farT2Y;

  ofPoint  panelsCtls[4]; //[near, far, near2, far2]
  ofPoint refPoint;

  float pan1angle, pan2angle;

  ofVec2f refVec, nearVec, farVec;
  float corrAngle, kAngle;

	//different method using 2 gradients.
	ofxCvGrayscaleImage frontPanelGradient;
  ofxCvGrayscaleImage backPanelGradient;

  void computePanels(ofxCvGrayscaleImage & gray, ofxCvGrayscaleImage & frontGradient, ofxCvGrayscaleImage & backGradient, int & near1, int & near2, int & far1, int & far2);
  void isPixelBetweenPanels(ofxCvGrayscaleImage & gray, ofxCvGrayscaleImage & frontGradient, ofxCvGrayscaleImage & backGradient);
  void drawGoodPoints(ofxCvGrayscaleImage & gray);

  void drawKeystone();
  //warp stuff
  ofPoint *				srcPositions;
  ofPoint *				dstPositions;
  bool wichPoints;



};
