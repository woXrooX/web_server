#ifndef CONFIGURATIONS_HPP
#define CONFIGURATIONS_HPP

#include <string>

namespace woXrooX{

  struct Configurations final{
    /////// Server
    struct Server final{
      // "127.0.0.1" -> localhost
      static inline const std::string IP = "192.168.0.194";
      static inline const std::string PORT = "8080";

      // 4 GCP
      // static inline const std::string IP = "10.166.0.2";
      // static inline const std::string PORT = "80";

      static inline const int BUFFER = 131072;
      static inline const int BACKLOG = 1024;

    };

    /////// Http
    struct Http final{
      struct ContentType final{
        static inline const std::string text = "text/plain; charset=utf-8";
        static inline const std::string textHTML = "text/html; charset=utf-8";
        static inline const std::string textJavaScript = "text/javascript; charset=utf-8";
        static inline const std::string textCSS = "text/css; charset=utf-8";

        static inline const std::string imageSVG = "image/svg+xml";
        static inline const std::string imageXIcon = "image/x-icon";
        static inline const std::string imagePNG = "image/png";
        static inline const std::string imageJPEG = "image/jpeg";
        static inline const std::string imageGIF = "image/gif";

        static inline const std::string fontWOFF = "ont/woff";
        static inline const std::string fontWOFF2 = "font/woff2";
        static inline const std::string fontOTF = "font/otf";
        static inline const std::string fontTTF = "font/ttf";
      };

      struct ResponseStatus final{
        static inline const std::string ok = "200 OK";
        static inline const std::string notFound = "404 Not Found";
      };

    };

    // If Two Or More "front" Named Folders Exist In Configurations::absolutePathFront Then It Will Leack System Files!!!
    static inline const std::string absolutePathFront = "/home/woxroox/Desktop/woXTools/source/tools/server/front/";

    // Path 4 GCP
    // static inline const std::string absolutePathFront = "/home/woxroox/woXTools/source/tools/server/front/";

    static inline const std::string pathIndexHTML = "/html/index.html";
    static inline const std::string absolutePathBack = "";

  };

}

#endif
