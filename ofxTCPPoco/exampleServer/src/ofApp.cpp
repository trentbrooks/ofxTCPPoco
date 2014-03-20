#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    //ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    // test sending an image buffer when pressing 'i'
    //image.loadImage("test.png");
    //ofSaveImage(image.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG, OF_IMAGE_QUALITY_MEDIUM);
    
    // server
    int port = 12345;
    server.setup(port);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // check server for incoming messages from all clients
    for(int i = 0; i < server.getNumClients(); i++) {
        
        if(server.hasWaitingMessage(i)) {
            string msg;
            server.getMessage(i, msg);
            receivedMessages.push_back(msg);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // draw the image we want to send
    //ofSetColor(255);
    //image.draw(ofGetWidth()*.5, ofGetHeight()*.5, 320, 240);

    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255);
    ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5);
    ofRotateZ(sin(ofGetElapsedTimef()) * 360);
    ofRect(-50, -50, 100, 100);
    ofPopStyle();
    ofPopMatrix();
    
    
    // display sent messages
    stringstream sendOutput;
    sendOutput << "SENT MESSAGES..." << endl;
    for(int i = sentMessages.size()-1; i >= 0; i--) {
        sendOutput << sentMessages[i] << endl;
    }
    ofSetColor(0);
    ofDrawBitmapString(sendOutput.str(), 20, 80);
    
    // display received messages
    stringstream receiveOutput;
    receiveOutput << "RECEIVED MESSAGES..." << endl;
    for(int i = receivedMessages.size()-1; i >= 0; i--) {
        receiveOutput << receivedMessages[i] << endl;
    }
    ofSetColor(0);
    ofDrawBitmapString(receiveOutput.str(), ofGetWidth()*.5, 80);
    
    
    
    // info
    stringstream output;
    output << "ofxTCPPocoServer." << endl;
    output << "Press a key to send a message" << endl;
    output << "Client connections: " << server.getNumClients();
    ofDrawBitmapStringHighlight(output.str(), 20, 20);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // send a message from server...
    for(int i = 0; i < server.getNumClients(); i++) {
        
        /*
        // send image buffer
        // to send a buffer- send a header message with the size, followed by the image.
        // then the receiver can change it's receive size
        server.sendMessage(i, ofToString(imageBuffer.size()));
        server.sendRawBuffer(i, imageBuffer);
        sentMessages.push_back("Just sent image bytes: " + ofToString(imageBuffer.size()));
         */
        
        // send string message
        string message = "Hello from server pressed: " + ofToString(key);
        server.sendMessage(i, message);
        sentMessages.push_back(message);
        
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
