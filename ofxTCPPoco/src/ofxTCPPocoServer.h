#pragma once

#include "ofMain.h"
#include "ofxTCPPocoConnectionHandler.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include "ofxTCPPocoUtils.h"
#include "ofxTCPPocoConnectionFactory.h"

// TODO: crash on client exit

class ofxTCPPocoServer {
public:
    
    ofxTCPPocoServer();
    virtual ~ofxTCPPocoServer();
    
    void setup(int port = 12345);

    // receive requests from clients
    bool hasWaitingRequest(int clientId);
    void setReceiveSize(int clientId, int size);
    bool receiveMessage(int clientId, string& msg);
    bool receiveRawBuffer(int clientId, ofBuffer& buffer);
    
    // send
    bool sendMessage(int clientId, string msg);
    bool sendRawBuffer(int clientId, ofBuffer& buffer);
    
    
    // server automatically pulls data from clients with ofxTCPPocoConnectionHandler run loop
    // no need to thread
    int getNumClients();
    
    //bool hasWaitingMessage(int clientId);
    //void getMessage(int clientId, string& msg);
    //void getRawBuffer(int clientId, ofBuffer& buffer);
    
    void printServerInfo();

    
protected:
    
    int port;
    
    // poco
    Poco::Net::ServerSocket* serverSocket;
    Poco::Net::TCPServer* server;
    ofxTCPPocoConnectionFactory* connection;
    
    bool waitingRequest; // temp var applied to any connection

};
