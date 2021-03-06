#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup() {

  ofBackground(0);
  ofSetFrameRate(60);

	kinect.setRegistration(true);
  kinect.init(false, false);
	kinect.open();
  kinect.setDepthClipping(500, 3000);
  kinectAngle = 30;
	kinect.setCameraTiltAngle(kinectAngle);

  grayImage.allocate(kinect.width, kinect.height);
  frontPanelGradient.allocate(kinect.width, kinect.height);
  backPanelGradient.allocate(kinect.width, kinect.height);
  warpedImage.allocate(kinect.width, kinect.height);

  savedFrontGradient.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
  savedBackGradient.allocate(kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

	nearThreshold = 238;
  farThreshold = 137;
  panelsDiff = 100;

    farThreshold2 = farThreshold - panelsDiff;
    nearThreshold2 = nearThreshold- panelsDiff;

    panelsCtls[0].x  = ofMap(nearThreshold, 0, 255, 480, 0);
    panelsCtls[0].y = 512;


    panelsCtls[1].x  = ofMap(farThreshold, 0, 255, 480, 0);
    panelsCtls[1].y = 0;


    panelsCtls[2].x  = ofMap(nearThreshold2, 0, 255, 480, 0);
    panelsCtls[2].y = 512;


    panelsCtls[3].x  = ofMap(farThreshold2, 0, 255, 480, 0);
    panelsCtls[3].y = 0;


    refPoint.x = panelsCtls[0].x;
    refPoint.y = 0;

    checkAngle = false;

    //keyStone

    if( XML.loadFile("corners.xml") ){
              cout<<"XML loaded"<<endl;
            }else{
              cout<<"unable to load XML"<<endl;
    }

    /*
    srcPositions  = new ofPoint[4];
    srcPositions[0].set(0, 0, 0);
    srcPositions[1].set(kinect.width, 0, 0);
    srcPositions[2].set(kinect.width, kinect.height, 0);
    srcPositions[3].set(0, kinect.height, 0);
    */
    srcPositions  = new ofPoint[4];
    srcPositions[0].set(XML.getValue("SRC:X1", 0), XML.getValue("SRC:Y1", 0), 0);
    srcPositions[1].set(XML.getValue("SRC:X2", kinect.width), XML.getValue("SRC:Y2", 0), 0);
    srcPositions[2].set(XML.getValue("SRC:X3", kinect.width), XML.getValue("SRC:Y3", kinect.height), 0);
    srcPositions[3].set(XML.getValue("SRC:X4", 0), XML.getValue("SRC:Y4", kinect.height), 0);

    dstPositions  = new ofPoint[4];
    dstPositions[0].set(0, 0, 0);
    dstPositions[1].set(kinect.width, 0, 0);
    dstPositions[2].set(kinect.width, kinect.height, 0);
    dstPositions[3].set(0, kinect.height, 0);

   wichPoints = true;

}


void testApp::computePanels(ofxCvGrayscaleImage & gray, ofxCvGrayscaleImage & frontGradient, ofxCvGrayscaleImage & backGradient, int & near1, int & near2, int & far1, int & far2){

    unsigned char* gradient1 = frontGradient.getPixels();

        for (int i = 0; i < 640; i++){
            for (int j = 0; j < 480; j++){

                int jVal = ofMap(j, 0, 480, far1, near1);

        gradient1[j*640 + i] = jVal;
      }
    }
  frontGradient.flagImageChanged();

  unsigned char* gradient2 = backGradient.getPixels();
        for (int i = 0; i < 640; i++){
            for (int j = 0; j < 480; j++){

                int jVal = ofMap(j, 0, 480, far2, near2);

        gradient2[j*640 + i] = jVal;
      }
    }
    backGradient.flagImageChanged();
}

void testApp::isPixelBetweenPanels(ofxCvGrayscaleImage & gray, ofxCvGrayscaleImage & frontGradient, ofxCvGrayscaleImage & backGradient){

      unsigned char * pix = gray.getPixels();

			unsigned char * pan1Pix = frontGradient.getPixels();
			unsigned char * pan2Pix = backGradient.getPixels();

			int numPixels = gray.getWidth() * gray.getHeight();
			for(int i = 0; i < numPixels; i++) {
				if(pix[i]!=0 && pix[i] <  pan1Pix[i]  && pix[i] > pan2Pix[i]) {
					pix[i] = pix[i];
				} else {
					pix[i] = 0;
				}
			}
			gray.flagImageChanged();
}

void drawGoodPoints(ofxCvGrayscaleImage & gray){




}

void testApp::drawKeystone(){


  // Draw keyStone
	ofSetColor(255);
	ofNoFill();

	ofBeginShape();
	for (int j = 0; j < 4; j++){
		ofVertex(srcPositions[j].x, srcPositions[j].y);
	}
	ofEndShape(true);

	// Draw points
	ofFill();
	ofSetColor(255);
	for (int j = 0; j < 4; j++){
		ofCircle(srcPositions[j].x, srcPositions[j].y, 3);
	}

}

//--------------------------------------------------------------
void testApp::update() {

    nearThreshold2 = nearThreshold - panelsDiff;
    farThreshold2 = farThreshold - panelsDiff;

    panelsCtls[2].x = ofMap(nearThreshold2, 0, 255, 480, 0);
    panelsCtls[3].x = ofMap(farThreshold2, 0, 255, 480, 0);


   if(checkAngle){
    refPoint.x = panelsCtls[0].x;
    refVec.x = refPoint.x;
    refVec.y = refPoint.y;

    nearVec.x = panelsCtls[0].x;
    nearVec.y = panelsCtls[0].y;

    farVec.x = panelsCtls[1].x;
    farVec.y = panelsCtls[1].y;


    corrAngle = refVec.distance(farVec)/refVec.distance(nearVec);
    kAngle = tan(ofDegToRad(30));
   }


    kinect.update();


    if(kinect.isFrameNew()) {

        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);

        computePanels(grayImage, frontPanelGradient, backPanelGradient, nearThreshold, nearThreshold2, farThreshold, farThreshold2);
        isPixelBetweenPanels(grayImage, frontPanelGradient, backPanelGradient);

        warpedImage.warpIntoMe(grayImage, srcPositions, dstPositions);
    }

}

//--------------------------------------------------------------
void testApp::draw() {

	ofSetColor(255, 255, 255);


        ofLine(panelsCtls[0], panelsCtls[1]);
        ofLine(panelsCtls[2], panelsCtls[3]);
        ofCircle(panelsCtls[0], 10);
        ofCircle(panelsCtls[1], 10);

        drawKeystone();


       if(checkAngle){
          ofSetColor(255, 0 , 0);
          ofCircle(refPoint, 10);
          ofLine(refPoint, panelsCtls[0]);
          ofSetColor(255, 255, 255);

          ofDrawBitmapString(ofToString(corrAngle)+" "+ofToString(kAngle), panelsCtls[0].x+20, panelsCtls[0].y-40);

       }
        //grayImage.draw(960, 0, 320, 240);
        warpedImage.draw(960, 0, 320, 240);

        kinect.drawDepth(640, 0, 320, 240);
        ofDrawBitmapString("DEPTH", 650, 10);
        frontPanelGradient.draw(640, 240, 320, 240);
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("FRONT "+ ofToString(farThreshold)  + " "+  ofToString(nearThreshold), 650, 250);
        ofSetColor(255, 255, 255);
        backPanelGradient.draw(960, 240, 320, 240);
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("BACK "+ ofToString(farThreshold2) + " "+  ofToString(nearThreshold2), 970, 250);
        ofSetColor(255, 255, 255);

        ofDrawBitmapString("press SPACE to save", 650, 500);

	}


//--------------------------------------------------------------
void testApp::exit() {

	kinect.close();

}

//--------------------------------------------------------------
void testApp::keyPressed (int key) {
	switch (key) {
        case 'a':
          checkAngle = !checkAngle;
        break;

        case ' ':
          savedFrontGradient.setFromPixels(frontPanelGradient.getPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);
          savedBackGradient.setFromPixels(backPanelGradient.getPixels(), kinect.width, kinect.height, OF_IMAGE_GRAYSCALE);

          savedFrontGradient.saveImage("/home/ekko/Desktop/front.png", OF_IMAGE_QUALITY_BEST);
          savedBackGradient.saveImage("/home/ekko/Desktop/back.png", OF_IMAGE_QUALITY_BEST);

          XML.setValue("SRC:X1", srcPositions[0].x);

            XML.setValue("SRC:Y1", srcPositions[0].y);
            XML.setValue("SRC:X2", srcPositions[1].x);
            XML.setValue("SRC:Y2", srcPositions[1].y);
            XML.setValue("SRC:X3", srcPositions[2].x);
            XML.setValue("SRC:Y3", srcPositions[2].y);
            XML.setValue("SRC:X4", srcPositions[3].x);
            XML.setValue("SRC:Y4", srcPositions[3].y);

            XML.saveFile("corners.xml");
        break;

        case 't':
            panelsDiff ++;
        break;
        case 'e':
        if(nearThreshold2 < nearThreshold && farThreshold2 < farThreshold ){

                panelsDiff -= 1;
              }
        break;
        case 'c':
          kinect.setCameraTiltAngle(0); // zero the tilt
          kinect.close();
        break;
        case OF_KEY_UP:
          kinectAngle++;
          if(kinectAngle>30) kinectAngle=30;
          kinect.setCameraTiltAngle(kinectAngle);
        break;

        case OF_KEY_DOWN:
          kinectAngle--;
          if(kinectAngle<-30) kinectAngle=-30;
          kinect.setCameraTiltAngle(kinectAngle);
        break;

        case OF_KEY_LEFT:


            if(wichPoints){
              srcPositions[2].x += 1;
              srcPositions[3].x -= 1;

            }else{
              srcPositions[0].x += 1;
              srcPositions[1].x -= 1;
            }
        break;
        case OF_KEY_RIGHT:

            if(wichPoints){

              srcPositions[2].x -= 1;
              srcPositions[3].x += 1;


            }else{

              srcPositions[0].x -= 1;
              srcPositions[1].x += 1;

            }
        break;
        case 'y':
          wichPoints = !wichPoints;
        break;
        case 'r':
          srcPositions[0].set(0, 0, 0);
          srcPositions[1].set(kinect.width, 0, 0);
          srcPositions[2].set(kinect.width, kinect.height, 0);
          srcPositions[3].set(0, kinect.height, 0);
        break;

      }
}





//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    if (activePointIn > -1){
        if(activePointIn == 0){
          panelsCtls[activePointIn].x = x;
          panelsCtls[activePointIn].y = 512;

          nearThreshold = ofMap(x, 480, 0, 0, 255);
        }else{

          panelsCtls[activePointIn].x = x;
          panelsCtls[activePointIn].y = 0;

          farThreshold = ofMap(x, 480, 0, 0, 255);

        }
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    //this is not the best way
    activePointIn       = -1;

    float smallestDist  = 999999;
    float clickRadius = 10;

	for (int j = 0; j < 2; j++){
		ofPoint inputPt;
		inputPt.x = panelsCtls[j].x;
		inputPt.y = panelsCtls[j].y;
		inputPt.z = 0;
		float len = sqrt( (inputPt.x - x) * (inputPt.x - x) +
							(inputPt.y - y) * (inputPt.y - y));
		if (len < clickRadius && len < smallestDist){
			activePointIn  = j;
			smallestDist = len;
		}
	}
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}



