#include "../include/RequestMessage.hpp"

RequestMessage::RequestMessage( ) {
    uri = NULL;
    headers = NULL;
}

RequestMessage::~RequestMessage( ) {
    if (uri) delete uri;
    if (headers) delete headers;
}