
#include "ofxTCPPocoServer.h"


ofxTCPPocoServer::ofxTCPPocoServer() {
    server = NULL;
    serverSocket = NULL;
    connection = NULL;
}

ofxTCPPocoServer::~ofxTCPPocoServer() {
    ofLog() << "~ofxTCPPocoServer deleted";
    disconnect();
    
}

void ofxTCPPocoServer::disconnect() {
   
    /*if(serverSocket) {        
        try {
            serverSocket->setLinger(true, 0);
            //serverSocket->close();
        } catch (Poco::Exception& exc) {
            // server must be closed or the above crashes on exit
            ofLogError() << "* ~ofxTCPPocoServer failed to setLinger: " << exc.displayText();
        }
    }*/
    if(server) server->stop();
    
    delete server;
    server = NULL;
    delete serverSocket;
    serverSocket = NULL;
    //if(connection) delete connection;
    connection = NULL;
    waitingRequest = false;
}

void ofxTCPPocoServer::start(int port){
    
    this->port = port;
    
    // alt setup...
    serverSocket = new Poco::Net::ServerSocket(port);    
    connection = new ofxTCPPocoConnectionFactory();
    
    server = new Poco::Net::TCPServer(connection, *serverSocket);
    server->start();
    
    //serverSocket->setNoDelay(true);
}



void ofxTCPPocoServer::printServerInfo() {
    
    ofLog() << "server setup " << server->port();// << ", " << svs.address().toString();
    ofLog() << "connections: " << server->currentConnections();
    ofLog() << "total connections: " << server->totalConnections();
    ofLog() << "threads: " << server->currentThreads();
    ofLog() << "ip: " << serverSocket->address().toString();
}


int ofxTCPPocoServer::getNumClients() {
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
bool ofxTCPPocoServer::hasWaitingMessage(int clientId) {
    if(server->currentConnections()) {
        waitingRequest = connection->getConnectionHandlers()[clientId]->hasWaitingMessage();
    } else {
        waitingRequest = false;
    }
    return waitingRequest;
}

// hasWaitingRequest must be called prior to this
// non blocking - message/buffer should already exist in connection handler.
bool ofxTCPPocoServer::getMessage(int clientId, string& msg) {
    if(waitingRequest) {
        ofBuffer receiveBuffer;
        connection->getConnectionHandlers()[clientId]->getRawBuffer(receiveBuffer);
        //msg = ofxTCPPocoUtils::parseMessage(receiveBuffer, TCPPOCO_DELIMITER);
        msg = receiveBuffer.getData();
    }
    return waitingRequest;
}

bool ofxTCPPocoServer::getRawBuffer(int clientId, ofBuffer& buffer) {
    if(waitingRequest) {
        connection->getConnectionHandlers()[clientId]->getRawBuffer(buffer);
    }
    return waitingRequest;
}



// send - blocking
//--------------------------------------------------------------
bool ofxTCPPocoServer::sendMessage(int clientId, string msg) {
    
    // format string before sending
    //string formatted = ofxTCPPocoUtils::buildPaddedMessage(msg, TCPPOCO_DEFAULT_MSG_SIZE);
    //ofBuffer buffer(formatted);

    return sendRawBuffer(clientId, msg.data(), msg.size());
}

bool ofxTCPPocoServer::sendRawBuffer(int clientId, ofBuffer& buffer) {

    return sendRawBuffer(clientId, buffer.getData(), buffer.size());
}

bool ofxTCPPocoServer::sendRawBuffer(int clientId, const char* buffer, int size) {
    if(clientId < getNumClients()) {
        return connection->getConnectionHandlers()[clientId]->sendRawBuffer(buffer, size);
    } else {
        ofLogError() << "* Invalid clientId " << clientId << ", Connections: " << getNumClients();
    }
}

bool ofxTCPPocoServer::sendMessageToAll(string msg) {

    return sendRawBufferToAll(msg.data(), msg.size());
}

bool ofxTCPPocoServer::sendRawBufferToAll(ofBuffer& buffer) {
    
    return sendRawBufferToAll(buffer.getData(), buffer.size());
}

bool ofxTCPPocoServer::sendRawBufferToAll(const char* buffer, int size) {
    if(getNumClients() == 0) {
        ofLogError() << "* No clients to sent to";
        return false;
    }
    bool sent = true;
    for(int i = 0; i < connection->getNumConnections(); i++) {
        if(!sendRawBuffer(i, buffer, size)) {
            ofLogError() << "* Failed to send to client: " << i;
            sent = false;
        }
    }
    return sent;
}


