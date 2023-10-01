#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream> // 4 debugging

// Not sure if map is faster! map -> O(log n) - unordered_map ->  O(1) inbetween O(n)
#include <unordered_map>

namespace woXrooX{
	class HTTP final{
	public:
		static void handle(const std::string& inData){
			HTTP::RAW = inData;

			// Parse HTTP Request
			HTTP::parse();

			// Handle GET
			HTTP::responseStatus = Configurations::HTTP::ResponseStatus::ok;
			HTTP::responseContentType = Configurations::HTTP::ContentType::textHTML;
			HTTP::responseBody = "Hello world!";

			// Update Response
			// HTTP::update_response();
		}

		//////// Getters
		static const std::string& getFirsLine(){return HTTP::requestLine;}
		static const std::string& getResponse(){return HTTP::response;}
		static const std::string& getStatusLine(){return HTTP::responseStatusLine;}

	private:
		static bool parse(){

			// Check if the requestLine is valid
			if(HTTP::parseRequestLine() == false) return false;



			// headers
			// HTTP::requestHeader = inData.substr(0, inData.find("\r\n\r\n"));

			return true;
		}

		static bool parseRequestLine(){
			// Request Line
			HTTP::requestLine = HTTP::RAW.substr(0, HTTP::RAW.find("\r\n"));

			// Validate Request Line. If Request Line Is Invalid Rest Doesn't Matter
			int spaceCount = 0;
			for(auto &c : HTTP::requestLine) if(c == ' ') spaceCount++;

			if(spaceCount != 2){
				Log::error("Invalid HTTP Request: Invalid First Line");
				HTTP::hasError = true;
				return false;
			}

			// Method
			HTTP::requestMethod = HTTP::requestLine.substr(0, HTTP::requestLine.find(' '));

			// Protocol
			HTTP::requestProtocol = HTTP::requestLine.substr(
				HTTP::requestLine.rfind(' ')+1, HTTP::requestLine.length() - HTTP::requestLine.rfind(' ')
			);

			// File
			HTTP::requestFile = HTTP::requestLine.substr(
				HTTP::requestLine.find(' ')+1, HTTP::requestLine.length() - HTTP::requestMethod.length() - HTTP::requestProtocol.length() - 2
			);

			return true;
		}

		static bool parseHeaders(){

			return true;
		}

		static bool parseBody(){

			return true;
		}

		static inline void update_response(){
			HTTP::responseStatusLine = HTTP::responseProtocol + " " + HTTP::responseStatus;
			HTTP::responseHeader =
			HTTP::responseStatusLine + "\r\n" +
			// HTTP::responseCacheControl + "\r\n" +
			"Content-Type: " + HTTP::responseContentType + "\r\n" +
			// "Content-Length: " + HTTP::responseBody::size() +
			// HTTP::responseConnection +
			HTTP::responseServer +
			HTTP::responseHeaderEnd;

			HTTP::response = HTTP::responseHeader + HTTP::responseBody;
		}

		static bool hasError;

		static std::string RAW;

		static std::string requestLine;
		static std::string requestMethod;
		static std::string requestFile;
		static std::string requestProtocol;


		static std::string statusLine;

		static std::unordered_map<std::string, std::string> headers;
		static std::string body;


		// Request
		static std::string requestHeader;
		static std::string requestFirstLine;


		// Response
		static std::string response;
		static std::string responseHeader;
		static std::string responseBody;
		static std::string responseStatusLine;
		static std::string responseStatus;
		static std::string responseProtocol;
		static std::string responseCacheControl;
		static std::string responseContentType;
		static std::string responseConnection;
		static const std::string responseServer;
		static const std::string responseHeaderEnd;

	};

	bool HTTP::hasError = false;

	std::string HTTP::RAW;
	std::string HTTP::requestLine;
	std::string HTTP::requestMethod;
	std::string HTTP::requestFile;
	std::string HTTP::requestProtocol;

	////// In Data
	std::string HTTP::requestHeader;
	std::string HTTP::requestFirstLine;


	////// Out Data
	std::string HTTP::response;
	std::string HTTP::responseHeader;
	std::string HTTP::responseBody;
	std::string HTTP::responseStatusLine;
	std::string HTTP::responseProtocol = "HTTP/2";
	std::string HTTP::responseStatus;
	std::string HTTP::responseCacheControl = "max-age=600";
	std::string HTTP::responseContentType;
	std::string HTTP::responseConnection = "Connection: Close\r\n";
	const std::string HTTP::responseServer = "Server: woXrooX Web Server (WWS)\r\n";
	const std::string HTTP::responseHeaderEnd = "\r\n";
}

#endif
