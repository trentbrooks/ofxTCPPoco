#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	

    ofSetFrameRate(60);
    
    // client
    int port = 12345;
    string ip = "10.0.1.9";//"192.168.1.5";
    client.setup(ip, port);
}

//--------------------------------------------------------------
void ofApp::update(){

    string pingMessage = "ping";
    bool sentPing = client.sendMessage(pingMessage);
    if(sentPing) {
        
        string replyHeader; // this will contain the size in bytes of the image buffer
        bool receivedReply = client.receiveMessage(replyHeader, TCPPOCO_DEFAULT_MSG_SIZE);
        
        if(receivedReply) {
            
            ofBuffer buffer;
            bool receivedBufferReply = client.receiveRawBuffer(buffer, ofToInt(replyHeader));
            if(receivedBufferReply) {
                ofLoadImage(image, buffer);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	
    // draw the image we received
    ofSetColor(255);
    if(image.isAllocated()) image.draw(0,0, 50, 50);
    
    
    
    // info
    stringstream output;
    output << "ofxTCPPocoClient Video." << endl;
    output << "FPS: " << ofGetFrameRate() << endl;
    output << "Connected to server: " << client.connected << endl;
    output << "Received buffer bytes: " << bufferSize;
    ofDrawBitmapStringHighlight(output.str(), 20, 20);
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}
