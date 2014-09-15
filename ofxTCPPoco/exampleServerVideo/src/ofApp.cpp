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
    
    // test sending an image buffer
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
    
    
    // check server for incoming messages from all clients
    // we are going to receive a ping message, and send back 2 messages (buffer size + image buffer)
    for(int i = 0; i < server.getNumClients(); i++) {
        
        if(server.hasWaitingRequest(i)) {

            string pingMessage;
            bool receivedPing = server.receiveMessage(i, pingMessage);
            if(receivedPing) {
                if(pingMessage == "ping") {
                    
                    // send back a reply header with the size of the image buffer
                    string replyMessage = ofToString(imageBuffer.size());
                    bool sentReplyHeader = server.sendMessage(i, replyMessage);

                    if(sentReplyHeader) {
                        
                        // send back image buffer
                        string replyMessage = ofToString(imageBuffer.size());
                        bool sentReplyBuffer = server.sendRawBuffer(i, imageBuffer);
                    }
                }
               
            } else {
                ofLog() << "failed to receive";
            }
            
        }
    }
    
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
    output << "Buffer bytes: " << imageBuffer.size();
    ofDrawBitmapStringHighlight(output.str(), 20, 20);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

    server.printServerInfo();
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
