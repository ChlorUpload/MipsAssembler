#ifndef __H_TOKEN_DEFINITION__
#define __H_TOKEN_DEFINITION__
#include "Token.hh"
#include <functional>
#include <stdexcept>
#include <vector>

using TokenDefinition = std::function<Token(std::string const&)>;

class TokenDefinitionFactory
{
  private:
    static Token ParseToken(std::string const& tokens, TokenType type, std::string const& word);
    static bool  _IsValidId(std::string id);

  public:
    static TokenDefinition CreateTokenDefinition(std::vector<std::string> const& tokens,
                                                 TokenType                       type);

    static Token RegisterTokenDefinition(std::string const& word);
    static Token LabelTokenDefinition(std::string const& word);
    static Token ConstantTokenDefinition(std::string const& word);
    static Token LabelIdTokenDefinition(std::string const& word);
};

#endif