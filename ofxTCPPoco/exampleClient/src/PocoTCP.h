#pragma once

#include "ofMain.h"
#include "PocoTCPServerConnection.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerParams.h"
#include "Poco/Net/TCPServerConnectionFactory.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/Socket.h"

// http://stackoverflow.com/questions/16510205/multithread-tcp-server-with-poco-c-libraries
// http://stackoverflow.com/questions/14632341/poconet-server-client-tcp-connection-event-handler


class PocoTCP {

	public:
    
    void setup() {
        
        //Create a server socket to listen.
        /*Poco::Net::ServerSocket svs(1234);
        
        //Configure some server params.
        Poco::Net::TCPServerParams* pParams = new Poco::Net::TCPServerParams();
        pParams->setMaxThreads(4);
        pParams->setMaxQueued(4);
        pParams->setThreadIdleTime(100);
        
        //Create your server
        Poco::Net::TCPServer myServer(new Poco::Net::TCPServerConnectionFactoryImpl<PocoTCPServerConnection>(), svs, pParams);
        myServer.start();*/
        
        // alt setup...
        Poco::Net::ServerSocket svs(1234);
        Poco::Net::TCPServer myServer(new Poco::Net::TCPServerConnectionFactoryImpl<PocoTCPServerConnection>(), svs);
        myServer.start();
        
        ofLog() << "PocoTCP setup";
        
        // send some data
        Poco::Net::SocketAddress sa("localhost", myServer.port());// .socket().address().port());
        Poco::Net::StreamSocket ss(sa);
        std::string data("hello, world");
        ss.sendBytes(data.data(), (int) data.size());
        char buffer[256] = {0};
        int n = ss.receiveBytes(buffer, sizeof(buffer));
        std::cout << std::string(buffer, n) << std::endl;
    }
    
    void update() {
        
    }
    
    void draw() {
        
    }
		
};
