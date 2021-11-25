#include "thread.h"
#include "socketserver.h"
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <algorithm>

using namespace Sync;

// This thread handles the socket operations
class SocketThread : public Thread
{
private:
    //reference to our connected socket
    Socket& socket;
    //data we are receiving
    ByteArray data;
    // End the thread
    bool& endThread;
public:
    SocketThread(Socket& socket,bool& endThread)
    : socket(socket), endThread(endThread){}

    ~SocketThread()
    {}

    Socket& GetSocket()
    {
        return socket;
    }

    virtual long ThreadMain()
    {
        std::cout << "SocketThread Started" << std::endl;
        while(!endThread)
        {
            try
            {
                if(endThread){
                    socket.Close();
                    std::cout << "SocketThread Ending" << std::endl;
                    delete this;
                }
                // Wait for data
                while(socket.Read(data) > 0){
                    std::string string = data.ToString();
                    std::cout << "Received: " << string << std::endl;
                    // Perform operations on the data
                    // convert to upper case 
                    for (int i=0; i<string.length(); i++) {
                        string[i] = toupper(string[i]);
                    }

                    //Notifies client has closed
                    std::string done = "DONE";
                    if (string == done) {
                        std::cout<<"Client has closed...\n";
                    }

                    //Closing server command entered
                    std::string close = "CLOSE";
                    if (string == close) {
                        std::cout<<"Terminating server...\n";
                        endThread = true;
                    }

                    ByteArray return_data = ByteArray(string);
                    // Send it back
                    socket.Write(return_data);
                }
            }
            catch (...)
            {
                endThread = true;
            }
        }
        return 0;
    }
};

//handles server operations
class ServerThread : public Thread
{
private:
    SocketServer& server;
    std::vector<SocketThread*> socketThreads;
    bool terminate = false;
public:
    ServerThread(SocketServer& server,std::vector<SocketThread*>  socketThreads)
    : server(server), socketThreads(socketThreads)
    {}

    ~ServerThread()
    {
        for(SocketThread* thread : socketThreads){
            try {
                // Close the sockets
                Socket& toClose = thread->GetSocket();
                toClose.Close();
                delete thread;
            }
            catch (...){
                terminate = true;
            }
        }

        terminate = true;
    }

    virtual long ThreadMain()
    {
        while(!terminate){
            try {
                // Wait for a client socket connection
                Socket* newConnection = new Socket(server.Accept());

                // Pass a reference to this pointer into a new socket thread
                Socket& socketReference = *newConnection;
                socketThreads.push_back(new SocketThread(socketReference, terminate));
            }
            catch (...){
                terminate = true;
            }
        }
        return 1;
    }
};


int main(void)
{
    std::cout << "I am a server." << std::endl;
	std::cout << "Press enter to terminate the server...";
    std::cout.flush();
	
    // Create our server
    SocketServer server(3000);    
    std::vector<SocketThread*> socketThreads; 

    // Need a thread to perform server operations
    ServerThread serverThread(server, socketThreads);
	
    // This will wait for input to shutdown the server
    FlexWait cinWaiter(1, stdin);
    cinWaiter.Wait();

    // Shut down and clean up the server
    server.Shutdown();
    std::cout << "Server Exiting." << std::endl;

}
