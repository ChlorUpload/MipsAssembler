#ifndef __H_TOKEN__
#define __H_TOKEN__
#include <string>

enum class TokenType
{
    INST_ADDIU,
    INST_ADDU,
    INST_AND,
    INST_ANDI,
    INST_BEQ,
    INST_BNE,
    INST_J,
    INST_JAL,
    INST_JR,
    INST_LUI,
    INST_LW,
    INST_LA,
    INST_NOR,
    INST_OR,
    INST_ORI,
    INST_SLTIU,
    INST_SLTU,
    INST_SLL,
    INST_SRL,
    INST_SW,
    INST_SUBU,
    INST_LB,
    INST_SB,
    DIRE_TEXT,
    DIRE_DATA,
    DIRE_WORD,
    CONST_DECI,
    CONST_HEX,
    REGISTER,
    LABEL,
    LABEL_ID,
    COMMA,
    BRAKET_LEFT,
    BRAKET_RIGHT,
    NULL_TOKEN,
};

class Token
{
  private:
    Token(TokenType type, std::string const& value) : type { type }, value { value } {}

  public:
    TokenType   type;
    std::string value;

    Token(Token const& token)
    {
        type  = token.type;
        value = token.value;
    }

    Token() : type { TokenType::NULL_TOKEN }, value { "" } {}

    bool IsNull() const
    {
        return type == TokenType::NULL_TOKEN;
    }

    static Token CreateToken(TokenType type, std::string const& value)
    {
        return Token(type, value);
    }

    static Token CreateNullToken()
    {
        return Token(TokenType::NULL_TOKEN, "");
    }

    bool operator==(Token const& other)
    {
        return type == other.type && value == other.value;
    }

    bool operator!=(Token const& other)
    {
        return type != other.type || value != other.value;
    }

    bool IsInstruction() const
    {
        return TokenType::INST_ADDIU <= type && type <= TokenType::INST_SB;
    }

    bool IsDirectives() const
    {
        return TokenType::DIRE_TEXT <= type && type <= TokenType::DIRE_WORD;
    }

    bool IsConstant() const
    {
        return TokenType::CONST_DECI <= type && type <= TokenType::CONST_HEX;
    }
};

#endif