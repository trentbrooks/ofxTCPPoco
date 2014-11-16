#include "ofApp.h"



//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(60);
    
    
    // udp client
    client.setup("localhost",5004);
    client.setMaxSendSize(36000); // default 9216
    ofLog() << "client setup: " << client.connected;
    
    video.initGrabber(640, 480);
}

//--------------------------------------------------------------
void ofApp::update(){

    video.update();
    if(video.isFrameNew()) {
        
        ofSaveImage(video.getPixels(), buffer, OF_IMAGE_FORMAT_JPEG, OF_IMAGE_QUALITY_LOW);
        
        // anything more than 9216 doesnt work?
        int sentBytes = client.sendRawBuffer(buffer.getData(), buffer.size());
        //ofLog() << "Sent bytes: " << sentBytes;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    video.draw(0, 0);

    
    // info
    stringstream output;
    output << "ofxUDPPocoClient." << endl;
    //output << "Press a key to send a message" << endl;
    ofDrawBitmapStringHighlight(output.str(), 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    // send message
    //string message = "Hello from client pressed:" + ofToString(key);
    //int sentBytes = client.sendMessage(message);
    //ofLog() << "Sent bytes: " << sentBytes;
    
    /*ofBuffer bufferCopy = buffer;


    ofLog() << "Sending pixels: " << bufferCopy.size();
    int sentBytes = client.sendRawBuffer(bufferCopy.getData(), bufferCopy.size());
    ofLog() << "Sent bytes: " << sentBytes;*/
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