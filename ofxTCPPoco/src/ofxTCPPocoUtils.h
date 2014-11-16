
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
 - send a string (in specific format) or char* buffer
 - receive a string (in specific format) or char* buffer
 */


#define TCPPOCO_DEFAULT_MSG_SIZE 256
#define TCPPOCO_HEADER_MSG_SIZE 4
#define TCPPOCO_REPLY "1"
#define TCPPOCO_DELIMITER '|'
#define TCPPOCO_POLL_TIME 5


class ofxTCPPocoUtils  {
public:
    
    // receiver helpers
    static bool receivePaddedMessage(Poco::Net::StreamSocket* clientSocket, string& message, int receiveSize);
    static bool receiveRawBytes(Poco::Net::StreamSocket* clientSocket, char* buffer, int receiveSize);
    
    // sender helpers
    static bool sendRawBytes(Poco::Net::StreamSocket* socket, const char* buffer, int sendSize);
    static bool sendPaddedMessage(Poco::Net::StreamSocket* socket, string& message, int fillSize);
    
    
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
    static string buildPaddedMessage(string& output, int fillSize);
    
    
    // about padded messages
    // buffer is padded to the required size (with junk)
    // delimiter "|" splits the message- left side is the data, right side is junk
    // or have a temp buffer to store the extra bytes on the receiver? todo...
    // eg. "ping|xxxxxxxxjunkxxxxxxxx"

};

