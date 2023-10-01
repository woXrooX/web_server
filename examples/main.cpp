#include "../source/server.hpp"
#include <iostream>

std::string resp = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\nContent-Type: text/plain; charset=utf-8\r\n\r\nHello World!";

const std::string& handle_request(const std::string& data){
	std::cout << data;
	return resp;
}


int main(){
    woXrooX::Server::init(handle_request, "127.0.0.1", "8008");

    return 1;
}
