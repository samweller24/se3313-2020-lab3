#include "thread.h"
#include "socket.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace Sync;

int main(void)
{
        // Welcome the user
        std::cout << "SE3313 Lab 3 Client" << std::endl;

        // Create our socket
        Socket socket("127.0.0.1", 3000);

        socket.Open();

        // Get user input
        std::string input;
        while(true) {
                std::cout << "Enter a message to send: ";
                std::cin >> input;

                ByteArray ba_input = ByteArray(input);
                socket.Write(ba_input);

                if (input == "done") {
                        break;
                }

                // Get result
                ByteArray result;
                socket.Read(result);

                // Convert to string and display
                std::string str_result = result.ToString();

                // Allow server to kill client
                if (str_result == "done") {
                        break;
                }
                std::cout << str_result << std::endl;
        }

        socket.Close();

        return 0;
}