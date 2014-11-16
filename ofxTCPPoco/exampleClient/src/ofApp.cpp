#include "ofApp.h"

/*
 ofxTCPPocoClient client example.
 - client receives messages from server
 - client sends a message on key press
 - default message framing protocol- ? (send header with size (4 bytes) followed by message)
 */

//--------------------------------------------------------------
void ofApp::setup(){

    ofSetFrameRate(60);
    
    // client
    int port = 12345;
    string ip = "localhost";
    client.connect(ip, port);
    
}

//--------------------------------------------------------------
void ofApp::update(){

    while(client.hasWaitingMessage()) {
        string replyMessage;
        client.getMessage(replyMessage);
        receivedMessages.push_back( replyMessage);
    }

}

//--------------------------------------------------------------
void ofApp::draw(){

    ofSetColor(255);
    ofDrawCircle(ofGetMouseX(), ofGetMouseY(), 20);
    
    
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
    output << "Connected to server: " << client.connected << endl;
    output << "Fps: " << ofGetFrameRate();
    ofDrawBitmapStringHighlight(output.str(), 20, 20);

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){


}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
//    if(key == '1') {
//        client.disconnect();
//        return;
//    } else if(key == '2') {
//        client.shutdown();
//        return;
//    }

    // send message
    string message = "Hello from client " + ofToString(sentMessages.size());
    bool sentRequest = client.sendMessage(message);
    
    if(sentRequest) {
        sentMessages.push_back(message);
    }
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
