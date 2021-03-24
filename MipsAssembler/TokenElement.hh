#ifndef __H_TOKEN_ELEMENT__
#define __H_TOKEN_ELEMENT__
#include "Token.hh"

struct TokenElement : public Element
{
    Token token;
    TokenElement(ElementType type, Token const& token) : Element { type }, token { token } {}
};

#endif