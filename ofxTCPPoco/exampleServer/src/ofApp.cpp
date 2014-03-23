#include "ofApp.h"


/*
 ofxTCPPocoServer server example.
 - server receives a request or ping
 - server sends back a reply or ack
 */
//--------------------------------------------------------------
void ofApp::setup(){

    //ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    // server
    int port = 12345;
    server.setup(port);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    // check server for incoming messages from all clients
    for(int i = 0; i < server.getNumClients(); i++) {
        
        // set a receive size per client connection before checking for requests, so know what buffer size needs to be filled for a request
        //server.setReceiveSize(i, TCPPOCO_DEFAULT_MSG_SIZE);
        if(server.hasWaitingRequest(i)) {
            
            string message;
            bool receivedRequest = server.receiveMessage(i, message);
            if(receivedRequest) {
                
                // once request is received, do something with it then respond with a reply or ack
                receivedMessages.push_back(message);
                
                string replyMessage = "Thanks, server received:" + ofToString(message.size()) + " bytes";
                bool sentReply = server.sendMessage(i, replyMessage);
                if(sentReply) {
                    sentMessages.push_back(replyMessage);
                }
            } else {
                ofLog() << "failed to receive";
            }
            
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){


    ofPushMatrix();
    ofPushStyle();
    ofSetColor(255);
    ofTranslate(ofGetWidth()*.5, ofGetHeight()*.5);
    ofRotateZ(sin(ofGetElapsedTimef()) * 360);
    ofRect(-50, -50, 100, 100);
    ofPopStyle();
    ofPopMatrix();
    
    
    // display received messages/requests
    stringstream receiveOutput;
    receiveOutput << "RECEIVED MESSAGES..." << endl;
    for(int i = receivedMessages.size()-1; i >= 0; i--) {
        receiveOutput << receivedMessages[i] << endl;
    }
    ofSetColor(0);
    ofDrawBitmapString(receiveOutput.str(), 20, 80);
    
    // display sent messages/replies
    stringstream sendOutput;
    sendOutput << "SENT MESSAGES..." << endl;
    for(int i = sentMessages.size()-1; i >= 0; i--) {
        sendOutput << sentMessages[i] << endl;
    }
    ofSetColor(0);
    ofDrawBitmapString(sendOutput.str(), ofGetWidth()*.5, 80);
    
    
    
    
    
    // info
    stringstream output;
    output << "ofxTCPPocoServer." << endl;
    output << "Press a key to send a message" << endl;
    output << "Client connections: " << server.getNumClients();
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
