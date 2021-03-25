#ifndef __H_DATA__
#define __H_DATA__
#include "Element.hh"

struct Data : public Element
{
    int                        value;
    std::optional<std::string> label;
    virtual ElementType        getType()
    {
        return ElementType::DATA;
    }
    Data(int value, std::optional<std::string> label = std::nullopt) :
        Element(ElementType::DATA),
        label { label },
        value { value }
    {}
};

#endif