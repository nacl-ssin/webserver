//
// Created by 26372 on 2022/4/17.
//

#include "http_connect.h"


HttpConnect::HttpConnect(int sock, sockaddr_in addr) : sock_(sock), addr_(addr) {

}
