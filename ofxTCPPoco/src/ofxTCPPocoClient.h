#pragma once

#include "ofMain.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "ofxTCPPocoUtils.h"


// http://stackoverflow.com/questions/16510205/multithread-tcp-server-with-poco-c-libraries
// http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler
// TODO: make threaded like the server's connection handler- queueing, etc.
// TODO: add timeouts to send and receive
// TODO: add update function which handles reconnection

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
   
    // messages must be the correct size on the receiving end
    bool sendMessage(string& msg, int fillSize=TCPPOCO_DEFAULT_MSG_SIZE); // messages are padded to fill size
    void sendRawBuffer(ofBuffer& buffer);
    void sendRawBuffer(char* buffer, int sendSize);
    
    // receive works by polling in the thread
    // don't need thread anymore - can now use socket.available()
    bool receiveMessage(string& msg, int receiveSize=TCPPOCO_DEFAULT_MSG_SIZE); // blocking
    bool receiveRawBuffer(ofBuffer& buffer, int receiveSize); // pass in new/empty ofBuffer
    
    
    // the connect timeout only occurs on the first run atm
    void setConnectTimeout(int timeoutInSeconds);
    //void setReceiveTimeout(int timeoutInSeconds);
    //void setSendTimeout(int timeoutInSeconds);
    void setPollTimeout(int timeoutInSeconds);
    
protected:
    
    Poco::Net::SocketAddress* socketAddress;
    Poco::Net::StreamSocket* socketStream;
    
    Poco::Timespan connectTimeout;
    //Poco::Timespan receiveTimeout;
    //Poco::Timespan sendTimeout;
    
    Poco::Timespan pollTimeout;

};
