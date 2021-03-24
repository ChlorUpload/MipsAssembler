#ifndef __H_READABLE_SERIALIZER__
#define __H_READABLE_SERIALIZER__
#include "ISerializer.hh"

class ReadableSerializer : public ISerializer
{
  private:
  public:
    virtual std::string Serialize(UnionInstruction inst);
    virtual std::string Serialize(InstructionI instI);
    virtual std::string Serialize(InstructionJ instJ);
    virtual std::string Serialize(InstructionR instR);
    virtual std::string Serialize(UnionAddress address);
};

#endif