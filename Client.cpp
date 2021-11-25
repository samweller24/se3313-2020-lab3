#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;

//handle connection to the server
class ClientThread : public Thread
{
private:
	// Reference to our connected socket
	Socket& socket;
	// Data to send to server
	ByteArray data;
	std::string data_str;
	bool& endThread;

public:
	ClientThread(Socket& socket, bool& endThread):socket(socket), endThread(endThread){}

	~ClientThread()
	{
		std::cout << "Stopping ClientThread instance" << std::endl;
	}

	virtual long ThreadMain()
	{
		int result = socket.Open();
		while(!endThread){
				std::cout << "Please input your data (done to exit): ";
				std::cout.flush();

				// Get the data
				std::getline(std::cin, data_str);
				data = ByteArray(data_str);

				if(data_str == "done"){
					std::cout << "Closing the client..." << std::endl;
					endThread = true;
					socket.Write(data);
					break;
				}	

				// Write to the server
				socket.Write(data);
				
				// Get the response
				socket.Read(data);
				if(data.ToString().size() == 0)
				{
					std::cout<<"Server failed to respond... Terminating client\n";
					endThread = true;
					break;
				}
				std::cout<< "Server Response: " + data.ToString() << std::endl;
			
			
		}
		return 0;
	}
};


int main(void)
{
	// Welcome the user 
	std::cout << "SE3313 Lab 3 Client" << std::endl;

	// Create our socket
	Socket socket("127.0.0.1", 3000);
	bool endThread = false;
	
	// Scope to kill thread
	{
		// Thread to perform socket operations on
		ClientThread clientThread(socket, endThread);

		//Loop for clientThread
		while(!endThread)
		{
			sleep(1);
		}
		
		// Wait to make sure the thread is cleaned up
		std::cout<<"Waiting for thread to close...\n";
	}

	// Attempt to close the socket
	try
	{
		std::cout<<"Closing socket...\n";
		socket.Close();
	}
	catch (...)
	{}

	return 0;
}
