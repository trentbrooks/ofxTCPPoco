
#include "ofxTCPPocoUtils.h"


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
    getline(ss, output, delim);
    return output;
}

string ofxTCPPocoUtils::buildMessage(string& output, int fillSize) {
    
    stringstream ss;
    ss << output << TCPPOCO_DELIMITER; // must have the '|' on the end for stringstream delimeter
    string buffer;
    buffer.assign(ss.str().c_str(),fillSize);
    return buffer;
}

bool ofxTCPPocoUtils::sendRawString(Poco::Net::StreamSocket* clientSocket, string& message, int fillSize) {
    
    // pad the message string to required fill size, client must use the same size for receiving.
    string buffer = ofxTCPPocoUtils::buildMessage(message, fillSize);
    
    // add to queue or send directly?
    // need to send all the bytes
    int bytesSent = clientSocket->sendBytes(buffer.data(), buffer.size());
    bool success = (bytesSent == fillSize);
    
    // if buffer sent is very large, then it says sent- but the receiver has not got everything yet?
    
    // need to add polling?
    // once message is sent - get a reply back from server. eg "1"
    //char replyMessage[2]; // allow extra byte for null terminated string
    //int replyBytes = clientSocket->receiveBytes(replyMessage, 2);
    //ofLog() << "Client received reply: " << replyMessage << ", " << replyBytes;
    //bool success = (replyBytes > 0);
    
    return success;
}

bool ofxTCPPocoUtils::sendRawBuffer(Poco::Net::StreamSocket* clientSocket, char* buffer, int sendSize) {
    
    // add to queue or send directly?
    int bytesSent = clientSocket->sendBytes(buffer, sendSize);
    bool success = (bytesSent == sendSize);
    
    // once message is sent - get a reply back from server. eg "1"
    //char replyMessage[2]; // allow extra byte for null terminated string
    //int replyBytes = clientSocket->receiveBytes(replyMessage, 2);
    //ofLog() << "Client received reply: " << replyMessage << ", " << replyBytes;
    //bool success = (replyBytes > 0);
    
    return success;
}

bool ofxTCPPocoUtils::receiveRawString(Poco::Net::StreamSocket* clientSocket, string& message, int receiveSize) {
    
    ofBuffer receiveBuffer;
    receiveBuffer.allocate(receiveSize + 1);
    bool received = ofxTCPPocoUtils::receiveRawBuffer(clientSocket, receiveBuffer.getBinaryBuffer(), receiveBuffer.size());
    if(received) {
        // parse message / header
        message = ofxTCPPocoUtils::parseMessage(receiveBuffer, TCPPOCO_DELIMITER);
        return true;
    }
    return false;
}

bool ofxTCPPocoUtils::receiveRawBuffer(Poco::Net::StreamSocket* clientSocket, char* buffer, int receiveSize) {
    
    // receiveSize must be > 0
    //buffer.allocate(receiveSize+1); // don't know why i have to add an extra byte to ofBuffer
    
    // while loop to get all data for client
    int dataReceived = 0;
    while(dataReceived < receiveSize) {
        
        try {
            int result = clientSocket->receiveBytes(&buffer[dataReceived], receiveSize-dataReceived);
            if(result > 0) {
                dataReceived += result;
            } else {
                ofLogVerbose() << "Error receiving raw bytes client. Bytes:" << dataReceived; // 0
                return false;
            }
        } catch (Poco::Exception& exc) {
            //Handle your network errors.
            ofLog() << "ofxTCPPocoUtils receiveRawBuffer error: " << exc.displayText();
            return false;
        }
    }
    
    return true;
}


