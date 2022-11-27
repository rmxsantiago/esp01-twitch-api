#include "TwitchAPI.h"

String getStreams(
    String streamerName,
    const char* clientId,
    String accessToken
) {
    stringstream header;
    header << "GET /helix/streams?user_login=" << streamerName.c_str() << " HTTP/1.1" << endl;
    header << "Host: " << TWITCH_API_HOST << endl;
    header << "Authorization: Bearer " << accessToken.c_str() << endl;
    header << "Client-Id: " << clientId << endl;    
    header << endl;
    header << endl;

    return String(header.str().c_str());
}

