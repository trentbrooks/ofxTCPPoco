
#include "ofxTCPPocoClient.h"


ofxTCPPocoClient::ofxTCPPocoClient() {
    
    connected = false;
    waitingMessage = false;
    maxReadTime = 10;
    setConnectTimeout(10);
    //setReceiveTimeout(10);
    //setSendTimeout(1);
    //setPollTimeout(TCPPOCO_POLL_TIME);
    sleepTime = 10;
    socketAddress = NULL;
    socketStream = NULL;
}

ofxTCPPocoClient::~ofxTCPPocoClient() {
    ofLog() << "~ofxTCPPocoClient deleted";
    waitForThread();
    disconnect(); // may cause problems?
}


void ofxTCPPocoClient::connect(string ipAddr, int port) {
        
    // setup tcp poco client    
    socketAddress = new Poco::Net::SocketAddress(ipAddr, port);
    socketStream = new Poco::Net::StreamSocket();
    
    try {
        
        socketStream->connect(*socketAddress, connectTimeout);
        //socketStream->setReceiveTimeout(receiveTimeout);
        //socketStream->setSendTimeout(sendTimeout);
        connected = true;
        ofLog() << "ofxTCPPocoClient connected";
        //ofLog() << "Blocking: " << socketStream->getBlocking();
        //ofLog() << "Delay: " << socketStream->getNoDelay();
        
        // start the thread
        //startThread(); // true, false = blocking, verbose by default
        
        
    } catch (Poco::Exception& exc) {
        ofLogError() << "* ofxTCPPocoClient Error: could not create stream socket- " << exc.displayText();
        disconnect();
        return;
    }

    if(!isThreadRunning()) startThread();
}

void ofxTCPPocoClient::disconnect() {
    
    waitForThread();
    if(socketStream) {
        
        // http://stackoverflow.com/questions/3757289/tcp-option-so-linger-zero-when-its-required
        // if don't set this, the server sends a few success messages even if this is closed
        try {
            socketStream->setLinger(true, 0);
        } catch (Poco::Exception& exc) {
            // server must be closed or the above crashes on exit
            ofLogError() << "* ~ofxTCPPocoClient failed to setLinger (server must be closed?): " << exc.displayText();
        }
        
        // these all do the same thing - same as delete so not needed
        //socketStream->setKeepAlive(false);
        //socketStream->shutdown();
        //socketStream->close();
    }
    connected = false;
    delete socketAddress;
    socketAddress = NULL;
    delete socketStream;
    socketStream = NULL;
}

// send
//--------------------------------------------------------------
bool ofxTCPPocoClient::sendMessage(string& msg) {
    
    return sendRawBuffer(msg.data(), msg.size());
}

bool ofxTCPPocoClient::sendRawBuffer(ofBuffer& buffer) {
    return sendRawBuffer(buffer.getData(), buffer.size());
}

bool ofxTCPPocoClient::sendRawBuffer(const char* buffer, int size) {
    
    if(connected) {
        
        // 1. send a header with the message size
        char* header = (char*)(&size); // convert int to bytes (4) by casting
        bool sentHeader = sendBytes(header, 4);
        if(sentHeader) {
            // 2. send the main message
            bool sentMessage = sendBytes(buffer, size);
            return sentMessage;
        }
    }
    return false;
}

// threaded receive - no sleep, same as connection handler for server
//--------------------------------------------------------------
void ofxTCPPocoClient::threadedFunction(){
    
    while( isThreadRunning() ){
        
        // need to thread this bit...
        if(connected) {
            // 1. polls the socket for 5 seconds - blocks socket
            /*bool pollSuccess = socketStream->poll(pollTimeout,Poco::Net::Socket::SELECT_READ);
            mutex.lock();
            waitingMessage = pollSuccess;
            mutex.unlock();*/
            
            // 2. just check when bytes available - does not block socket
            int bytesAvailable = socketStream->available();
            if(bytesAvailable) {
                
                // 1. get the header
                // allocate a buffer to receive incoming message
                ofBuffer nextBuffer;
                nextBuffer.allocate(4 + 1); //+1
                bool getHeaderBytes = recvBytes(nextBuffer.getData(), nextBuffer.size());
                if(getHeaderBytes) {
                    
                    // 2. get main message
                    // convert bytes back into int by casting
                    int nextBufferSize = *(int *)nextBuffer.getData();
                    nextBuffer.allocate(nextBufferSize + 1);
                    bool getMessageBytes = recvBytes(nextBuffer.getData(), nextBuffer.size());
                    if(getMessageBytes) {
                        
                        // push received message into a queue
                        // there is no limit on the queue, so need to be careful as it is emptied externally with getRawBuffer()
                        mutex.lock();
                        receivedBuffers.push(nextBuffer);
                        waitingMessage = true;
                        mutex.unlock();
                    } else {
                        ofLog() << "Didn't receive main message!";
                    }
                    
                } else {                    
                    ofLog() << "Didn't receive header - everything might be out of order!";
                }
            } else {
                
                // sleep thread when no bytes available same as server
                sleep(sleepTime);
            }
        } else {
            
            // thread is not connected - shouldnet be in here
            sleep(sleepTime);
        }
    }

}



// receive / replys
//--------------------------------------------------------------
bool ofxTCPPocoClient::hasWaitingMessage() {
    return waitingMessage;
}



// eg. http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler
void ofxTCPPocoClient::getMessage(string& msg) {
    
    Poco::ScopedLock<ofMutex> lock(mutex);
    if(receivedBuffers.size() == 0) {
        ofLogError() << "* ofxTCPPocoClient getMessage error: no messages in queue";
        waitingMessage = false;
        return;
    }
    msg = receivedBuffers.front().getData();
    receivedBuffers.pop();
    if(receivedBuffers.size() == 0) {
        waitingMessage = false;
    }
}

void ofxTCPPocoClient::getRawBuffer(ofBuffer& buffer) {
    
    Poco::ScopedLock<ofMutex> lock(mutex);
    if(receivedBuffers.size() == 0) {
        ofLogError() << "* ofxTCPPocoClient getRawBuffer error: no messages in queue";
        waitingMessage = false;
        return;
    }
    buffer = receivedBuffers.front();
    receivedBuffers.pop();
    if(receivedBuffers.size() == 0) {
        waitingMessage = false;
    }
}



// threaded receive method (private)
//--------------------------------------------------------------
bool ofxTCPPocoClient::recvBytes(char* bytes, int size) {
    try {
        
        // 1. grab a bunch of bytes
        //socketStream->setKeepAlive(true);
        //int n = socketStream->receiveBytes(bytes, size);
        
        // 2. fill the buffer up to required size
        int nBytes = 0;
        int bytesReceived = 0;
        while (bytesReceived < size) {
            socketStream->setKeepAlive(true);
            nBytes = socketStream->receiveBytes(&bytes[bytesReceived], size-bytesReceived);
            if(nBytes <= 0) break;
            bytesReceived += nBytes;
        }
        
        if (nBytes <= 0) {
            ofLogError() << "* ofxTCPPocoClient incorrect receive bytes " << nBytes;
            return false;
        }
        
        return true;
    }
    catch (Poco::Exception& exc) {
        //Handle your network errors.
        ofLogError() << "* ofxTCPPocoClient recvBytes: " << exc.displayText();
        //socketStream->setKeepAlive(false);
        return false;
    }
}

// sending: blocking
bool ofxTCPPocoClient::sendBytes(const char* bytes, int size) {
    try {

        /*int nBytes = 0;
        int bytesSent = 0;
        while (bytesSent < size) {
            
            // when receiving socket exits- can crash during sendBytes. this keeps socket alive till at least after while loop.
            // then gets caught in exception error
            socketStream->setKeepAlive(true);
            
            // send all the bytes
            nBytes = socketStream->sendBytes(&bytes[bytesSent], size - bytesSent);
            if(nBytes <= 0) break;
            bytesSent += nBytes;
        }*/
        
        // when receiving socket exits- can crash during sendBytes. this keeps socket alive till at least after while loop.
        // then gets caught in exception error
        socketStream->setKeepAlive(true);
        int nBytes = socketStream->sendBytes(bytes, size);
        
        if (nBytes < size) {
            ofLogError() << "* ofxTCPPocoClient incorrect send bytes " << nBytes;
            return false;
        }
        
        return true;
        
    } catch (Poco::Exception& exc) {
        
        ofLogError() << "* ofxTCPPocoClient sendRawBuffer error: " << exc.displayText() << ". " << exc.code();
        return false;
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
