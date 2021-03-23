#include "TokenIterator.hh"

TokenIterator::TokenIterator(Token const& token, Tokenizer* tokenizer, int wordInd) :
    token { token },
    tokenizer { tokenizer },
    wordInd { wordInd }
{}

TokenIterator::TokenIterator(TokenIterator const& iter) :
    token { iter.token },
    tokenizer { iter.tokenizer },
    wordInd { iter.wordInd }
{}

TokenIterator::~TokenIterator()
{
    token = Token::CreateNullToken();
}

TokenIterator& TokenIterator::operator++()
{
    auto tuple = tokenizer->GenerateToken(wordInd);
    token      = tuple.first;
    wordInd    = tuple.second;
    return *this;
}

TokenIterator& TokenIterator::operator++(int)
{
    TokenIterator temp = *this;
    ++(*this);
    return temp;
}

Token const& TokenIterator::operator*() const
{
    return token;
}

Token const* TokenIterator::operator->() const
{
    return &token;
}

bool TokenIterator::operator==(TokenIterator const& other)
{
    return token == other.token;
}

bool TokenIterator::operator!=(TokenIterator const& other)
{
    return token != other.token;
}

TokenIterator TokenIterator::begin()
{
    return *this;
}

TokenIterator TokenIterator::end()
{
    return TokenIterator(Token::CreateNullToken(), nullptr, 0);
}