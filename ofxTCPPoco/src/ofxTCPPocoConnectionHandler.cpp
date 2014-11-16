#include "ofxTCPPocoConnectionHandler.h"
#include "ofxTCPPocoServer.h"



ofxTCPPocoConnectionHandler::ofxTCPPocoConnectionHandler(const Poco::Net::StreamSocket& client) : Poco::Net::TCPServerConnection(client) {
    
    receiveBufferSize = 256;
    waitingMessage = false; // only receive/process messages one at a time
    clientId = -1;
    sleepTime = 10;
    markSocketForDelete = false;
    isRunning = true;
    
    // socket options - avoids crash on exit when ios device disconnects: https://github.com/pocoproject/poco/issues/235
    //socket().setOption(SOL_SOCKET, SO_NOSIGPIPE, 1);
    //socket().setBlocking(false);
    
    //ofLog() << "Blocking: " << socket().getBlocking();
    //ofLog() << "Delay: " << socket().getNoDelay();
    
    // apparantly 'osx really needs this'?
    socket().setNoDelay(true);
    
}

ofxTCPPocoConnectionHandler::~ofxTCPPocoConnectionHandler() {
    
    ofLog() << "~ofxTCPPocoConnectionHandler deleted";
    ofNotifyEvent(closeEvent, clientId, this);
    
    try {
        socket().setLinger(true, 0);
        //socket().setKeepAlive(false);
    } catch (Poco::Exception& exc) {
        // server must be closed or the above crashes on exit
        ofLogError() << "* ~ofxTCPPocoConnectionHandler failed to setLinger: " << exc.displayText();
    }
    
    isRunning = false;
    flush();
}



// for reading messages...
//--------------------------------------------------------------
bool ofxTCPPocoConnectionHandler::hasWaitingMessage() {
    
    Poco::ScopedLock<ofMutex> lock(mutex);
    return waitingMessage;
}

// assumes hasWaitingMessage() was called, and their is a message buffer in the queue
void ofxTCPPocoConnectionHandler::getRawBuffer(ofBuffer& buffer) {
    
    Poco::ScopedLock<ofMutex> lock(mutex);
    if(receivedBuffers.size() == 0) {
        ofLogError() << "* ofxTCPPocoConnectionHandler getRawBuffer error: no messages in queue";
        waitingMessage = false;
        return;
    }
    buffer = receivedBuffers.front();
    receivedBuffers.pop();
    if(receivedBuffers.size() == 0) {
        waitingMessage = false;
    }
}

void ofxTCPPocoConnectionHandler::flush() {
    
    mutex.lock();
    receivedBuffers.empty();
    waitingMessage = false;
    mutex.unlock();
}

// this is more for custom framing - maybe remove?
// TODO: this needs to be set per message if don't know size of incoming message
void ofxTCPPocoConnectionHandler::setReceiveBufferSize(int size) {
    
    mutex.lock();
    receiveBufferSize = size;
    mutex.unlock();
}


// for writing messages...
//--------------------------------------------------------------
// blocking message sending
bool ofxTCPPocoConnectionHandler::sendRawBuffer(ofBuffer& buffer) {
    
    return sendRawBuffer(buffer.getBinaryBuffer(), buffer.size());
}

bool ofxTCPPocoConnectionHandler::sendRawBuffer(const char* buffer, int size) {

    Poco::ScopedLock<ofMutex> lock(mutex);
    
    // 1. send a header with the message size
    char* header = (char*)(&size); // convert int to bytes (4) by casting
    bool sentHeader = sendBytes(header, 4);
    if(sentHeader) {
        // 2. send the main message
        bool sentMessage = sendBytes(buffer, size);
        return sentMessage;
    }
    
    // this avoids the server crash on client exit
    // will still crash when disconnecting an ios device unless ignore SIGPIPE socket().setOption(SOL_SOCKET, SO_NOSIGPIPE, 1);
    markSocketForDelete = true;
    return false;
}





// thread loop - no sleep
//--------------------------------------------------------------
void ofxTCPPocoConnectionHandler::run() {
    
    while(isRunning) {
        
        /*Poco::Timespan timeOut(1,0);
        bool error = socket().poll(timeOut,Poco::Net::Socket::SELECT_ERROR);
        ofLog() << "Is error: " << error;*/
        
        // read messages when data is available (or a blocking alternative- socket().poll(timeOut,Poco::Net::Socket::SELECT_READ)
        // this needs to change- currently dont know how many bytes i will receive
        int bytesAvailable = socket().available(); // might have to lock/unlock this
        if(bytesAvailable) {
            
            mutex.lock();
            int nextBufferSize = receiveBufferSize;
            mutex.unlock();
            
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
                    markSocketForDelete = true;
                }
                
            } else {
                ofLog() << "Didn't receive header - everything might be out of order!";
                markSocketForDelete = true;
            }
            
//            //int nBytes = -1;
//            try {
//
//                // fill the buffer up to required size (eg. receiveBufferSize)?
//                /*int bytesReceived = 0;
//                while (bytesReceived < nextBufferSize) {
//                    socket().setKeepAlive(true);
//                    nBytes = socket().receiveBytes(&nextBuffer.getBinaryBuffer()[bytesReceived], nextBufferSize-bytesReceived);
//                    if(nBytes == 0) break;
//                    bytesReceived += nBytes;
//                }*/
//                
//                // or just grab whatever bytes available and save to queue
//                socket().setKeepAlive(true);
//                int n = socket().receiveBytes(nextBuffer.getBinaryBuffer(), nextBuffer.size());
//                
//                // there is no limit on the queue, so need to be careful as it is emptied externally with getRawBuffer()
//                mutex.lock();
//                receivedBuffers.push(nextBuffer);
//                waitingMessage = true;
//                mutex.unlock();
//                
//            }
//            catch (Poco::Exception& exc) {
//
//                ofLogVerbose() << "ofxTCPPocoConnection ReceiveError: " << exc.displayText();
//                socket().setKeepAlive(false);
//                isRunning = false;
//            }
            
            // client closed connection
            /*if (nBytes==0){
                isOpen = false;
                ofLogVerbose() << "ofxTCPPocoConnection Client closes connection!";
                //ofNotifyEvent(closeEvent, clientId, this);
            }*/
            
        } else {
            ofSleepMillis(sleepTime);
            //sleep(33);
        }
        
        
        // can gracefully close connection if previously marked socket for delete
        if(markSocketForDelete) {
            markSocketForDelete = false;
            isRunning = false;
            socket().setKeepAlive(false);
            //ofLog() << "ofxTCPPocoConnection closed connection during send!";
            //ofNotifyEvent(closeEvent, clientId, this);
        }
        
        
    }

    ofLog() << "ofxTCPPocoConnection connection thread finished!";
}



// threaded receive method (private)
//--------------------------------------------------------------
bool ofxTCPPocoConnectionHandler::recvBytes(char* bytes, int size) {
    try {
        
        // 1. grab a bunch of bytes
        //socketStream->setKeepAlive(true);
        //int n = socketStream->receiveBytes(bytes, size);
        
        // 2. fill the buffer up to required size
        int nBytes = 0;
        int bytesReceived = 0;
        while (bytesReceived < size) {
            socket().setKeepAlive(true);
            nBytes = socket().receiveBytes(&bytes[bytesReceived], size-bytesReceived);
            if(nBytes <= 0) break;
            bytesReceived += nBytes;
        }
        
        if (nBytes <= 0) {
            ofLogError() << "* ofxTCPPocoConnectionHandler incorrect receive bytes " << nBytes;
            //markSocketForDelete = true;
            return false;
        }
        
//        ofLog() << "received from server: " << nBytes << ", " << bytesReceived << ", " << size;
        return true;
    }
    catch (Poco::Exception& exc) {
        //Handle your network errors.
        ofLogError() << "* ofxTCPPocoConnectionHandler recvBytes: " << exc.displayText();
        //socket().setKeepAlive(false);
        //markSocketForDelete = true;
        return false;
    }
}

// sending: blocking
// might be doubling up here: https://github.com/pocoproject/poco/blob/develop/Net/src/StreamSocket.cpp
// https://github.com/pocoproject/poco/blob/develop/Net/src/StreamSocketImpl.cpp
// dont think i need the while loops for the send?
bool ofxTCPPocoConnectionHandler::sendBytes(const char* bytes, int size) {
    
    try {
        
        /*int nBytes = 0;
        int bytesSent = 0;
        while (bytesSent < size) {
            
            // when receiving socket exits- can crash during sendBytes. this keeps socket alive till at least after while loop.
            // then gets caught in exception error
            socket().setKeepAlive(true);
            
            // send all the bytes
            int nBytes = socket().sendBytes(&bytes[bytesSent], size - bytesSent);
            if(nBytes <= 0) break;
            bytesSent += nBytes;
        }*/
        
        // when receiving socket exits- can crash during sendBytes. this keeps socket alive till at least after while loop.
        // then gets caught in exception error
        socket().setKeepAlive(true);
        int nBytes = socket().sendBytes(bytes, size);
        if (nBytes < size) {
            ofLogError() << "* ofxTCPPocoConnectionHandler incorrect send bytes " << nBytes;
            //markSocketForDelete = true;
            return false;
        }
        
//        ofLog() << "sent from server: " << nBytes << ", " << bytesSent << ", " << size;
        return true;
        
    } catch (Poco::Exception& exc) {
        ofLogError() << "* ofxTCPPocoConnectionHandler sendRawBuffer: " << exc.displayText() << ". " << exc.code();
        //markSocketForDelete = true;
        return false;
    }
    
    return false;
}


// thread safe getting of event (copy) - not working
ofEvent<int> ofxTCPPocoConnectionHandler::getCloseEvent() {
    Poco::ScopedLock<ofMutex> lock(mutex);
    return closeEvent;
}

