
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
    waitingRequest = false;
}

void ofxTCPPocoServer::setup(int port){
    
    this->port = port;
    
    // alt setup...
    serverSocket = new Poco::Net::ServerSocket(port);    
    connection = new ofxTCPPocoConnectionFactory(); //Poco::Net::TCPServerConnectionFactoryImpl<ofxTCPPocoConnection>();
    
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


// receive- non blocking (automatically pulled from ofxTCPPocoConnectionHandlers)
//--------------------------------------------------------------
// receive size needs to be updated if the server is receiving anything larger than TCPPOCO_DEFAULT_MSG_SIZE
// this need to be configured per message for each client connection handler
void ofxTCPPocoServer::setReceiveSize(int clientId, int size) {
    connection->getConnectionHandlers()[clientId]->setReceiveBufferSize(size);
}

// shoudl also store the boolean - as receiveMessage is expected to be called directly after this
// avoid locking the mutex in the connection thread twice
bool ofxTCPPocoServer::hasWaitingRequest(int clientId) {
    if(server->currentConnections()) {
        waitingRequest = connection->getConnectionHandlers()[clientId]->hasWaitingMessage();
    } else {
        waitingRequest = false;
    }
    return waitingRequest;
}

// hasWaitingRequest must be called prior to this
// non blocking - message/buffer should already exist in connection handler.
bool ofxTCPPocoServer::receiveMessage(int clientId, string& msg) {
    
    if(waitingRequest) {
        ofBuffer receiveBuffer;
        connection->getConnectionHandlers()[clientId]->getRawBuffer(receiveBuffer);
        msg = ofxTCPPocoUtils::parseMessage(receiveBuffer, TCPPOCO_DELIMITER);
    }
    return waitingRequest;
}

bool ofxTCPPocoServer::receiveRawBuffer(int clientId, ofBuffer& buffer) {
    
    if(waitingRequest) {
        connection->getConnectionHandlers()[clientId]->getRawBuffer(buffer);
    }
    return waitingRequest;
}




// send - blocking
//--------------------------------------------------------------
bool ofxTCPPocoServer::sendMessage(int clientId, string msg) {
    
    // format string before sending
    string formatted = ofxTCPPocoUtils::buildPaddedMessage(msg, TCPPOCO_DEFAULT_MSG_SIZE);
    ofBuffer buffer(formatted);

    // this call is blocking, not adding to queue anymore
    return connection->getConnectionHandlers()[clientId]->sendRawBuffer(buffer);
    
}

bool ofxTCPPocoServer::sendRawBuffer(int clientId, ofBuffer& buffer) {
    
    return connection->getConnectionHandlers()[clientId]->sendRawBuffer(buffer);
}



