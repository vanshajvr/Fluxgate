#pragma once

#include "authenticator.hpp"

#include <initializer_list>
#include <string>
#include <unordered_set>

namespace fluxgate {

class TokenListAuthenticator : public Authenticator {
public:
    explicit TokenListAuthenticator(std::initializer_list<std::string> valid_tokens)
        : valid_tokens_(valid_tokens) {}

    bool verify(const std::string& token) const override {
        return valid_tokens_.count(token) > 0;
    }

private:
    std::unordered_set<std::string> valid_tokens_;

};

}//namespace fluxgate