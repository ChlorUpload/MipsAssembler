#include "ElementDefinition.hh"

#include "NotImplementedException.hh"
#include <sstream>
#include <variant>

ElementDefinition ElementDefinitionFactory::CreateTokenElementDefinition(TokenType   tokenType,
                                                                         ElementType elemType)
{
    return ElementDefinition([=](Tokenizer::TokenIterator& iter, Parser* parser) {
        if (iter->type == tokenType)
        {
            Token token = *iter;
            ++iter;
            return UnionElement(TokenElement(elemType, token));
        }
        else
        {
            return UnionElement(NullElement());
        }
    });
}

ElementDefinition ElementDefinitionFactory::CreateInstructionDefinition(InstructionId     id,
                                                                        InstructionFormat format)
{
    return ElementDefinition([=](Tokenizer::TokenIterator& iter, Parser* parser) {
        Tokenizer::TokenIterator copyIter(iter);
        UnionElementType         elementType = NullElement();
        std::string              label;

        auto labelOrNull = parser->GetNextElement(ElementType::TOKEN_LABEL, copyIter);
        if (!labelOrNull.IsNull())
        { label = std::get<TokenElement>(labelOrNull.element).token.value; }
        else
        {
            label = "";
        }

        auto instOrNull = parser->GetNextElement(ElementType::TOKEN_INST_ID, copyIter);
        if (!instOrNull.IsNull())
        {
            Token inst = std::get<TokenElement>(instOrNull.element).token;
            if (inst.IsInstruction() && _TokenTypeToInstructionId(inst) == id)
            {
                switch (format)
                {
                case InstructionFormat::I_FORMAT:
                {

                    break;
                }
                case InstructionFormat::J_FORMAT: break;
                case InstructionFormat::R_FORMAT:
                {

                    InstructionR instR;

                    auto registerOrNull
                        = parser->GetNextElement(ElementType::TOKEN_REGISTER, copyIter);
                    if (!registerOrNull.IsNull())
                    {
                        instR.registerSrc1
                            = _Atoi(std::get<TokenElement>(registerOrNull.element).token.value);
                    }
                    else
                    {
                        break;
                    }
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) { break; }
                    registerOrNull = parser->GetNextElement(ElementType::TOKEN_REGISTER, copyIter);
                    if (!registerOrNull.IsNull())
                    {
                        instR.registerSrc2
                            = _Atoi(std::get<TokenElement>(registerOrNull.element).token.value);
                    }
                    else
                    {
                        break;
                    }
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) { break; }
                    registerOrNull = parser->GetNextElement(ElementType::TOKEN_REGISTER, copyIter);
                    if (!registerOrNull.IsNull())
                    {
                        instR.destOrValue
                            = _Atoi(std::get<TokenElement>(registerOrNull.element).token.value);
                    }
                    else
                    {
                        auto constantOrNull
                            = parser->GetNextElement(ElementType::TOKEN_CONST, copyIter);
                        if (!constantOrNull.IsNull())
                        {
                            instR.destOrValue
                                = _Atoi(std::get<TokenElement>(constantOrNull.element).token.value);
                        }
                        else
                        {
                            break;
                        }
                    }
                    auto instruction = UnionInstruction(id, format, instR);
                    elementType      = instruction;
                    iter             = copyIter;
                    break;
                }
                }
            }
        }

        return UnionElement(elementType);
    });
}

InstructionId ElementDefinitionFactory::_TokenTypeToInstructionId(Token token)
{
    if (token.IsInstruction()) return static_cast<InstructionId>(token.type);
    else
        throw NotImplementedException();
}

int ElementDefinitionFactory::_Atoi(std::string str)
{
    try
    {
        int                n;
        std::istringstream ss(str);
        ss >> n;

        return n;
    }
    catch (...)
    {
        throw new NotImplementedException();
    }
}

bool ElementDefinitionFactory::_Require(ElementType               type,
                                        Parser*                   parser,
                                        Tokenizer::TokenIterator& iter)
{
    auto elementOrNull = parser->GetNextElement(type, iter);
    return !elementOrNull.IsNull();
}