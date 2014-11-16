#pragma once

#include "ofMain.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include "ofxTCPPocoConnectionHandler.h"

// Provides a basic implementation of TCPServerConnectionFactory
// Same as the template, except keeps a reference to the connectionHandlers
class ofxTCPPocoConnectionFactory: public Poco::Net::TCPServerConnectionFactory {
public:
    
	ofxTCPPocoConnectionFactory();
	~ofxTCPPocoConnectionFactory();
    
    Poco::Net::TCPServerConnection* createConnection(const Poco::Net::StreamSocket& socket);
	
    void onDisconnection(int &clientId);
    
    int getNumConnections();    
    vector<ofxTCPPocoConnectionHandler*>& getConnectionHandlers();
    
    vector<ofxTCPPocoConnectionHandler*> connectionHandlers; // # of clients
    //ofEvent<ofEventArgs> connectionEvent;
    int totalClientsCount;
};
