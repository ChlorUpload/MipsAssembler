#ifndef __H_PARSER__
#define __H_PARSER__
#include "Tokenizer.hh"
#include <unordered_map>
#include <vector>

enum class ElementType;
struct ElementDefinition;
struct UnionElement;

class Parser
{
  private:
    std::unordered_map<ElementType, std::vector<ElementDefinition>> _definitions;

  public:
    Parser&      AddElementDefinition(ElementType type, ElementDefinition const& definition);
    UnionElement GetNextElement(ElementType type, Tokenizer::TokenIterator& iter);
};

#endif