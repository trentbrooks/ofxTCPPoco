
#include "ofxTCPPocoClient.h"


ofxTCPPocoClient::ofxTCPPocoClient() {
    
    connected = false;
    maxReadTime = 10;
    setConnectTimeout(10);
    //setReceiveTimeout(10);
    //setSendTimeout(1);
    //setPollTimeout(TCPPOCO_POLL_TIME);
    //sleepTime = 16;
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
        
        disconnect();
        
        ofLog() << "ofxTCPPocoClient Error: could not create stream socket- " << exc.displayText();
    }

}

void ofxTCPPocoClient::disconnect() {
    
    connected = false;
    if(socketAddress) delete socketAddress;
    if(socketStream) delete socketStream;
}

// send
//--------------------------------------------------------------
bool ofxTCPPocoClient::sendMessage(string& msg, int fillSize) {
    
    if(connected) {
        return ofxTCPPocoUtils::sendPaddedMessage(socketStream, msg, fillSize); // blocking
    }
    return false;
}

void ofxTCPPocoClient::sendRawBuffer(ofBuffer& buffer) {
    sendRawBuffer(buffer.getBinaryBuffer(), buffer.size());
}

void ofxTCPPocoClient::sendRawBuffer(char* buffer, int sendSize) {
    
    if(connected) {
        //bool result = ofxTCPPocoUtils::sendRawBuffer(socketStream, buffer, sendSize);
        bool result = ofxTCPPocoUtils::sendRawBytes(socketStream, buffer, sendSize); // blocking
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

// receive / replys
//--------------------------------------------------------------
// TODO: add polling instead of while loop, or figure out a better threading solution
// eg. http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler
bool ofxTCPPocoClient::receiveMessage(string& msg, int receiveSize) {
    
    if(connected) {
        
        // blocking
        float startReadTime = ofGetElapsedTimef();
        while (socketStream->available() < receiveSize) {
            // poll / waiting until received enough bytes
            
            // or manually set a timeout
            if(ofGetElapsedTimef() - startReadTime > maxReadTime) {
                disconnect(); // if we have not received data in X amount of time, the connection must be lost
                return false;
            }
        }
        
        // also blocking
        return ofxTCPPocoUtils::receivePaddedMessage(socketStream, msg, receiveSize);
    }
    return false;
}

bool ofxTCPPocoClient::receiveRawBuffer(ofBuffer& buffer, int receiveSize) {
    
    // make sure ofBuffer is allocated to correct receive size + 1!
    if(connected) {
        
        // blocking
        float startReadTime = ofGetElapsedTimef();
        while (socketStream->available() < receiveSize) {
            // poll / waiting until received enough bytes
            
            // or manually set a timeout
            if(ofGetElapsedTimef() - startReadTime > maxReadTime) {
                disconnect(); // if we have not received data in X amount of time, the connection must be lost
                return false;
            }
        }
        
        buffer.allocate(receiveSize+1);
        return ofxTCPPocoUtils::receiveRawBytes(socketStream, buffer.getBinaryBuffer(), receiveSize);
    }
    
    return false;
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
}*/

/*
void ofxTCPPocoClient::setSendTimeout(int timeoutInSeconds) {
    sendTimeout.assign(timeoutInSeconds, 0);
    if(connected) {
        socketStream->setSendTimeout(sendTimeout);
    }
}

void ofxTCPPocoClient::setPollTimeout(int timeoutInSeconds) {
    
    pollTimeout.assign(timeoutInSeconds, 0);
}*/
