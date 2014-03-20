
#pragma  once

#include "ofMain.h"
#include "Poco/Net/Socket.h"
#include "Poco/Net/SocketStream.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/RWLock.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"
#include <string>

/*
 ofxTCPPocoUtils.
 - util functions for poco tcp server and client
 */


#define TCPPOCO_DEFAULT_MSG_SIZE 32
#define TCPPOCO_REPLY "1"
#define TCPPOCO_DELIMITER '|'
#define TCPPOCO_POLL_TIME 5


class ofxTCPPocoUtils  {
public:
    
    //ofxTCPPocoUtils() {};
    //virtual ~ofxTCPPocoUtils() {};
    
    
    // parse incoming request buffer and get header string
    // extracts the important bit only (everything up to the first " ")
    // must have " " as input delimiter for stringstream operator to work
    static string parseMessage(ofBuffer& input);
    
    // same as above, except input buffer needs to be delimited by a custom character. eg '|'
    static string parseMessage(ofBuffer& input, char delim);
    
    // construct string padded to required size
    // delimiter splits the message- left side is the data, right side is junk
    // eg. "ping xxxxxxxxjunkxxxxxxxx"
    // TODO: maybe have a temp buffer to store the extra bytes on the receiver instead of discarding?
    static string buildMessage(string& output, int fillSize);
    
    
    // construct outgoing request
    // buffer is padded to the required size (with junk)
    // delimiter " " splits the message- left side is the data, right side is junk
    // or have a temp buffer to store the extra bytes on the receiver? todo...
    // eg. "ping xxxxxxxxjunkxxxxxxxx"
    static bool sendRawString(Poco::Net::StreamSocket* clientSocket, string& message, int fillSize=TCPPOCO_DEFAULT_MSG_SIZE);
    
    static bool sendRawBuffer(Poco::Net::StreamSocket* clientSocket, char* buffer, int sendSize);
    
    
    // helper method when receiving a buffer for client- ensures whole buffer is full
    static bool receiveRawString(Poco::Net::StreamSocket* clientSocket, string& message, int receiveSize=TCPPOCO_DEFAULT_MSG_SIZE);
    
    // helper method when receiving a buffer for client- ensures whole buffer is full
    static bool receiveRawBuffer(Poco::Net::StreamSocket* clientSocket, char* buffer, int receiveSize);

};

