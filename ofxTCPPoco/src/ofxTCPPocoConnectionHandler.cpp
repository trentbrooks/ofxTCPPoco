#include "ofxTCPPocoConnectionHandler.h"
#include "ofxTCPPocoServer.h"



ofxTCPPocoConnectionHandler::ofxTCPPocoConnectionHandler(const Poco::Net::StreamSocket& client) : Poco::Net::TCPServerConnection(client) {
    
    defaultMessageSize = TCPPOCO_DEFAULT_MSG_SIZE; // smaller message size initially
    receiveBufferSize = defaultMessageSize;
    waitingMessage = false;
    waitingSend = false;
    clientId = -1;
}

ofxTCPPocoConnectionHandler::~ofxTCPPocoConnectionHandler() {
}


bool ofxTCPPocoConnectionHandler::hasWaitingMessage() {
    
    Poco::ScopedRWLock lock(mutex);
    return waitingMessage;
}

void ofxTCPPocoConnectionHandler::setReceiveBufferSize(int size) {
    
    ofLog() << "Setting new buffer size for server connections " << size;
    // this doesn't work because if the dumb receivebytes waiting when already allocated... shithouse!!!
    mutex.writeLock();
    receiveBufferSize = size;
    mutex.unlock();
}


void ofxTCPPocoConnectionHandler::setSendBuffer(ofBuffer& buffer) {
    
    
    /*mutex.writeLock();
    sendBuffer = buffer; // a reference
    waitingSend = true;
    ofLog() << "Ready to send a buffer " << buffer.size();
    mutex.unlock();*/
    
    sendBuffer = buffer; // a reference
    int bytesSent = socket().sendBytes(sendBuffer.getBinaryBuffer(), sendBuffer.size());
}

// best to process all the data in the run before calling the callback.
// setup to receive messages 1 at a time
void ofxTCPPocoConnectionHandler::run() {
    
    bool isOpen = true;
    Poco::Timespan timeOut(TCPPOCO_POLL_TIME,0); // polling timeout - how often to read
    while(isOpen) {

        // this is a read poll...
        if (socket().poll(timeOut,Poco::Net::Socket::SELECT_READ)){
            
            mutex.readLock();
            int nextBufferSize = receiveBufferSize;
            bool nextWaitingMessage = waitingMessage;
            /*bool nextWaitingSend = waitingSend;
            if(nextWaitingSend) {
                ofBuffer bufferToSend(sendBuffer);
            }*/
            mutex.unlock();
            
            // send messages
//            if(nextWaitingSend) {
//                
//                ofLog() << "Sending... " << sendBuffer.size();
//                int bytesSent = socket().sendBytes(sendBuffer.getBinaryBuffer(), sendBuffer.size());
//                ofLog() << "server sent message: " << bytesSent << " / " << sendBuffer.size();
//                
//                mutex.writeLock();
//                waitingSend = false;
//                mutex.unlock();
//            }
            
            
            // current message must be read one at a time (or pushed into a queue- TODO) before continuing to process further messages
            if(waitingMessage) continue;
            
            // receive messages
            int nBytes = -1;
            try {
                
                receiveBuffer.allocate(nextBufferSize + 1); // extra char for null terminated string. now buffer size == nextBufferSize
                
                int bytesReceived = 0;
                while (bytesReceived < nextBufferSize) {
                    nBytes = socket().receiveBytes(&receiveBuffer.getBinaryBuffer()[bytesReceived], nextBufferSize-bytesReceived);
                    if(nBytes == 0) break;
                    bytesReceived += nBytes;
                }
                
                mutex.writeLock();
                waitingMessage = true;
                mutex.unlock();
                
                // for every 'receive' need to send back a reply. eg "1"
                /*int bytesSent = socket().sendBytes(replyMessage.data(), replyMessage.size() + 1); // must add 1 for null terminated string
                ofLog() << "Server send reply bytes: " << replyMessage.size() + 1;
                
                ofLog() << receiveBuffer.size();
                if(callbackAdded) runCallback(receiveBuffer) ;//string(receiveBuffer.getText()));*/
            }
            catch (Poco::Exception& exc) {
                //Handle your network errors.
                ofLog() << "ofxTCPPocoConnection Error: " << exc.displayText();
                isOpen = false;
            }
            
            
            if (nBytes==0){
                isOpen = false;
                ofLog() << "ofxTCPPocoConnection Client closes connection!";
                ofNotifyEvent(closeEvent, clientId, this);
            }
        } else {
            
            // timeout occured. can now change data.
        }
    }

    ofLog() << "ofxTCPPocoConnection connection finished!";
}


