#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>

#define SSID "TP-Link_876C"
#define PASS "62935845"
#define PORT 80


class Srv {
    public: 
        int begin();
        void update();

    private: 
        AsyncWebServer server{PORT};
        AsyncWebSocket ws{"/ws"};
};

