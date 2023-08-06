#ifndef HTTP_HPP
#define HTTP_HPP

#include <iostream> // 4 debugging

namespace woXrooX{
  class HTTP final{
  public:
    static void handle(const std::string& inData){
      // Parse HTTP Request
      HTTP::parse(inData);

      // Handle GET
      if(HTTP::requestMethod == "GET") HTTP::handle_GET();

      // Update Response
      HTTP::update_response();
    }

    //////// Getters
    static const std::string& getRequestFirsLine(){return HTTP::requestFirstLine;}
    static const std::string& getResponse(){return HTTP::response;}
    static const std::string& getResponseFirstLine(){return HTTP::responseFirstLine;}

  private:
    static inline void parse(const std::string& inData){
      //// First Line
      HTTP::requestFirstLine = inData.substr(0, inData.find("\r\n"));

      // Validate First Line. If First Line Is Invalid Rest Doesn't Matter
      int spaceCount = 0;
      for(auto &c : HTTP::requestFirstLine) if(c == ' ') spaceCount++;

      if(spaceCount != 2){
        Log::error("Invalid HTTP Request: Invalid First Line");
        HTTP::hasError = true;
        return;
      }

      // header
      HTTP::requestHeader = inData.substr(0, inData.find("\r\n\r\n"));

      // Method
      HTTP::requestMethod = HTTP::requestFirstLine.substr(0, HTTP::requestFirstLine.find(' '));

      // Protocol
      HTTP::requestProtocol = HTTP::requestFirstLine.substr(
        HTTP::requestFirstLine.rfind(' ')+1, HTTP::requestFirstLine.length() - HTTP::requestFirstLine.rfind(' ')
      );

      // File
      HTTP::requestFile = HTTP::requestFirstLine.substr(
        HTTP::requestFirstLine.find(' ')+1, HTTP::requestFirstLine.length() - HTTP::requestMethod.length() - HTTP::requestProtocol.length() - 2
      );

    }

    static inline void handle_GET(){
      //// Check If Request Is File Or Just Path
      if(HTTP::requestFile.rfind('.') != std::string::npos && HTTP::requestFile.rfind('/') < HTTP::requestFile.rfind('.')){

        //// Get The File
        // 404
        if(Files::getAll().find(HTTP::requestFile) == Files::getAll().end()){
          HTTP::responseStatus = Configurations::HTTP::ResponseStatus::notFound;
          HTTP::responseContentType.clear();
          HTTP::responseBody.clear();
        }

        // 200
        else{

          //// If File Exists Then Define Response Content Type

          // File Extension
          std::string fileExtension = HTTP::requestFile.substr(HTTP::requestFile.rfind('.')+1, HTTP::requestFile.length() - HTTP::requestFile.rfind('.'));

          // HTML
          if(fileExtension == "html") HTTP::responseContentType = Configurations::HTTP::ContentType::textHTML;
          // CSS
          else if(fileExtension == "css") HTTP::responseContentType = Configurations::HTTP::ContentType::textCSS;
          // JavaScript
          else if(fileExtension == "js") HTTP::responseContentType = Configurations::HTTP::ContentType::textJavaScript;
          // Text
          else if(fileExtension == "txt") HTTP::responseContentType = Configurations::HTTP::ContentType::text;

          // SVG
          else if(fileExtension == "svg") HTTP::responseContentType = Configurations::HTTP::ContentType::imageSVG;
          // ICO
          else if(fileExtension == "ico") HTTP::responseContentType = Configurations::HTTP::ContentType::imageXIcon;
          // PNG
          else if(fileExtension == "png") HTTP::responseContentType = Configurations::HTTP::ContentType::imagePNG;
          // JPEG & JPG
          else if(fileExtension == "jpeg" || fileExtension == "jpg") HTTP::responseContentType = Configurations::HTTP::ContentType::imageJPEG;

          // WOFF2
          else if(fileExtension == "woff2") HTTP::responseContentType = Configurations::HTTP::ContentType::fontWOFF2;
          // OTF
          else if(fileExtension == "woff2") HTTP::responseContentType = Configurations::HTTP::ContentType::fontOTF;
          // TTF
          else if(fileExtension == "ttf") HTTP::responseContentType = Configurations::HTTP::ContentType::fontTTF;

          // Unknown
          else HTTP::responseContentType = "unknown/unknown";


          HTTP::responseStatus = Configurations::HTTP::ResponseStatus::ok;
          HTTP::responseBody = Files::getAll().find(HTTP::requestFile)->second;
        }
      }

      // Serve For Just Path Request
      else{
        HTTP::responseStatus = Configurations::HTTP::ResponseStatus::ok;
        HTTP::responseContentType = Configurations::HTTP::ContentType::textHTML;
        HTTP::responseBody = Files::getAll().find(Configurations::pathIndexHTML)->second;
      }
    }

    static inline void update_response(){
      HTTP::responseFirstLine = HTTP::responseProtocol + " " + HTTP::responseStatus;
      HTTP::responseHeader =
        HTTP::responseFirstLine + "\r\n" +
        // HTTP::responseCacheControl + "\r\n" +
        "Content-Type: " + HTTP::responseContentType + "\r\n" +
        // "Content-Length: " + HTTP::responseBody::size() +
        // HTTP::responseConnection +
        HTTP::responseServer +
        HTTP::responseHeaderEnd;

      HTTP::response = HTTP::responseHeader + HTTP::responseBody;

    }

    static bool hasError;

    // Request
    static std::string requestHeader;
    static std::string requestFirstLine;
    static std::string requestMethod;
    static std::string requestFile;
    static std::string requestProtocol;

    // Response
    static std::string response;
    static std::string responseHeader;
    static std::string responseBody;
    static std::string responseFirstLine;
    static std::string responseStatus;
    static std::string responseProtocol;
    static std::string responseCacheControl;
    static std::string responseContentType;
    static std::string responseConnection;
    static const std::string responseServer;
    static const std::string responseHeaderEnd;

  };

  bool HTTP::hasError = false;

  ////// In Data
  std::string HTTP::requestHeader;
  std::string HTTP::requestFirstLine;
  std::string HTTP::requestMethod;
  std::string HTTP::requestFile;
  std::string HTTP::requestProtocol;


  ////// Out Data
  std::string HTTP::response;
  std::string HTTP::responseHeader;
  std::string HTTP::responseBody;
  std::string HTTP::responseFirstLine;
  std::string HTTP::responseProtocol = "HTTP/2";
  std::string HTTP::responseStatus;
  std::string HTTP::responseCacheControl = "max-age=600";
  std::string HTTP::responseContentType;
  std::string HTTP::responseConnection = "Connection: Close\r\n";
  const std::string HTTP::responseServer = "Server: woXrooX Web Server (WWS)\r\n";
  const std::string HTTP::responseHeaderEnd = "\r\n";

}

#endif

// https://code.tutsplus.com/tutorials/http-headers-for-dummies--net-8039
