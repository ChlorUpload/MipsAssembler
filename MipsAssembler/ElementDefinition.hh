#ifndef __H_ELEMENT_DEFINITION__
#define __H_ELEMENT_DEFINITION__
#include "Element.hh"
#include "Instruction.hh"
#include "Parser.hh"
#include "Tokenizer.hh"
#include "UnionElement.hh"
#include <functional>

using GetNext = std::function<UnionElement(Tokenizer::TokenIterator&, Parser*)>;

struct ElementDefinition
{
    GetNext getNext;
    ElementDefinition(GetNext getNext) : getNext { std::move(getNext) } {}
};

class ElementDefinitionFactory
{
  private:
    static InstructionId _TokenTypeToInstructionId(Token token);
    static bool          _Require(ElementType type, Parser* parser, Tokenizer::TokenIterator& iter);
    static std::optional<int> _RequireConstant(Parser* parser, Tokenizer::TokenIterator& iter);
    static std::optional<int> _RequireRegister(Parser* parser, Tokenizer::TokenIterator& iter);
    static int                _Atoi(std::string str);

  public:
    static ElementDefinition CreateTokenElementDefinition(TokenType   tokenType,
                                                          ElementType elemType);
    static ElementDefinition CreateInstructionDefinition(InstructionId id, InstructionType type);
    static ElementDefinition CreateOffsetAddressDefinition();
    static ElementDefinition CreateAddressDefinition();
    static ElementDefinition CreateDataDefinition();
};

#endif