#include "Tokenizer.hh"

#include <sstream>
#include <stdexcept>

Tokenizer::Tokenizer(Tokenizer&& tokenizer) noexcept
{
    _mips  = std::move(tokenizer._mips);
    _words = std::move(tokenizer._words);
}
Tokenizer::Tokenizer(std::string const& mips) : _mips { mips }
{
    _words = _split(mips);
}

std::vector<std::string> Tokenizer::_split(std::string mips)
{
    std::vector<std::string> vec;

    std::string::iterator lastIter = mips.begin(), iter = mips.begin();
    while (true)
    {
        lastIter = iter;
        iter     = std::find_if(
            iter, mips.end(), [&](char c) { return _IsDelimeter(c) || _IsPunctuationMark(c); });

        if (iter != mips.end())
        {
            std::string str(lastIter, iter);
            if (str.size() > 0) vec.push_back(str);

            if (_IsPunctuationMark(*iter)) { vec.push_back(std::string(1, *iter)); }

            iter++;
        }
        else
            break;
    }

    return vec;
}

std::pair<Token, int> Tokenizer::GenerateToken(int wordInd)
{
    Token retToken = Token::CreateNullToken();
    int   retInd   = wordInd;

    try
    {
        do {
            for (TokenDefinition& definition : _tokenDefinitions)
            {
                if (_words.size() <= retInd) break;

                auto const& token = definition(_words.at(retInd));
                if (token.IsNull()) { continue; }
                else
                {
                    retToken = token;
                    retInd++;
                    break;
                }
            }

            return std::make_pair(retToken, retInd);
        } while (true);
    }
    catch (std::out_of_range e)
    {
        return std::make_pair(Token::CreateNullToken(), wordInd);
    }
}

Tokenizer::TokenIterator Tokenizer::begin()
{
    auto pair = GenerateToken(0);
    return TokenIterator(pair.first, this, pair.second);
}

Tokenizer::TokenIterator Tokenizer::end()
{
    return TokenIterator(Token::CreateNullToken(), this, 0);
}

Tokenizer& Tokenizer::AddTokenDefinition(TokenDefinition const& tokenDefinition)
{
    _tokenDefinitions.push_back(tokenDefinition);
    return *this;
}

bool Tokenizer::_IsDelimeter(char c)
{
    static const std::vector<char> _delimeters = { '\n', ' ' };
    return std::find(_delimeters.begin(), _delimeters.end(), c) != _delimeters.end();
}

bool Tokenizer::_IsPunctuationMark(char c)
{
    static const std::vector<char> _punctuationMarks = { ',', '(', ')' };
    return std::find(_punctuationMarks.begin(), _punctuationMarks.end(), c)
           != _punctuationMarks.end();
}

/* token iterator */

Tokenizer::TokenIterator::TokenIterator(Token const& token, Tokenizer* tokenizer, int wordInd) :
    token { token },
    tokenizer { tokenizer },
    wordInd { wordInd }
{}

Tokenizer::TokenIterator::TokenIterator(Tokenizer::TokenIterator const& iter) :
    token { iter.token },
    tokenizer { iter.tokenizer },
    wordInd { iter.wordInd }
{}

Tokenizer::TokenIterator::~TokenIterator()
{
    token = Token::CreateNullToken();
}

Tokenizer::TokenIterator& Tokenizer::TokenIterator::operator++()
{
    auto tuple = tokenizer->GenerateToken(wordInd);
    token      = tuple.first;
    wordInd    = tuple.second;
    return *this;
}

Tokenizer::TokenIterator Tokenizer::TokenIterator::operator++(int)
{
    Tokenizer::TokenIterator temp = *this;
    ++(*this);
    return temp;
}

Token const& Tokenizer::TokenIterator::operator*() const
{
    return token;
}

Token const* Tokenizer::TokenIterator::operator->() const
{
    return &token;
}

bool Tokenizer::TokenIterator::operator==(Tokenizer::TokenIterator const& other)
{
    return token == other.token;
}

bool Tokenizer::TokenIterator::operator!=(Tokenizer::TokenIterator const& other)
{
    return token != other.token;
}

Tokenizer::TokenIterator Tokenizer::TokenIterator::begin()
{
    return *this;
}

Tokenizer::TokenIterator Tokenizer::TokenIterator::end()
{
    return Tokenizer::TokenIterator(Token::CreateNullToken(), nullptr, 0);
}