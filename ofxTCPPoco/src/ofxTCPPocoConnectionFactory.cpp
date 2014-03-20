#include "ofxTCPPocoConnectionFactory.h"


ofxTCPPocoConnectionFactory::ofxTCPPocoConnectionFactory() {
}

ofxTCPPocoConnectionFactory::~ofxTCPPocoConnectionFactory() {
    
    ofLog() << "ofxTCPPocoConnectionFactory deleted!";
    /*for(int i = 0; i < connectionHandlers.size(); i++) {
     ofRemoveListener(connectionHandlers[i]->closeEvent, this, &ofxTCPPocoConnectionFactory::onDisconnection);
     //delete connectionHandlers[i];
     }*/
    connectionHandlers.clear();
}

Poco::Net::TCPServerConnection* ofxTCPPocoConnectionFactory::createConnection(const Poco::Net::StreamSocket& socket)
{
    totalClientsCount++;
    ofLog() << "ofxTCPPocoConnectionFactory received new client conncetion";
    ofxTCPPocoConnectionHandler* handler = new ofxTCPPocoConnectionHandler(socket);
    handler->clientId = totalClientsCount;
    ofAddListener(handler->closeEvent, this, &ofxTCPPocoConnectionFactory::onDisconnection);
    connectionHandlers.push_back(handler);
    
    //ofEventArgs args;
    //ofNotifyEvent(connectionEvent, args);
    
    return handler;
}

void ofxTCPPocoConnectionFactory::onDisconnection(int &clientId){
    
    for(int i = 0; i < connectionHandlers.size(); i++) {
        if(clientId == connectionHandlers[i]->clientId) {
            ofRemoveListener(connectionHandlers[i]->closeEvent, this, &ofxTCPPocoConnectionFactory::onDisconnection);
            //delete connectionHandlers[i];
            connectionHandlers.erase(connectionHandlers.begin() + i);
            return;
        }
    }
}

vector<ofxTCPPocoConnectionHandler*>& ofxTCPPocoConnectionFactory::getConnectionHandlers() {
    return connectionHandlers;
}

int ofxTCPPocoConnectionFactory::getNumConnections() {
    return connectionHandlers.size();
}