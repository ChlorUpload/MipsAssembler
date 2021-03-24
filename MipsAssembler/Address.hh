#ifndef __H_ADDRESS__
#define __H_ADDRESS__
#include "Element.hh"
#include <string>
#include <variant>

struct ConstantAddress
{
    int address;
};

struct LabelAddress
{
    std::string labelId;
};

using UnionAddressType = std::variant<ConstantAddress, LabelAddress>;

struct UnionAddress : public Element
{
    UnionAddressType    element;
    virtual ElementType getType()
    {
        return ElementType::ADDRESS;
    }
    UnionAddress(UnionAddressType element) : Element(ElementType::ADDRESS), element { element } {}
};

#endif