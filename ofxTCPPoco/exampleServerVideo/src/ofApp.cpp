#include "ofApp.h"


/*
 ofxTCPPocoServerVideo server example.
 - server receives a request or ping from client
 - server sends back a reply with size of image buffer in bytes
 - then server sends image buffer
 */
//--------------------------------------------------------------
void ofApp::setup(){

    //ofSetVerticalSync(true);
    ofSetFrameRate(60);
    
    // test sending an image buffer when pressing 'i'
    video.initGrabber(640, 480);
    
    // server
    int port = 12345;
    server.setup(port);
}


//--------------------------------------------------------------
void ofApp::update(){
    
    video.update();
    
    if(video.isFrameNew()) {
        
        // thread the buffer encode/saving to increase performance
        ofSaveImage(video.getPixelsRef(), imageBuffer, OF_IMAGE_FORMAT_JPEG, OF_IMAGE_QUALITY_MEDIUM);
        
    }
    
    
    int s1, s2, s3, s4, s5;
    s1 = s2 = s3 = s4 = 0;
    int d1, d2, d3, d4, d5;
    d1 = d2 = d3 = d4 = 0;
    // check server for incoming messages from all clients
    // we are going to receive a ping message, and send back 2 messages (buffer size + image buffer)
    for(int i = 0; i < server.getNumClients(); i++) {
        
        s1 = ofGetElapsedTimeMillis();
        if(server.hasWaitingRequest(i)) {
            d1 = (ofGetElapsedTimeMillis() - s1);
            string pingMessage;
            s2 = ofGetElapsedTimeMillis();
            bool receivedPing = server.receiveMessage(i, pingMessage);
            if(receivedPing) {
                d2 = (ofGetElapsedTimeMillis() - s2);
                if(pingMessage == "ping") {
                    s3 = ofGetElapsedTimeMillis();
                    // send back a reply header with the size of the image buffer
                    string replyMessage = ofToString(imageBuffer.size());
                    bool sentReplyHeader = server.sendMessage(i, replyMessage);
                    d3 = (ofGetElapsedTimeMillis() - s3);
                    if(sentReplyHeader) {
                        s4 = ofGetElapsedTimeMillis();
                        // send back image buffer
                        string replyMessage = ofToString(imageBuffer.size());
                        bool sentReplyBuffer = server.sendRawBuffer(i, imageBuffer);
                        d4 = (ofGetElapsedTimeMillis() - s4);

                    }
                }
               
            } else {
                ofLog() << "failed to receive";
            }
            
        }
    }
    
    //ofLog() << "timings... " << d1 << ", " << d2 << ", " << d3 << ", " << d4;
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // draw the image we want to send
    ofSetColor(255);
    video.draw(0, 0);

    
    
    // info
    stringstream output;
    output << "ofxTCPPocoServer Video." << endl;
    output << "FPS: " << ofGetFrameRate() << endl;
    output << "Client connections: " << server.getNumClients() << endl;
    output << "Sent buffer bytes: " << imageBuffer.size();
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
