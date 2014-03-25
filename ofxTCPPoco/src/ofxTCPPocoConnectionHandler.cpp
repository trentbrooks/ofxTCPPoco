#include "ofxTCPPocoConnectionHandler.h"
#include "ofxTCPPocoServer.h"



ofxTCPPocoConnectionHandler::ofxTCPPocoConnectionHandler(const Poco::Net::StreamSocket& client) : Poco::Net::TCPServerConnection(client) {
    
    defaultMessageSize = TCPPOCO_DEFAULT_MSG_SIZE; // smaller message size initially
    receiveBufferSize = defaultMessageSize;
    waitingMessage = false; // only receive/process messages one at a time
    //messagesToSend = false;
    clientId = -1;
    //sleepTime = 16;
    markSocketForDelete = false;
    
    // socket options - avoids crash on exit when ios device disconnects: https://github.com/pocoproject/poco/issues/235
    socket().setOption(SOL_SOCKET, SO_NOSIGPIPE, 1);
}

ofxTCPPocoConnectionHandler::~ofxTCPPocoConnectionHandler() {
    ofLog() << "*** ofxTCPPocoConnectionHandler deleted!";
    ofNotifyEvent(closeEvent, clientId, this);
    //socket().setKeepAlive(false);
}



// for reading messages...
//--------------------------------------------------------------
bool ofxTCPPocoConnectionHandler::hasWaitingMessage() {
    
    Poco::ScopedLock<ofMutex> lock(mutex);
    return waitingMessage;
}

// assumes hasWaitingMessage() was called, and their is a message buffer in the queue
void ofxTCPPocoConnectionHandler::getRawBuffer(ofBuffer& buffer) {
    
    mutex.lock();
    buffer = receiveBuffer;
    waitingMessage = true;
//    buffer = receivedBuffers.front();
//    receivedBuffers.pop();
//    if(receivedBuffers.size() == 0) {
//        waitingMessage = false;
//    }
    mutex.unlock();
}

void ofxTCPPocoConnectionHandler::flush() {
    
    mutex.lock();
    receiveBuffer.clear();
    //receivedBuffers.empty();
    waitingMessage = false;
    mutex.unlock();
}

// atm the receive buffer size for the server is only small (TCPPOCO_DEFAULT_MSG_SIZE)
// if receiving a large buffer, eg. image, this needs to be set from the server.
// TODO: this needs to be set per message if don't know size of incoming message
void ofxTCPPocoConnectionHandler::setReceiveBufferSize(int size) {
    
    //ofLog() << "Setting new buffer size for server connections " << size;
    mutex.lock();
    receiveBufferSize = size;
    mutex.unlock();
}


// for writing messages...

//--------------------------------------------------------------
// blocking message sending
bool ofxTCPPocoConnectionHandler::sendRawBuffer(ofBuffer& buffer) {
    
    bool sent = false;
    mutex.lock();
    
    sent = ofxTCPPocoUtils::sendRawBytes(&socket(), buffer.getBinaryBuffer(), buffer.size());
    if(!sent) {
        
        // this avoids the server crash on client exit
        // will still crash when disconnecting an ios device unless ignore SIGPIPE socket().setOption(SOL_SOCKET, SO_NOSIGPIPE, 1);
        markSocketForDelete = true;
    }
    
    mutex.unlock();
    return sent;
}


/*void ofxTCPPocoConnectionHandler::queueRawBuffer(ofBuffer& buffer) {
    
    mutex.lock();
    sendBuffers.push(buffer);
    messagesToSend = true;
    mutex.unlock();
}

// same as getRawBuffer(), but for internal use by the sender thread
// gets next buffer from queue
void ofxTCPPocoConnectionHandler::getQueuedBuffer(ofBuffer& buffer) {
    
    mutex.lock();
    buffer = sendBuffers.front();
    sendBuffers.pop();
    if(sendBuffers.size() == 0) {
        messagesToSend = false;
    }
    mutex.unlock();
}*/



// thread loop
//--------------------------------------------------------------
void ofxTCPPocoConnectionHandler::run() {
    
    bool isOpen = true;
    //Poco::Timespan timeOut(TCPPOCO_POLL_TIME,0); // polling timeout - how often to read
    
    while(isOpen) {
        
        //int startTime = ofGetElapsedTimeMillis();
        // read messages when data is available (or a blocking alternative- socket().poll(timeOut,Poco::Net::Socket::SELECT_READ)
        //if(socket().available()) {
        //ofLog() << "...";
        // this needs to change- currently dont know how many bytes i will receive
        int bytesAvailable = socket().available();
        //ofLog() << bytesAvailable << " : " << ofGetFrameNum();
        //while(socket().available()) {
        if(bytesAvailable >= receiveBufferSize) {

            mutex.lock();
            int nextBufferSize = receiveBufferSize;//bytesAvailable; //socket().available(); //receiveBufferSize; // what to do if i get partial message? TODO
            mutex.unlock();
            
            // allocate a buffer to receive incoming message
            ofBuffer nextBuffer;
            nextBuffer.allocate(nextBufferSize + 1);
            //ofLog() << "allocated for receive: " << nextBufferSize << ", " << receiveBufferSize;//socket().available();
            
            int nBytes = -1;
            try {
                
                // fill the buffer up to required size (eg. receiveBufferSize)
                int bytesReceived = 0;
                while (bytesReceived < nextBufferSize) {
                    socket().setKeepAlive(true);
                    nBytes = socket().receiveBytes(&nextBuffer.getBinaryBuffer()[bytesReceived], nextBufferSize-bytesReceived);
                    if(nBytes == 0) break;
                    bytesReceived += nBytes;
                }
                
                // push received message into a queue
                // there is no limit on the queue, so need to be careful as it is emptied externally with getRawBuffer()
                mutex.lock();
                receiveBuffer = nextBuffer;
                //receivedBuffers.push(receiveBuffer);
                waitingMessage = true;
                mutex.unlock();
                
            }
            catch (Poco::Exception& exc) {
                //Handle your network errors.
                ofLogVerbose() << "ofxTCPPocoConnection ReceiveError: " << exc.displayText();
                socket().setKeepAlive(false);
                isOpen = false;
            }
            
            
            // client closed connection
            if (nBytes==0){
                isOpen = false;
                ofLogVerbose() << "ofxTCPPocoConnection Client closes connection!";
                //ofNotifyEvent(closeEvent, clientId, this);
            }
        }
        
        
        // can gracefully close connection if previously marked socket for delete
        if(markSocketForDelete) {
            markSocketForDelete = true;
            isOpen = false;
            socket().setKeepAlive(false);
            ofLogVerbose() << "ofxTCPPocoConnection closed connection during send!";
            //ofNotifyEvent(closeEvent, clientId, this);
        }
        // send messages from queue
        /*mutex.lock();
        bool hasMessagesToSend = messagesToSend;
        int totalMessagesToSend = sendBuffers.size();
        mutex.unlock();
        
        // TODO: check if socket was closed by client somehow
        if(hasMessagesToSend) {
            
            for(int i = 0; i < totalMessagesToSend; i++) {
                
                ofBuffer sendBuffer;
                getQueuedBuffer(sendBuffer);
                
                int nBytes = -1;
                try {
                    
                    // send all the bytes
                    int bytesSent = 0;
                    while (bytesSent < sendBuffer.size()) {
                        // when client exits- crashes at sendBytes. this keeps socket alive till at least after while loop.
                        // then gets caught in exception error
                        socket().setKeepAlive(true);
                        
                        nBytes = socket().sendBytes(&sendBuffer.getBinaryBuffer()[bytesSent], sendBuffer.size() - bytesSent);
                        if(nBytes == 0) break;
                        bytesSent += nBytes;
                    }
                    
                    
                    // message was sent
                    //ofLog() << "sent message from server..." << nBytes <<" / " << bytesSent;
                    
                } catch (Poco::Exception& exc) {
                    
                    //Handle your network errors.
                    ofLogVerbose() << "ofxTCPPocoConnection Send Error, closing connection: " << exc.displayText();
                    //ofLogVerbose() << "ofxTCPPocoConnection Client closes connection!";
                    isOpen = false;                    
                    ofNotifyEvent(closeEvent, clientId, this);
                    socket().setKeepAlive(false);
                }
            }
        }*/
        
        // thread loop complete
        
        // sleep the thread
        /*int processTime = ofGetElapsedTimeMillis() - startTime;
        int diff = sleepTime - processTime; //processTime- 16 = 60fps, 33 = 30fps
        if(diff < 1) diff = 1;
        
        //sleep(diff);
        ofSleepMillis(diff);*/
    }

    ofLogVerbose() << "ofxTCPPocoConnection connection finished!";
}


// thread safe getting of event - not working
ofEvent<int> ofxTCPPocoConnectionHandler::getCloseEvent() {
    Poco::ScopedLock<ofMutex> lock(mutex);
    return closeEvent;
}

