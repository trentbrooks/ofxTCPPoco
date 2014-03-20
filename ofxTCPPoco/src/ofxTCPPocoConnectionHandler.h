#pragma once

#ifdef _WIN32
#include <functional> // tr1 deprecated in VS
#else
#include <tr1/functional>
#endif
#include "ofMain.h"
#include "Poco/RWLock.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include "ofxTCPPocoUtils.h"

// dealing with poco's computer science style coding is doing my head in.


// ofxTCPPocoConnectionHandler.
// following poco TCPServerConnection interface
// this object gets generated by a TCPServerConnectionFactory
// has a callback function to interface back to ofxTCPPocoServer
// handles receving messages from clients - a new ofxTCPPocoConnectionHandler is created for each client
class ofxTCPPocoConnectionHandler : public Poco::Net::TCPServerConnection {
public:
    
    ofxTCPPocoConnectionHandler(const Poco::Net::StreamSocket& client);// : Poco::Net::TCPServerConnection(client);
    ~ofxTCPPocoConnectionHandler();
    
    int clientId; // just the overall total amount of clients created over duration of app
    ofEvent<int> closeEvent;
    
    Poco::RWLock mutex; // read/write lock: http://www.cs.bgu.ac.il/~spl111/PracticalSession09_-_Threads_Using_POCO
    void run(); // async loop
    
    bool hasWaitingMessage();
    bool waitingMessage;
    
    // run callback
    /*std::tr1::function<void(ofBuffer)> runCallback;
    bool callbackAdded;
    
    template <class T>
    void addCallback(T  * listener, void (T::*listenerMethod)(ofBuffer&)) {
        runCallback = std::tr1::bind(listenerMethod, listener, std::tr1::placeholders::_1);
        callbackAdded = true;
    };*/
    int defaultMessageSize;
    
    // receive
    ofBuffer receiveBuffer;
    int receiveBufferSize;
    void setReceiveBufferSize(int size); // this need to be configured per message
    
    // send
    ofBuffer sendBuffer; // preallocated
    void setSendBuffer(ofBuffer& buffer); //
    bool waitingSend;

};



// ofxTCPPocoConnectionEventArgs.
// ofEvent dispatched when the connection is made.
/*class ofxTCPPocoConnectionEventArgs : public ofEventArgs {
public:
    ofxTCPPocoConnectionEventArgs() {};
    ofxTCPPocoConnectionEventArgs(ofxTCPPocoConnectionHandler *handler) { target = handler; }
    ofxTCPPocoConnectionHandler* target;
};*/

