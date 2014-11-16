#pragma once

#include "ofMain.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"


// http://stackoverflow.com/questions/16510205/multithread-tcp-server-with-poco-c-libraries
// http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler
// TODO: add timeouts to send and receive
// TODO: add update function which handles reconnection
// TODO: detect when server closes ('connected' needs to reset)


class ofxTCPPocoClient : public ofThread {
public:
    
    ofxTCPPocoClient();
    virtual ~ofxTCPPocoClient();
    
    void connect(string ipAddr = "127.0.0.1", int port = 1234);
    bool connected;
    
    // must be threaded for polling to receive a message
    int sleepTime;
    void threadedFunction();

   
    // sendine messages - blocking
    bool sendMessage(string& msg); // messages are padded to fill size
    bool sendRawBuffer(ofBuffer& buffer);
    bool sendRawBuffer(const char* buffer, int size);
    
    // receive works by polling in the thread
    bool hasWaitingMessage();
    void getMessage(string& msg);
    void getRawBuffer(ofBuffer& buffer); // pass in new/empty ofBuffer
    
    
    
    // the connect timeout only occurs on the first run atm
    void setConnectTimeout(int timeoutInSeconds);
    //void setReceiveTimeout(int timeoutInSeconds);
    //void setSendTimeout(int timeoutInSeconds);
    //void setPollTimeout(int timeoutInSeconds);
    
    void disconnect();
    /*void shutdown() {
        if(socketStream) {
            bool on;
            int s;
            socketStream->getLinger(on, s);
            ofLog() << "linger: " << on << ", " << s;
            socketStream->setLinger(true, 0);
            socketStream->setKeepAlive(false);
            //socketStream->shutdown();
            //socketStream->close();
        }
    }*/
    
protected:
    
    Poco::Net::SocketAddress* socketAddress;
    Poco::Net::StreamSocket* socketStream;
    
    Poco::Timespan connectTimeout;
    //Poco::Timespan receiveTimeout;
    //Poco::Timespan sendTimeout;
    
    //Poco::Timespan pollTimeout;
    
    // main receive/send functions
    bool recvBytes(char* bytes, int size);
    bool sendBytes(const char* bytes, int size);
    
    int maxReadTime;
    bool waitingMessage;
    
    queue<ofBuffer> receivedBuffers;
    

};
