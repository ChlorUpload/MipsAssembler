#include "TokenDefinition.hh"
#include <algorithm>
#include <sstream>

Token TokenDefinitionFactory::ParseToken(std::string const& token,
                                         TokenType          type,
                                         std::string const& word)
{
    if (token == word) { return Token::CreateToken(type, word); }
    return Token::CreateNullToken();
};

TokenDefinition
TokenDefinitionFactory::CreateTokenDefinition(std::vector<std::string> const& tokens,
                                              TokenType                       type)
{
    auto lambda = [=](std::string const& word) {
        Token retToken = Token::CreateNullToken();

        for (auto& token : tokens)
        {
            retToken = ParseToken(token, type, word);
            if (retToken.IsNull()) { continue; }
            else
            {
                break;
            }
        }

        return retToken;
    };

    return lambda;
}

Token TokenDefinitionFactory::RegisterTokenDefinition(std::string const& word)
{
    if (word == "zero") return Token::CreateToken(TokenType::REGISTER, "0");
    if (word.length() < 2 || word[0] != '$') return Token::CreateNullToken();

    std::string registerNum = word.substr(1);

    std::stringstream ss(registerNum);
    int               n;

    try
    {
        ss >> n;
        if (n >= 0 && n <= 31) return Token::CreateToken(TokenType::REGISTER, registerNum);
    }
    catch (...)
    {}

    return Token::CreateNullToken();
}

Token TokenDefinitionFactory::LabelTokenDefinition(std::string const& word)
{
    if (!word.empty() && word.back() == ':')
        return Token::CreateToken(TokenType::LABEL, word.substr(0, word.length() - 1));

    return Token::CreateNullToken();
}

Token TokenDefinitionFactory::ConstantTokenDefinition(std::string const& word)
{
    if (word.empty()) return Token::CreateNullToken();
    if (word[0] == '-' || std::isdigit(word[0]))
    {
        if (word[0] == '-' && word.size() == 1) return Token::CreateNullToken();
        auto it = std::find_if_not(
            word.begin() + 1, word.end(), [](char c) { return std::isdigit(c); });
        if (it != word.end()) { return Token::CreateNullToken(); }
        else
        {
            return Token::CreateToken(TokenType::CONST_DECI, word);
        }
    }
    else if (word.size() > 2 && word[0] == '0' && word[1] == 'x')
    {
        auto it = std::find_if_not(word.begin() + 2, word.end(), [](char c) {
            return std::isdigit(c) && ('A' <= std::toupper(c) && std::toupper(c) <= 'F');
        });
        if (it != word.end()) { return Token::CreateNullToken(); }
        else
        {
            return Token::CreateToken(TokenType::CONST_HEX, word);
        }
    }

    return Token::CreateNullToken();
}

Token TokenDefinitionFactory::LabelIdTokenDefinition(std::string const& word)
{
    if (_IsValidId(word)) return Token::CreateToken(TokenType::LABEL_ID, word);
    return Token::CreateNullToken();
}

bool TokenDefinitionFactory::_IsValidId(std::string id)
{
    for (auto c : id)
    {
        if ('A' <= c && c <= 'z') { continue; }
        else if ('0' <= c && c <= '9')
        {
            continue;
        }
        else if (c == '_')
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}