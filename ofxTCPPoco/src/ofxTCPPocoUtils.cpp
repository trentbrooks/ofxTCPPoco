
#include "ofxTCPPocoUtils.h"


// message parsing, padding with delim
//--------------------------------------------------------------
string ofxTCPPocoUtils::parseMessage(ofBuffer& input) {
    
    stringstream ss;
    ss << input; // input buffer inserted into stringstream
    string output;
    ss >> output; // extract first part of message only (up until " ")
    return output;
}

// same as above, except input buffer needs to be delimited by a custom character. eg '|'
string ofxTCPPocoUtils::parseMessage(ofBuffer& input, char delim) {
    
    stringstream ss;
    ss << input; // input buffer inserted into stringstream
    string output;
    getline(ss, output, delim); // only want the message up until '|' delimiter
    return output;
}

string ofxTCPPocoUtils::buildPaddedMessage(string& output, int fillSize) {
    
    stringstream ss;
    ss << output << TCPPOCO_DELIMITER; // must have the '|' on the end for stringstream delimeter
    string buffer;
    buffer.assign(ss.str().c_str(),fillSize);
    return buffer;
}


// receiving helpers
//--------------------------------------------------------------
bool ofxTCPPocoUtils::receivePaddedMessage(Poco::Net::StreamSocket* socket, string& message, int receiveSize) {
    
    ofBuffer receiveBuffer;
    receiveBuffer.allocate(receiveSize + 1);
    bool received = ofxTCPPocoUtils::receiveRawBytes(socket, receiveBuffer.getBinaryBuffer(), receiveBuffer.size());
    if(received) {
        // parse message / header
        message = ofxTCPPocoUtils::parseMessage(receiveBuffer, TCPPOCO_DELIMITER);
        return true;
    }
    return false;
}

bool ofxTCPPocoUtils::receiveRawBytes(Poco::Net::StreamSocket* socket, char* buffer, int receiveSize) {
    
    try {
        // send all the bytes
        int bytesReceived = 0;
        while (bytesReceived < receiveSize) {
            
            // when receiving socket exits- can crash during sendBytes. this keeps socket alive till at least after while loop.
            // then gets caught in exception error
            socket->setKeepAlive(true);
            
            // receive all the bytes
            int result = socket->receiveBytes(&buffer[bytesReceived], receiveSize-bytesReceived);
            //if(result == 0) return false; // not sure about this, not handling this case yet.
            bytesReceived += result;
        }
        
        return true;
        
    } catch (Poco::Exception& exc) {
        
        ofLog() << "ofxTCPPocoUtils receiveRawBuffer error: " << exc.displayText();
        return false;
    }
    
    return true;
}



// sending helpers
//--------------------------------------------------------------
bool ofxTCPPocoUtils::sendPaddedMessage(Poco::Net::StreamSocket* socket, string& message, int fillSize) {
    
    // pad the message string to required fill size, receiving socket must use the same size when receiving bytes.
    string paddedMessage = ofxTCPPocoUtils::buildPaddedMessage(message, fillSize);
    ofBuffer sendBuffer(paddedMessage);
    
    bool success = sendRawBytes(socket, sendBuffer.getBinaryBuffer(), fillSize);
    return success;
}

bool ofxTCPPocoUtils::sendRawBytes(Poco::Net::StreamSocket* socket, char* buffer, int sendSize) {
    
    // still problems with abrupt disconnection of client during server send
    // doesn't happen with localhost, but does with ios device on wifi

    
    try {
        // send all the bytes
        int bytesSent = 0;
        
        while (bytesSent < sendSize) {
            
            // when receiving socket exits- can crash during sendBytes. this keeps socket alive till at least after while loop.
            // then gets caught in exception error
            //ofLog() << socket->getKeepAlive() << ", " << bytesSent << " / " << sendSize;
            socket->setKeepAlive(true);
            //socket->setLinger(false, 0);
            
            // send all the bytes
            // getting a crash here on thread 2 when server is sending data
            int result = socket->sendBytes(&buffer[bytesSent], sendSize - bytesSent);
            //ofLog() << result << " / " << bytesSent << " / " << sendSize;
            //if(result == 0) return false; // not sure about this, not handling this case yet.
            bytesSent += result;
        }
        
        return true;
        
        
    } catch (Poco::Exception& exc) {
        
        //cout << exc.displayText() << endl;
        ofLog() << "ofxTCPPocoUtils sendRawBuffer error: " << exc.displayText() << ". " << exc.code();
        //ofLog() << socket->getKeepAlive();
        //ofLog() << socket->get();
        //socket->shutdownSend();
        //if(exc.code() != 0)
        //socket->setKeepAlive(false);
        return false;
    }
    
    return false;
}
