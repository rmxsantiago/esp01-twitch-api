#include "Networking.h"

#define PORT 443

int connect(
    WiFiClientSecure &client,
    const char* host
) {    
    if (!client.connect(host, PORT)) {
        return FAILED;
    }
    return CONNECTED;
}

String processResponse(WiFiClientSecure &client) {
    stringstream response;
    bool capturing = false;
    
    while (client.available()) {
        char ch = static_cast<char>(client.read());
        if(ch == '{' || capturing == true || ch == '}') {
            capturing = true;
            response << ch << flush;
            if(ch == '}') {
                break;
            }
        }
    }

    return String(response.str().c_str());
}
