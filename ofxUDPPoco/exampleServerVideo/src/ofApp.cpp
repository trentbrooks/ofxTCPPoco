#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    
    
    // udp server
    server.setup(5004);
    server.setReceiveSize(640*480*3); // anything big enough to hold video
}

//--------------------------------------------------------------
void ofApp::update(){

    if(server.hasWaitingMessage()) {
        
        ofBuffer buffer;
        server.getRawBuffer(buffer);
        ofLoadImage(video, buffer);
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    if(video.isAllocated()) {
        video.draw(0, 0);
    }
    
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