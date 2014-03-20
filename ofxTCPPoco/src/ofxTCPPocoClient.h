#pragma once

#include "ofMain.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "ofxTCPPocoUtils.h"


// http://stackoverflow.com/questions/16510205/multithread-tcp-server-with-poco-c-libraries
// http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler
// TODO: make it similar to ofxTCPClient so can drop in replace ofxNetwork.

//: public ofThread 
class ofxTCPPocoClient {
public:
    
    ofxTCPPocoClient();
    virtual ~ofxTCPPocoClient();
    
    void setup(string ipAddr = "127.0.0.1", int port = 1234);
    bool connected;
    
    // must be threaded for polling to receive a message
    //int sleepTime;
    //void threadedFunction();
    
    
    // clients usually send then receive
    void sendMessage(string msg);
    void sendRawBuffer(ofBuffer& buffer);
    void sendRawBuffer(char* buffer, int sendSize);
    
    // receive works by polling in the thread
    // don't need thread anymore - can now use socket.available()
    bool hasWaitingMessage();
    void receiveMessage(string& msg);
    void receiveRawBuffer(ofBuffer& buffer);
    void receiveRawBuffer(char* buffer, int receiveSize);
    
    
    
    void update();
    void draw();
    
    Poco::Net::SocketAddress* socketAddress;
    Poco::Net::StreamSocket* socketStream;
    
    void setConnectTimeout(int timeoutInSeconds);
    //void setReceiveTimeout(int timeoutInSeconds);
    //void setSendTimeout(int timeoutInSeconds);
    void setPollTimeout(int timeoutInSeconds);
    
protected:
    
    Poco::Timespan connectTimeout;
    //Poco::Timespan receiveTimeout;
    //Poco::Timespan sendTimeout;
    
    Poco::Timespan pollTimeout;
    
    bool waitingMessage;
};
