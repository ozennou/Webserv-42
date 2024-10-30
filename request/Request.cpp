#include "../include/Request.hpp"

Request::Request( ) {
    uri = NULL;
    headers = NULL;
}

Request::~Request( ) {
    if (uri) delete uri;
    if (headers) delete headers;
}