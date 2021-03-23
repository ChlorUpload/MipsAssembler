#include "Parser.hh"
#include "ElementDefinition.hh"
#include "Element.hh"

Parser& Parser::AddElementDefinition(ElementType type, ElementDefinition const& definition)
{
    if (_definitions.find(type) == _definitions.end())
    {
        std::vector<ElementDefinition> vec = { definition };
        _definitions.insert(std::make_pair(type, vec));
    }
    else
    {
        _definitions.at(type).push_back(definition);
    }

    return *this;
}

UnionElement Parser::GetNextElement(ElementType type, Tokenizer::TokenIterator& iter)
{
    UnionElement ret = UnionElement(UnionElementType(NullElement()));

    if (_definitions.find(type) != _definitions.end())
    {
        for (auto& def : _definitions.at(type))
        {
            ret = def.getNext(iter, this);
            if (!ret.IsNull()) break;
        }
    }

    return ret;
}