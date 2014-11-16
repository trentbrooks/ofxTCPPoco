#pragma once
#include "ofMain.h"
#include "Poco/Net/DatagramSocket.h"
#include "Poco/Net/SocketAddress.h"


/*
 ofxUDPPocoServer
 - receiver
 - threaded + blocking

 TODO: add sending 
 - can send data if know socketaddress with sendTo(), but because socket is blocking will need to make this useable
 */

class ofxUDPPocoServer : public ofThread {
public:
    
    ofxUDPPocoServer();
    virtual ~ofxUDPPocoServer();
    void disconnect();
    
    void bind(int port = 12345); // bind
    bool connected;
    
    void threadedFunction();

    // receive
    bool hasWaitingMessage();
    void setReceiveSize(int size);
    void getRawBuffer(ofBuffer& buffer);
    void getMessage(string& message);

    
protected:
    
    int sleepTime;
    int receiveSize;
    
    ofBuffer receiveBuffer;
    bool waitingRequest; // temp var applied to any connection
    
    Poco::Net::SocketAddress* socketAddress;
    Poco::Net::DatagramSocket* socket;

};
