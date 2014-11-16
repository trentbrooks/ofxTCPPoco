#pragma once

#include "ofMain.h"
#include "ofMain.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"

/*
 ofxUDPPocoClient
 - sender
 
 TODO: add receiving + threading maybe
 */

class ofxUDPPocoClient {
public:
    
    ofxUDPPocoClient();
    virtual ~ofxUDPPocoClient();
    void disconnect();
    
    void connect(string ipAddr = "127.0.0.1", int port = 1234);
    bool connected;
    
    // send messages
    int sendMessage(string& msg);
    int sendRawBuffer(ofBuffer& buffer);
    int sendRawBuffer(char* buffer, int sendSize);
    
    // advanced- internal poco buffer sizes (best not to change this unless you need video or something)
    void setMaxSendSize(int size); // change pocos max send size- default 9216
    int getMaxSendSize();
    
protected:

    Poco::Net::SocketAddress* socketAddress;
    Poco::Net::DatagramSocket* socket;
    
    int maxBufferSendSize;

};
