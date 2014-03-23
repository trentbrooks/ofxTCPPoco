#include "ofApp.h"

/*
 ofxTCPPocoClientVideo client example.
 - client sends a ping
 - receives a reply message (header) with size of image buffer
 - then receives the image buffer
 */

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
    if(image.isAllocated()) image.draw(0,0);
    
   
    
    // info
    stringstream output;
    output << "ofxTCPPocoClient Video." << endl;
    output << "FPS: " << ofGetFrameRate() << endl;
    output << "Connected to server: " << client.connected << endl;
    output << "Received buffer bytes: " << bufferSize;
    ofDrawBitmapStringHighlight(output.str(), 20, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    
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
