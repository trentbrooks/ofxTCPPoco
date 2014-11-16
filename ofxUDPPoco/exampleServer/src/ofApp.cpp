#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    
    
    // udp server
    server.bind(5004);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(server.hasWaitingMessage()) {
        
        // can receive as raw buffer
//        ofBuffer buffer;
//        server.getRawBuffer(buffer);
//        receivedMessages.push_back(buffer.getText());
        
        // or as string message
        string message;
        server.getMessage(message);
        receivedMessages.push_back(message);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(255);
    
    // display sent messages
    stringstream receiveOutput;
    receiveOutput << "RECEIVED MESSAGES..." << endl;
    for(int i = receivedMessages.size()-1; i >= 0; i--) {
        receiveOutput << receivedMessages[i] << endl;
    }
    ofSetColor(0);
    ofDrawBitmapString(receiveOutput.str(), 20, 80);
    
    // info
    stringstream output;
    output << "ofxUDPPocoServer." << endl;
    ofDrawBitmapStringHighlight(output.str(), 20, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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