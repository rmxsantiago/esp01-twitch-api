#include <Networking.h>
#include <sstream>
#include <string>

#define TWITCH_API_HOST "api.twitch.tv"

using namespace std;

String getStreams(
    String streamerName,
    const char* clientId,
    String accessToken
);
