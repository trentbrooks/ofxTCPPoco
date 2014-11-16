
#include "ofxUDPPocoServer.h"


ofxUDPPocoServer::ofxUDPPocoServer() {
    receiveSize = 256;
    sleepTime = 16;
    connected = false;
    waitingRequest = false;
}

ofxUDPPocoServer::~ofxUDPPocoServer() {
    disconnect();
}

void ofxUDPPocoServer::disconnect() {
    connected = false;
    if(socketAddress) delete socketAddress;
    if(socket) delete socket;
}

void ofxUDPPocoServer::bind(int port){
    
    if(connected) disconnect();
    
    // setup udp poco server
    socketAddress = new Poco::Net::SocketAddress(Poco::Net::IPAddress(), port);
    //socket = new Poco::Net::DatagramSocket(Poco::Net::IPAddress::IPv4); // not binded
    socket = new Poco::Net::DatagramSocket(*socketAddress); // automatically binds to socket
    
    connected = true;
    ofLog() << "Max receive size: " << socket->getReceiveBufferSize();
    ofLog() << "Max send size: " << socket->getSendBufferSize();
    
    
    // a server must be bound- and can only send to a defined socket address
    //socket->connect(*socketAddress);
    //socket->bind(*socketAddress, true);
    //socket->sendBytes("hello", 5);
    //socket->sendTo("hello", 5, *socketAddress);
    
    
    if(!isThreadRunning()) startThread();
}



// thread
//--------------------------------------------------------------
void ofxUDPPocoServer::threadedFunction(){

    while( isThreadRunning() ){

        if(connected) {
            
            // currently only setup for receiving
            // receive message - blocks thread until message
            try {
                
                // receive
                ofBuffer buffer;
                buffer.allocate(receiveSize);
                Poco::Net::SocketAddress sender;
                int n = socket->receiveFrom(buffer.getData(), buffer.size(), sender);
                
                // who sent message (sender)
                //ofLog() << "Received message from: " << sender.toString() << ", size: " << n;
                //ofLog() << "Message: " << buffer.getData();
                
                // old receive
                //buffer.getData()[n] = '\0';
                //char buffer[UDPPOCO_DEFAULT_MSG_SIZE];
                //int n = socket->receiveFrom(buffer.getData(), sizeof(buffer)-1, sender);
                
                // copy/replace buffer / or push into queue
                // TODO: push messages into a queue
                mutex.lock();
                receiveBuffer = buffer;
                waitingRequest = true;
                mutex.unlock();
                
                // test: send a message back to sender- works
                //int sent = socket->sendTo("hello", 5, sender);
                //ofLog() << "Sent back: " << sent;

            } catch (Poco::Exception& exc) {
                ofLog() << "ofxUDPPocoServer receive error: " << exc.displayText();
                return 0;
            }
        }

        sleep(sleepTime);
    }

}


// receive
//--------------------------------------------------------------
void ofxUDPPocoServer::setReceiveSize(int size) {
    Poco::ScopedLock<ofMutex> lock(mutex);
    receiveSize = size;
}

bool ofxUDPPocoServer::hasWaitingMessage() {
    Poco::ScopedLock<ofMutex> lock(mutex);
    return waitingRequest;
}

// currently just grabs the last available message
void ofxUDPPocoServer::getRawBuffer(ofBuffer& buffer) {
    mutex.lock();
    buffer = receiveBuffer;
    waitingRequest = false;
    mutex.unlock();
}

void ofxUDPPocoServer::getMessage(string& message) {
    mutex.lock();
    message = receiveBuffer.getData();
    waitingRequest = false;
    mutex.unlock();
}


// advanced- internal poco buffer sizes
//--------------------------------------------------------------
/*
 // advanced: change pocos max internal buffer receive size- default send is 9216 if smaller than current
 if(socket->getReceiveBufferSize() < size) {
 socket->setReceiveBufferSize(size);
 }
 */

