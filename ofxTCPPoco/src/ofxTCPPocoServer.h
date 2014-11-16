#pragma once

#include "ofMain.h"
#include "ofxTCPPocoConnectionHandler.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include "ofxTCPPocoConnectionFactory.h"


class ofxTCPPocoServer {
public:
    
    ofxTCPPocoServer();
    virtual ~ofxTCPPocoServer();
    
    void start(int port = 12345);

    // receive requests from clients
    bool hasWaitingMessage(int clientId);
    void setReceiveSize(int clientId, int size);
    bool getMessage(int clientId, string& msg);
    bool getRawBuffer(int clientId, ofBuffer& buffer);
    
    // send
    bool sendMessageToAll(string msg);
    bool sendRawBufferToAll(ofBuffer& buffer);
    bool sendRawBufferToAll(const char* buffer, int size);
    bool sendMessage(int clientId, string msg);
    bool sendRawBuffer(int clientId, ofBuffer& buffer);
    bool sendRawBuffer(int clientId, const char* buffer, int size);
    
    // server automatically pulls data from clients with ofxTCPPocoConnectionHandler run loop
    // no need to thread
    int getNumClients();
    
    void printServerInfo();
    
    void disconnect();
    
protected:
    
    int port;
    
    Poco::Net::ServerSocket* serverSocket;
    Poco::Net::TCPServer* server;
    ofxTCPPocoConnectionFactory* connection;
    
    bool waitingRequest; // temp var applied to any connection

};
