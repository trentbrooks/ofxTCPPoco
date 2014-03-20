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


class ofxTCPPocoServer {
public:
    
    ofxTCPPocoServer();
    virtual ~ofxTCPPocoServer();
    
    void setup(int port = 12345);

    // server automatically pulls data from clients with ofxTCPPocoConnectionHandler run loop
    // no need to thread
    int getNumClients();
    bool hasWaitingMessage(int clientId);
    void getMessage(int clientId, string& msg);
    void getRawBuffer(int clientId, ofBuffer& buffer);
    void getRawBuffer(int clientId, char* buffer, int& size);
    
    // send
    void sendMessage(int clientId, string msg);
    void sendRawBuffer(int clientId, ofBuffer& buffer);
    void sendRawBuffer(int clientId, char* buffer, int sendSize);
    
    
    
    void draw();
    void printServerInfo();
    
    //void setBufferSize(int size);
    
    // send/receive
    //void receiveRawBuffer(ofBuffer& buffer);
    //void receiveRawBuffer(char* buffer, int size);
    
    // events/callbacks
    //void onDataCallback(ofBuffer& data);
    //void onDataCallback2(ofBuffer data);
    //void onConnectionEstablished(ofEventArgs& args);
    
    
protected:
    
    int port;
    
    // poco
    Poco::Net::ServerSocket* serverSocket;
    Poco::Net::TCPServer* server;
    ofxTCPPocoConnectionFactory* connection;

};
