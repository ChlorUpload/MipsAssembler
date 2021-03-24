#ifndef __H_I_SERIALIZER__
#define __H_I_SERIALIZER__
#include "UnionElement.hh"

class ISerializer
{
  public:
    virtual std::string Serialize(UnionInstruction inst) = 0;
};

#endif