#include "ofApp.h"

/*
 ofxTCPPocoClient client example.
 - client sends a request or ping
 - receives a reply or ack
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


}

//--------------------------------------------------------------
void ofApp::draw(){

    // draw the image we received
    ofSetColor(255);
    
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

    // send message
    string message = "Hello from client pressed:" + ofToString(key);
    bool sentRequest = client.sendMessage(message);
    
    if(sentRequest) {
        sentMessages.push_back(message);
        
        // once a message is sent, read the reply from server (blocking)
        string replyMessage;
        bool receivedReply = client.receiveMessage(replyMessage, TCPPOCO_DEFAULT_MSG_SIZE); // blocking/waiting
        if(receivedReply) {
            receivedMessages.push_back( replyMessage);
        } else {
            ofLog() << "failed to receive reply";
        }
        
    } else {
        
        ofLog() << "failed to send request";
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
