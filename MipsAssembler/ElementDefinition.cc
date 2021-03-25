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

ElementDefinition ElementDefinitionFactory::CreateInstructionDefinition(InstructionId   id,
                                                                        InstructionType type)
{
    return ElementDefinition([=](Tokenizer::TokenIterator& iter, Parser* parser) {
        Tokenizer::TokenIterator   copyIter(iter);
        UnionElementType           elementType = NullElement();
        std::optional<std::string> label       = std::nullopt;

        auto labelOrNull = parser->GetNextElement(ElementType::TOKEN_LABEL, copyIter);
        if (!labelOrNull.IsNull())
        {
            label = std::get<TokenElement>(labelOrNull.element).token.value;
        }

        auto instOrNull = parser->GetNextElement(ElementType::TOKEN_INST_ID, copyIter);
        if (!instOrNull.IsNull())
        {
            Token inst = std::get<TokenElement>(instOrNull.element).token;
            if (inst.IsInstruction() && _TokenTypeToInstructionId(inst) == id)
            {
                switch (type)
                {
                case InstructionType::R_FORMAT:
                {
                    InstructionR       instR;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instR.rd = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instR.rs = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instR.rt = res.value();
                    else
                        break;

                    elementType = UnionElementType(UnionInstruction(id, type, instR, label));
                    iter        = copyIter;
                    break;
                }
                case InstructionType::R_FORMAT_SHAMT:
                {
                    InstructionRShamt  instRShamt;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instRShamt.rd = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instRShamt.rt = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireConstant(parser, copyIter);
                    if (res.has_value()) instRShamt.shamt = res.value();
                    else
                        break;

                    elementType = UnionElementType(UnionInstruction(id, type, instRShamt, label));
                    iter        = copyIter;
                    break;
                }
                case InstructionType::R_FORMAT_JR:
                {
                    InstructionRJr     instRJr;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instRJr.rs = res.value();
                    else
                        break;

                    elementType = UnionElementType(UnionInstruction(id, type, instRJr, label));
                    iter        = copyIter;
                    break;
                }
                case InstructionType::I_FORMAT:
                {
                    InstructionI       instI;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instI.rt = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instI.rs = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireConstant(parser, copyIter);
                    if (res.has_value()) instI.immediate = res.value();
                    else
                        break;

                    elementType = UnionInstruction(id, type, instI, label);
                    iter        = copyIter;
                    break;
                }
                case InstructionType::I_FORMAT_OFFSET:
                {
                    InstructionIOffset instIOffset;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instIOffset.rt = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    auto offsetAddressOrNull
                        = parser->GetNextElement(ElementType::OFFSET_ADDRESS, copyIter);
                    if (offsetAddressOrNull.IsNull()) break;
                    auto const& offsetAddress
                        = std::get<OffsetAddress>(offsetAddressOrNull.element);
                    instIOffset.rs     = offsetAddress.numRegister;
                    instIOffset.offset = offsetAddress.offset;

                    elementType = UnionInstruction(id, type, instIOffset, label);
                    iter        = copyIter;
                    break;
                }
                case InstructionType::I_FORMAT_ADDRESS:
                {
                    int                rt, rs;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) rt = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) rs = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    auto addressOrNull = parser->GetNextElement(ElementType::ADDRESS, copyIter);
                    if (addressOrNull.IsNull()) break;
                    auto const& address = std::get<UnionAddress>(addressOrNull.element);

                    elementType
                        = UnionInstruction(id, type, InstructionIAddress(rt, rs, address), label);
                    iter = copyIter;
                    break;
                }
                case InstructionType::I_FORMAT_LUI:
                {
                    InstructionILui    instILui;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) instILui.rt = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    res = _RequireConstant(parser, copyIter);
                    if (res.has_value()) instILui.immediate = res.value();
                    else
                        break;

                    elementType = UnionInstruction(id, type, instILui, label);
                    iter        = copyIter;
                    break;
                }
                case InstructionType::I_FORMAT_LA:
                {
                    int                rt;
                    std::optional<int> res = _RequireRegister(parser, copyIter);
                    if (res.has_value()) rt = res.value();
                    else
                        break;
                    if (!_Require(ElementType::TOKEN_COMMA, parser, copyIter)) break;
                    auto addressOrNull = parser->GetNextElement(ElementType::ADDRESS, copyIter);
                    if (addressOrNull.IsNull()) break;
                    auto const& address = std::get<UnionAddress>(addressOrNull.element);

                    elementType = UnionInstruction(id, type, InstructionILa(rt, address), label);
                    iter        = copyIter;
                    break;
                }
                case InstructionType::J_FORMAT:
                {
                    auto addressOrNull = parser->GetNextElement(ElementType::ADDRESS, copyIter);
                    if (addressOrNull.IsNull()) break;
                    auto address = std::get<UnionAddress>(addressOrNull.element);

                    elementType = UnionInstruction(id, type, InstructionJ(address), label);
                    iter        = copyIter;
                    break;
                }
                }
            }
        }
        else if (label.has_value())
        {
            elementType = UnionInstruction(
                InstructionId::INST_EMPTY, InstructionType::EMPTY, InstructionEmpty(), label);
            iter = copyIter;
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
        if (!_Require(ElementType::TOKEN_BRAKET_RIGHT, parser, copyIter)) goto end;

        element = OffsetAddress(numRegister, offset);
        iter    = copyIter;
end:
        return UnionElement(element);
    });
}

ElementDefinition ElementDefinitionFactory::CreateLabelAddressDefinition()
{
    return ElementDefinition([](Tokenizer::TokenIterator& iter, Parser* parser) {
        Tokenizer::TokenIterator copyIter(iter);
        UnionElementType         element = NullElement();

        auto labelIdOrNull = parser->GetNextElement(ElementType::TOKEN_LABEL_ID, copyIter);
        if (!labelIdOrNull.IsNull())
        {
            auto         labelId = std::get<TokenElement>(labelIdOrNull.element).token.value;
            LabelAddress labelAddress;
            labelAddress.labelId = labelId;
            element              = UnionAddress(labelAddress);
            iter                 = copyIter;
        }

        return UnionElement(element);
    });
}

ElementDefinition ElementDefinitionFactory::CreateDataDefinition()
{
    return ElementDefinition([](Tokenizer::TokenIterator& iter, Parser* parser) {
        Tokenizer::TokenIterator   copyIter(iter);
        UnionElementType           element = NullElement();
        std::optional<std::string> label   = std::nullopt;

        auto labelOrNull = parser->GetNextElement(ElementType::TOKEN_LABEL, copyIter);
        if (!labelOrNull.IsNull())
        {
            label = std::get<TokenElement>(labelOrNull.element).token.value;
        }

        if (_Require(ElementType::TOKEN_DIRE_WORD, parser, copyIter))
        {
            std::optional<int> res = _RequireConstant(parser, copyIter);
            if (res.has_value())
            {
                element = Data(res.value(), label);
                iter    = copyIter;
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
    {
        return _Atoi(std::get<TokenElement>(constantOrNull.element).token.value);
    }
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
    {
        return _Atoi(std::get<TokenElement>(registerOrNull.element).token.value);
    }
    else
    {
        return std::nullopt;
    }
}
