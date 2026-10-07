#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

#define PORT 80

class Srv {
    public: 
        int begin();
        void update();

    private: 
        AsyncWebServer server{PORT};
        AsyncWebSocket ws{"/ws"};
};

