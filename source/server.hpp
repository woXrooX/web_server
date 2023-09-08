#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <string.h> // memset
#include <sys/socket.h>
#include <arpa/inet.h> // for -> pton mton;arga
#include <unistd.h> // for -> close()
#include <netdb.h> //  for -> getaddrinfo

#include "includes/logger/source/logger.hpp"

// Order Is Important!
#include "configurations.hpp"
// #include "files.hpp"
#include "HTTP.hpp"

//////////////// Mess
////// Server Computer's Infos
struct addrinfo hints, *result;

////// Client Computer's Infos
struct sockaddr_storage client_address;
socklen_t client_address_size = sizeof client_address;

namespace woXrooX{
  class Server final{
  public:
    static void start(){
      Server::server_computer_infos_init();
      Server::create_socket_tcp();
      Server::make_address_reusalbe();
      Server::keep_alive();
      Server::create_bind();
      Server::create_listen();
      Server::on = true;
      Server::create_new_socket_tcp();
    }

    static void stop(){
      // Check If Server Is Running
      if(Server::on == false) return;

      Server::on = false;
      Server::close_new_socket_tcp();
      Server::shutdown_socket_tcp();
      Server::close_socket_tcp();
    }

  private:
    static void server_computer_infos_init(){
      memset(&hints, 0, sizeof hints);      // Zeroing hints before using

      hints.ai_family = AF_UNSPEC;          // don't care IPv4 or IPv6
      hints.ai_socktype = SOCK_STREAM;      // TCP stream sockets
      hints.ai_flags = AI_PASSIVE;          // fill in my computer's IP for me

      Server::get_addr_info = getaddrinfo(Server::IP.c_str(), Server::PORT.c_str(), &hints, &result);

      if(Server::get_addr_info != 0) Log::error("Error In Establishing Server Computer Informations");
      else Log::success("Server Computer Informations Established Successfully");

    }


    static void create_socket_tcp(){
      // Check If Server Computer Informations Established Successfully
      if(Server::get_addr_info != 0) return;

      Server::socket_tcp = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
      if(Server::socket_tcp == -1) Log::error("Failed To Create Socket Descriptor.");
      else Log::success("Socket Descriptor Created Successfully");

    }


    static void make_address_reusalbe(){
      // Check If Socket TCP Created Successfully
      if(Server::socket_tcp == -1) return;

      // For Development!
      // Below Code Allows Rebind To Address While The Address Is In Use.
      // w/o This Function It Fails To Bind To The Address After Terminating The Server.
      // OS Keeps Socket Open For A While So Binding Is Imposible!
      // Disable This Function On Production.
      // Enable/Disable At woXrooX::Server::start::make_address_reusalbe();
      const int REUSE_MODE = 1;
      if(setsockopt(Server::socket_tcp, SOL_SOCKET, SO_REUSEADDR, &REUSE_MODE, sizeof(REUSE_MODE)) == -1) Log::error("Failed to enable address reuse mode.");
      else{
        Log::custom("DEV", "Address reuseable mode is enabled successfully.");
        Log::warning("On production disable address reuseable mode at 'woXrooX::Server::start()::make_address_reusalbe();'");
      }
    }

    static void keep_alive(){
      // Check If Socket TCP Created Successfully
      if(Server::socket_tcp == -1) return;

      const int KEEP_ALIVE = 1;
      if(setsockopt(Server::socket_tcp, SOL_SOCKET, SO_KEEPALIVE, &KEEP_ALIVE, sizeof(KEEP_ALIVE)) == -1) Log::error("Failed to enable 'Keep Alive'.");
      else Log::success("'Keep Alive' enabled successfully.");
    }


    static void create_bind(){
      // Check If Socket TCP Created Successfully
      if(Server::socket_tcp == -1) return;

      Server::binding = bind(Server::socket_tcp, result->ai_addr, result->ai_addrlen);
      if(Server::binding == -1) Log::error("Failed to bind.");
      else Log::success("Bound successfully.");
    }

    static void create_listen(){
      // Check If Bound Successfully
      if(Server::binding == -1) return;

      Server::listening = listen(Server::socket_tcp, Server::BACKLOG);
      if(Server::listening == -1) Log::error("Failed to listen.");
      else{
        Log::success("Listening successfully on: http://"+Server::IP+":"+Server::PORT);
        Log::line();
      }
    }


    static void create_new_socket_tcp(){
      // Check if listening successfully
      if(Server::listening == -1) return;

      while((Server::new_socket_tcp = accept(Server::socket_tcp, (struct sockaddr *)&client_address, &client_address_size)) >= 0){
        Log::line();
        Log::success("New connection accepted successfully.");

        // Log Client IP Address
        char client_address_printable[46]; // 46 is max IPv6 size
        inet_ntop(AF_INET, &client_address, client_address_printable, sizeof client_address_printable);
        Log::custom("Client IP address", client_address_printable);

        // Receive data
        Server::in();

        // Send data
        Server::out();

        Server::close_new_socket_tcp();

      }

      if(Server::new_socket_tcp == -1) Log::error("Failed to accept.");

    }

    static void in(){
      // bytes_received -1 on error
      // bytes_received 0 on end of the line
      // bytes_received > 0 on data

      // Check if new_socket_tcp established successfully
      if(Server::new_socket_tcp == -1) return;

      char inData[Server::BUFFER];
      int received_data_size = 0;
      do{
        Server::bytes_received = recv(Server::new_socket_tcp, inData, Server::BUFFER, 0);
        received_data_size += Server::bytes_received;

        if(received_data_size > Server::BUFFER-1 || inData[Server::BUFFER-1] == '\n') break;
        if(Server::bytes_received == -1){Log::error("Failed to receive data."); return;}
        if(Server::bytes_received == 0){Log::info("End of the line."); break;}
        if(Server::bytes_received > 0){Log::success("Data received successfully."); break;}

      }while(true);

      HTTP::handle(inData);

      Log::custom("<", HTTP::getRequestFirsLine());

    }

    static void out(){
      // Check if new_socket_tcp established successfully
      if(Server::new_socket_tcp == -1) return;

      Server::outData = HTTP::getResponse();
      Server::bytes_sent = send(Server::new_socket_tcp, Server::outData.c_str(), Server::outData.size(), 0);
      if(Server::bytes_sent == -1){Log::error("Failed to send data."); return;}
      // else Log::success("Data Sent Successfully");

      Log::custom(">", HTTP::getResponseFirstLine());
    }



    static void close_new_socket_tcp(){
      // Check if new_socket_tcp established successfully
      if(Server::new_socket_tcp == -1) return;

      if(close(Server::new_socket_tcp) == -1) Log::error("Failed to close new socket TCP.");
      else Log::success("New socket TCP closed successfully.");
    }

    static void shutdown_socket_tcp(){
      // Check if socket_tcp established successfully
      if(Server::socket_tcp == -1) return;

      if(shutdown(Server::socket_tcp, SHUT_RDWR) != 0) Log::error("Failed to shutdown socket TCP.");
      else Log::success("Socket TCP shutdown successfully.");
    }

    static void close_socket_tcp(){
      // Check if socket_tcp established successfully
      if(Server::socket_tcp == -1) return;

      if(close(Server::socket_tcp) == -1) Log::error("Failed to close socket TCP.");
      else Log::success("Socket TCP closed successfully.");
    }


    /////////// Variables
    static bool on;

    static std::string outData;
    // static std::string inData; Not Used For Now

    static int bytes_sent;
    static int bytes_received;
    static int BACKLOG;
    static int BUFFER;

    // Socket SetUp
    static int get_addr_info;
    static int socket_tcp;
    static int binding;
    static int listening;
    static int new_socket_tcp;

    static std::string IP;
    static std::string PORT;
  };

  bool Server::on = false;

  std::string Server::outData;

  int Server::bytes_sent = 0;
  int Server::bytes_received = 0;
  int Server::BACKLOG = 1024;
  int Server::BUFFER = 131072;

  int Server::get_addr_info = 0;
  int Server::socket_tcp = -1;
  int Server::binding = -1;
  int Server::listening = -1;
  int Server::new_socket_tcp = -1;

  std::string Server::IP = "127.0.0.1";
  std::string Server::PORT = "8080";

}

#endif
