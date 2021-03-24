#ifndef __H_OFFSET_ADDRESS__
#define __H_OFFSET_ADDRESS__

#include "Element.hh"

struct OffsetAddress : public Element
{
    int                 numRegister;
    int                 offset;
    virtual ElementType getType()
    {
        return ElementType::OFFSET_ADDRESS;
    }
    OffsetAddress(int numRegister, int offset) :
        Element(ElementType::OFFSET_ADDRESS),
        numRegister { numRegister },
        offset { offset }
    {}
};

#endif