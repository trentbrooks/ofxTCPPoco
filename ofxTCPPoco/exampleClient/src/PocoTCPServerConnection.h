#pragma once

#include "ofMain.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"


class PocoTCPServerConnection : public Poco::Net::TCPServerConnection {

	public:
    
    PocoTCPServerConnection(const Poco::Net::StreamSocket& s) :
        Poco::Net::TCPServerConnection(s) {
    }
    
    void run() {
        
        Poco::Net::StreamSocket& ss = socket();
        
        try {
            char buffer[256];
            int n = ss.receiveBytes(buffer, sizeof(buffer));
            while (n > 0) {
                ss.sendBytes(buffer, n);
                n = ss.receiveBytes(buffer, sizeof(buffer));
            }
        }
        catch (Poco::Exception& exc)
        { std::cerr << "EchoConnection: " << exc.displayText() << std::endl; }
        
        /*cout << "New connection from: " << socket().peerAddress().host().toString() <<  endl << flush;
        bool isOpen = true;
        Poco::Timespan timeOut(10,0);
        unsigned char incommingBuffer[1000];
        while(isOpen){
            if (socket().poll(timeOut,Poco::Net::Socket::SELECT_READ) == false){
                cout << "TIMEOUT!" << endl << flush;
            }
            else{
                cout << "RX EVENT!!! ---> "   << flush;
                int nBytes = -1;
                
                try {
                    nBytes = socket().receiveBytes(incommingBuffer, sizeof(incommingBuffer));
                }
                catch (Poco::Exception& exc) {
                    //Handle your network errors.
                    cerr << "Network error: " << exc.displayText() << endl;
                    isOpen = false;
                }
                
                
                if (nBytes==0){
                    cout << "Client closes connection!" << endl << flush;
                    isOpen = false;
                }
                else{
                    cout << "Receiving nBytes: " << nBytes << endl << flush;
                }
            }
        }
        cout << "Connection finished!" << endl << flush;*/
    }

		
};
