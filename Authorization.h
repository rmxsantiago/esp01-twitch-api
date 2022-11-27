#include <Arduino.h>
#include <Networking.h>
#include <string>

#define OAUTH_HOST "id.twitch.tv"

String getAuthorizationRequest(
    const char* clientId,
    const char* clientSecret
);
