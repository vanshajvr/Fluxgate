#pragma once

#include <string>

namespace fluxgate {

class Authenticator {

public:
    virtual ~Authenticator()=default;
    virtual bool verify(const std::string& token) const=0;

};


}//namespace fluxgate