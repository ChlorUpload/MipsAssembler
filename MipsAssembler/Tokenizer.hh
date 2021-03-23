#ifndef __H_TOKENIZER__
#define __H_TOKENIZER__
#include "Token.hh"
#include "TokenDefinition.hh"
#include <string>
#include <vector>

class TokenIterator;

class Tokenizer
{
  private:
    std::string                  _mips;
    std::vector<std::string>     _words;
    std::vector<TokenDefinition> _tokenDefinitions;
    std::vector<std::string>     _split(std::string mips);

    bool _IsDelimeter(char c);
    bool _IsPunctuationMark(char c);

  public:
    class TokenIterator
    {
      private:
        Token      token;
        Tokenizer* tokenizer;
        int        wordInd;

      public:
        TokenIterator(Token const& token, Tokenizer* tokenizer, int wordInd);
        TokenIterator(TokenIterator const& iter);
        ~TokenIterator();

        TokenIterator& operator++();
        TokenIterator& operator++(int);
        Token const&   operator*() const;
        Token const*   operator->() const;

        bool operator==(TokenIterator const& other);
        bool operator!=(TokenIterator const& other);

        TokenIterator begin();
        TokenIterator end();
    };

    Tokenizer(Tokenizer&& tokenizer) noexcept;
    Tokenizer(std::string const& mips);
    Tokenizer(Tokenizer const&) = delete;
    Tokenizer()                 = delete;

    std::pair<Token, int> GenerateToken(int wordInd);
    TokenIterator         begin();
    TokenIterator         end();

    Tokenizer& AddTokenDefinition(TokenDefinition const& tokenDefinition);
};

#endif