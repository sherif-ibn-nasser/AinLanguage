#pragma once
#include "LexerToken.hpp"
#include "TokensIterator.hpp"
#include <vector>
SharedTokensIterator getTokensIterator(
    std::vector<LexerToken> tokens
);