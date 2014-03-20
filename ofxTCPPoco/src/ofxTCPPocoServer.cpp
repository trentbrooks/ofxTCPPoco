
#include "ofxTCPPocoServer.h"


ofxTCPPocoServer::ofxTCPPocoServer() {
    
}

ofxTCPPocoServer::~ofxTCPPocoServer() {
    
    server->stop();
    serverSocket->close();
    
    delete server;
    server = NULL;
    delete serverSocket;
    serverSocket = NULL;
    //if(connection) delete connection;
    connection = NULL;
    
}

void ofxTCPPocoServer::setup(int port){
    
    this->port = port;
    //Create a server socket to listen.
    /*Poco::Net::ServerSocket svs(port);
     
     //Configure some server params.
     Poco::Net::TCPServerParams* pParams = new Poco::Net::TCPServerParams();
     pParams->setMaxThreads(4);
     pParams->setMaxQueued(4);
     pParams->setThreadIdleTime(10);
     
     //Create your server
     Poco::Net::TCPServer myServer(new Poco::Net::TCPServerConnectionFactoryImpl<ofxTCPPocoConnection>(), svs, pParams);
     myServer.start();*/
    
    // alt setup...
    serverSocket = new Poco::Net::ServerSocket(port);    
    connection = new ofxTCPPocoConnectionFactory(); //Poco::Net::TCPServerConnectionFactoryImpl<ofxTCPPocoConnection>();
    //ofAddListener(connection->connectionEvent, this, &ofxTCPPocoServer::onConnectionEstablished);
    server = new Poco::Net::TCPServer(connection, *serverSocket);
    server->start();

}



void ofxTCPPocoServer::printServerInfo() {
    
    ofLog() << "server setup " << server->port();// << ", " << svs.address().toString();
    ofLog() << "connections: " << server->currentConnections();
    ofLog() << "total connections: " << server->totalConnections();
    ofLog() << "threads: " << server->currentThreads();
}


int ofxTCPPocoServer::getNumClients() {
    //return connection->getNumConnections();
    return server->currentConnections();
}


// receive (automatically pulled from ofxTCPPocoConnectionHandlers)
//--------------------------------------------------------------
// parses the saved message from the connection handler, allows further messages to be received
// TODO: error checking, timeout, hasWaitingMessage check
void ofxTCPPocoServer::getMessage(int clientId, string& msg) {
    
    msg = ofxTCPPocoUtils::parseMessage(connection->getConnectionHandlers()[clientId]->receiveBuffer, TCPPOCO_DELIMITER);
    connection->getConnectionHandlers()[clientId]->waitingMessage = false;
}

void ofxTCPPocoServer::getRawBuffer(int clientId, ofBuffer& buffer) {
    
    // copy the saved data
    buffer = connection->getConnectionHandlers()[clientId]->receiveBuffer;
    connection->getConnectionHandlers()[clientId]->waitingMessage = false;
}

void ofxTCPPocoServer::getRawBuffer(int clientId, char* buffer, int& size) {
    
    // need to change this to make a copy of the char* array
    // currently just returning the address of the ofBuffers internal char array
    buffer = connection->getConnectionHandlers()[clientId]->receiveBuffer.getBinaryBuffer();
    size = connection->getConnectionHandlers()[clientId]->receiveBuffer.size();
    //buffer = new char[size];
    //memcpy(buffer, handlerRef->receiveBuffer.getBinaryBuffer(), size);
    connection->getConnectionHandlers()[clientId]->waitingMessage = false;
}


// send
//--------------------------------------------------------------
void ofxTCPPocoServer::sendMessage(int clientId, string msg) {
    
    /*if(connected) {
        bool result = ofxTCPPocoUtils::sendRawString(socketStream, msg, TCPPOCO_DEFAULT_MSG_SIZE);
        ofLog() << "message sent..." << result;
    }*/
    
    //Poco::Net::ServerSocket* serverSocket;
    //Poco::Net::TCPServer* server;
    //ofxTCPPocoConnectionFactory* connection;
    
    //serverSocket->get
    
    //Poco::Net::StreamSocket& socket= connection->getConnectionHandlers()[clientId]->socket();
    
    // sending messages from server need to be pushed into connection handler
    
    // format string before sending
    string formatted = ofxTCPPocoUtils::buildMessage(msg, TCPPOCO_DEFAULT_MSG_SIZE);
    ofBuffer buffer(formatted);
    connection->getConnectionHandlers()[clientId]->setSendBuffer(buffer);
}

void ofxTCPPocoServer::sendRawBuffer(int clientId, ofBuffer& buffer) {
    //sendRawBuffer(clientId, buffer.getBinaryBuffer(), buffer.size());
    connection->getConnectionHandlers()[clientId]->setSendBuffer(buffer);
}

void ofxTCPPocoServer::sendRawBuffer(int clientId, char* buffer, int sendSize) {
    
    /*if(connected) {
        bool result = ofxTCPPocoUtils::sendRawBuffer(socketStream, buffer, sendSize);
        ofLog() << "buffer sent..." << result;
    }*/
    
    
    //setSendBuffer
}


// need to create an event for ofxTCPPocoConnectionHandler to be created - and then callback added.
/*void ofxTCPPocoServer::onConnectionEstablished(ofEventArgs& args) {
    
    ofLog() << "ofxTCPPocoServer received new client conncetion";
}*/


// TODO: timeout
// adjusted for multiple clients
bool ofxTCPPocoServer::hasWaitingMessage(int clientId) {
    
    if(server->currentConnections()) {
        if(connection->getConnectionHandlers()[clientId]->hasWaitingMessage()) {
            return true;
        }
    }
    
    return false;
}



void ofxTCPPocoServer::draw() {
    
}

