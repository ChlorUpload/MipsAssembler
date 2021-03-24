#include "ElementDefinition.hh"

#include "Address.hh"
#include "NotImplementedException.hh"
#include "OffsetAddress.hh"
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
        Tokenizer::TokenIterator   copyIter(iter);
        UnionElementType           elementType = NullElement();
        std::optional<std::string> label       = std::nullopt;

        auto labelOrNull = parser->GetNextElement(ElementType::TOKEN_LABEL, copyIter);
        if (!labelOrNull.IsNull())
        { label = std::get<TokenElement>(labelOrNull.element).token.value; }

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
                    InstructionI       instI;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) { instI.registerSrc = res.value(); }
                    else
                        break;
                    auto offsetAddressOrNull
                        = parser->GetNextElement(ElementType::OFFSET_ADDRESS, copyIter);
                    if (!offsetAddressOrNull.IsNull())
                    {
                        // offsetAddress가 등장한 경우
                        auto const& oa     = std::get<OffsetAddress>(offsetAddressOrNull.element);
                        instI.registerDest = oa.numRegister;
                        instI.immediate    = oa.offset;
                    }
                    else
                    {
                        std::optional<int> res = _RequireRegister(parser, copyIter);
                        if (res.has_value()) { instI.registerDest = res.value(); }
                        else
                            break;
                        res = _RequireConstant(parser, copyIter);
                        if (res.has_value()) { instI.immediate = res.value(); }
                        else
                            break;
                    }

                    auto instruction = UnionInstruction(id, format, instI, label);
                    elementType      = instruction;
                    iter             = copyIter;
                    break;
                }
                case InstructionFormat::J_FORMAT:
                {
                    auto addressOrNull = parser->GetNextElement(ElementType::ADDRESS, copyIter);
                    if (addressOrNull.IsNull()) break;
                    auto address     = std::get<UnionAddress>(addressOrNull.element);
                    auto instruction = UnionInstruction(id, format, InstructionJ(address), label);
                    elementType      = instruction;
                    iter             = copyIter;
                    break;
                }
                case InstructionFormat::R_FORMAT:
                {

                    InstructionR       instR;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) { instR.registerSrc1 = res.value(); }
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) { break; }
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) { instR.registerSrc2 = res.value(); }
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) { break; }
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) { instR.destOrValue = res.value(); }
                    else
                    {
                        res = _RequireConstant(parser, copyIter);
                        if (res.has_value()) { instR.destOrValue = res.value(); }
                        else
                            break;
                    }
                    auto instruction = UnionInstruction(id, format, instR, label);
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

ElementDefinition ElementDefinitionFactory::CreateOffsetAddressDefinition()
{
    return ElementDefinition([](Tokenizer::TokenIterator& iter, Parser* parser) {
        Tokenizer::TokenIterator copyIter(iter);
        UnionElementType         element = NullElement();

        int                offset, numRegister;
        std::optional<int> res = _RequireConstant(parser, copyIter);
        if (res.has_value()) { offset = res.value(); }
        else
            goto end;
        if (!_Require(ElementType::TOKEN_BRAKET_LEFT, parser, copyIter)) goto end;
        res = _RequireRegister(parser, copyIter);
        if (res.has_value()) { numRegister = res.value(); }
        else
            goto end;
        if (_Require(ElementType::TOKEN_BRAKET_RIGHT, parser, copyIter)) goto end;

        element = OffsetAddress(numRegister, offset);
        iter    = copyIter;
end:
        return UnionElement(element);
    });
}

ElementDefinition ElementDefinitionFactory::CreateAddressDefinition()
{
    return ElementDefinition([](Tokenizer::TokenIterator& iter, Parser* parser) {
        Tokenizer::TokenIterator copyIter(iter);
        UnionElementType         element = NullElement();

        std::optional<int> res = _RequireConstant(parser, copyIter);
        if (res.has_value())
        {
            ConstantAddress constantAddress;
            constantAddress.address = res.value();
            element                 = UnionAddress(constantAddress);
            iter                    = copyIter;
        }
        else
        {
            auto labelIdOrNull = parser->GetNextElement(ElementType::TOKEN_LABEL_ID, copyIter);
            if (!labelIdOrNull.IsNull())
            {
                auto         labelId = std::get<TokenElement>(labelIdOrNull.element).token.value;
                LabelAddress labelAddress;
                labelAddress.labelId = labelId;
                element              = UnionAddress(labelAddress);
                iter                 = copyIter;
            }
        }

        return UnionElement(element);
    });
}

/* private methods */

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

std::optional<int> ElementDefinitionFactory::_RequireConstant(Parser*                   parser,
                                                              Tokenizer::TokenIterator& iter)
{
    auto constantOrNull = parser->GetNextElement(ElementType::TOKEN_CONST, iter);
    if (!constantOrNull.IsNull())
    { return _Atoi(std::get<TokenElement>(constantOrNull.element).token.value); }
    else
    {
        return std::nullopt;
    }
}

std::optional<int> ElementDefinitionFactory::_RequireRegister(Parser*                   parser,
                                                              Tokenizer::TokenIterator& iter)
{
    auto registerOrNull = parser->GetNextElement(ElementType::TOKEN_REGISTER, iter);
    if (!registerOrNull.IsNull())
    { return _Atoi(std::get<TokenElement>(registerOrNull.element).token.value); }
    else
    {
        return std::nullopt;
    }
}
