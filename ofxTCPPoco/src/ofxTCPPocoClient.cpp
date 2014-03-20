
#include "ofxTCPPocoClient.h"


ofxTCPPocoClient::ofxTCPPocoClient() {
    
    connected = false;
    setConnectTimeout(10);
    //setReceiveTimeout(1);
    //setSendTimeout(1);
    setPollTimeout(TCPPOCO_POLL_TIME);
    //sleepTime = 16;
    waitingMessage = false;
    
}

ofxTCPPocoClient::~ofxTCPPocoClient() {
    //waitForThread();
}


void ofxTCPPocoClient::setup(string ipAddr, int port) {
        
    // setup tcp poco client    
    socketAddress = new Poco::Net::SocketAddress(ipAddr, port);
    socketStream = new Poco::Net::StreamSocket();
    
    try {
        
        socketStream->connect(*socketAddress, connectTimeout);
        //socketStream->setReceiveTimeout(receiveTimeout);
        //socketStream->setSendTimeout(sendTimeout);
        connected = true;
        ofLog() << "ofxTCPPocoClient connected";
        
        // start the thread
        //startThread(); // true, false = blocking, verbose by default
        
    } catch (Poco::Exception& exc) {
        
        ofLog() << "ofxTCPPocoClient Error: could not create stream socket- " << exc.displayText();
    }

}

// send
//--------------------------------------------------------------
void ofxTCPPocoClient::sendMessage(string msg) {
    
    if(connected) {
        bool result = ofxTCPPocoUtils::sendRawString(socketStream, msg, TCPPOCO_DEFAULT_MSG_SIZE);
    }
}

void ofxTCPPocoClient::sendRawBuffer(ofBuffer& buffer) {
    sendRawBuffer(buffer.getBinaryBuffer(), buffer.size());
}

void ofxTCPPocoClient::sendRawBuffer(char* buffer, int sendSize) {
    
    if(connected) {
        bool result = ofxTCPPocoUtils::sendRawBuffer(socketStream, buffer, sendSize);
    }
}

// threaded receive
//--------------------------------------------------------------
/*void ofxTCPPocoClient::threadedFunction(){
    
    while( isThreadRunning() ){
        
        // need to thread this bit...
        // polls the socket for 5 seconds
        ofLog() << "1. " <<socketStream->available();
        bool pollSuccess = socketStream->poll(pollTimeout,Poco::Net::Socket::SELECT_READ);
        
        mutex.lock();
        waitingMessage = pollSuccess;
        mutex.unlock();
        
        sleep(sleepTime);
    }

}*/

// receive
//--------------------------------------------------------------
bool ofxTCPPocoClient::hasWaitingMessage() {
    
    /*Poco::ScopedLock<ofMutex> lock(mutex);
    ofLog() << "2. " << socketStream->available();
    if(socketStream->available()) {*/
    waitingMessage = (socketStream->available() > 0) ? true : false;
    return waitingMessage;
}

void ofxTCPPocoClient::receiveMessage(string& msg) {
    
    // if (socket().poll(timeOut,Poco::Net::Socket::SELECT_READ)){
    if(connected) {
        bool success = ofxTCPPocoUtils::receiveRawString(socketStream, msg, TCPPOCO_DEFAULT_MSG_SIZE);
        waitingMessage = false;
    }
}

void ofxTCPPocoClient::receiveRawBuffer(ofBuffer& buffer) {
    
    // make sure ofBuffer is allocated to correct receive size + 1!
    receiveRawBuffer(buffer.getBinaryBuffer(), buffer.size());
}

void ofxTCPPocoClient::receiveRawBuffer(char* buffer, int receiveSize) {
    
    if(connected) {
        bool success = ofxTCPPocoUtils::receiveRawBuffer(socketStream, buffer, receiveSize);
        waitingMessage = false;
    }
}

//--------------------------------------------------------------
void ofxTCPPocoClient::update() {
    
}

void ofxTCPPocoClient::draw() {
    
}


//--------------------------------------------------------------
void ofxTCPPocoClient::setConnectTimeout(int timeoutInSeconds) {
    connectTimeout.assign(timeoutInSeconds, 0);
    if(connected) {
        ofLog() << "ofxTCPPocoClient connect timeout must be called before setup";
    }
}

/*void ofxTCPPocoClient::setReceiveTimeout(int timeoutInSeconds) {
    receiveTimeout.assign(timeoutInSeconds, 0);
    if(connected) {
        socketStream->setReceiveTimeout(receiveTimeout);
    }
}

void ofxTCPPocoClient::setSendTimeout(int timeoutInSeconds) {
    sendTimeout.assign(timeoutInSeconds, 0);
    if(connected) {
        socketStream->setSendTimeout(sendTimeout);
    }
}*/

void ofxTCPPocoClient::setPollTimeout(int timeoutInSeconds) {
    
    pollTimeout.assign(timeoutInSeconds, 0);
}
