#ifndef __H_UNION_ELEMENT__
#define __H_UNION_ELEMENT__
#include "Element.hh"
#include "Instruction.hh"
#include "TokenElement.hh"
#include <variant>

using UnionElementType
    = std::variant<NullElement, UnionInstruction, TokenElement>;

struct UnionElement
{
    UnionElementType element;

    bool IsNull()
    {
        return element.index() == 0;
    }

    UnionElement(UnionElementType element) : element { element } {}
};

#endif