
#include "ofxUDPPocoClient.h"


ofxUDPPocoClient::ofxUDPPocoClient() {    
    connected = false;
}

ofxUDPPocoClient::~ofxUDPPocoClient() {
    disconnect();
}

void ofxUDPPocoClient::disconnect() {
    
    connected = false;
    if(socketAddress) delete socketAddress;
    if(socket) delete socket;
}

void ofxUDPPocoClient::connect(string ipAddr, int port) {

    // setup tcp poco client
    socketAddress = new Poco::Net::SocketAddress(ipAddr, port);
    socket = new Poco::Net::DatagramSocket(Poco::Net::IPAddress::IPv4);
    
    try {
        
        // client must connect to server
        socket->connect(*socketAddress);
        connected = true;
        ofLog() << "ofxUDPPocoClient connected";
        ofLog() << "Max receive size: " << socket->getReceiveBufferSize();
        ofLog() << "Max send size: " << socket->getSendBufferSize(); // 9216
        
        
    } catch (Poco::Exception& exc) {
        
        disconnect();        
        ofLog() << "ofxUDPPocoClient Error: could not create socket- " << exc.displayText();
    }
}



// send
//--------------------------------------------------------------
int ofxUDPPocoClient::sendMessage(string& msg) {
    
    if(connected) {
        try {
            int sent = socket->sendBytes(msg.data(), msg.length());
            //int sent = socket->sendTo(msg.data(), msg.length(), *socketAddress);
            return sent;
        } catch (Poco::Exception& exc) {
            ofLog() << "ofxUDPPocoClient sendMessage error: " << exc.displayText();
            return 0;
        }
    }
    return 0;
}

int ofxUDPPocoClient::sendRawBuffer(ofBuffer& buffer) {
    return sendRawBuffer(buffer.getData(), buffer.size());
}

int ofxUDPPocoClient::sendRawBuffer(char* buffer, int sendSize) {
    
    if(connected) {
        try {
            int sent = socket->sendBytes(buffer, sendSize);
            //int sent = socket->sendTo(buffer, sendSize, *socketAddress);
            return sent;
        } catch (Poco::Exception& exc) {
            ofLog() << "ofxUDPPocoClient sendRawBuffer error: " << exc.displayText();
            return 0;
        }
    }
    return 0;
}

// advanced- internal poco buffer sizes
//--------------------------------------------------------------
void ofxUDPPocoClient::setMaxSendSize(int size) {
    if(connected) {
        
        // advanced: change pocos max internal buffer send size- default send is 9216
        socket->setSendBufferSize(size);
        ofLog() << "Max send size: " << socket->getSendBufferSize();
    }
}

int ofxUDPPocoClient::getMaxSendSize() {
    if(connected) {
        return socket->getSendBufferSize();
    }
    return 0;
}
