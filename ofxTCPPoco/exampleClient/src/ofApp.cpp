#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    
    // client
    int port = 12345;
    string ip = "localhost";
    client.setup(ip, port);
}

//--------------------------------------------------------------
void ofApp::update(){

    if(client.hasWaitingMessage()) {
        string msg;
        client.receiveMessage(msg);
        receivedMessages.push_back(msg);
        ofLog() << "client received: " << msg;
        
        /*
        // for this test, we receive a header which has the size of the incoming image buffer for the next message 
        string headerMessage;
        client.receiveMessage(headerMessage);
        receivedMessages.push_back(headerMessage);
        
        // we know the next image is the image buffer
        ofBuffer receiveBuffer;
        receiveBuffer.allocate(ofToInt(headerMessage)+1);
        client.receiveRawBuffer(receiveBuffer);
        ofLoadImage(image, receiveBuffer);
         */
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    // draw the image we received
    //ofSetColor(255);
    //if(image.isAllocated()) image.draw(ofGetWidth()*.5, ofGetHeight()*.5, 320, 240);
    
    ofPushMatrix();
    ofPushStyle();
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
    output << "ofxTCPPocoClient." << endl;
    output << "Press a key to send a message" << endl;
    output << "Connected to server: " << client.connected;
    ofDrawBitmapStringHighlight(output.str(), 20, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // send a message from client...
    string message = "Hello from client pressed:" + ofToString(key);
    client.sendMessage(message);
    sentMessages.push_back(message);
    
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
