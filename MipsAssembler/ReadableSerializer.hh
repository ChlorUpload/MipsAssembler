#ifndef __H_READABLE_SERIALIZER__
#define __H_READABLE_SERIALIZER__
#include "ISerializer.hh"

class ReadableSerializer : public ISerializer
{
  private:
  public:
    virtual std::string Serialize(UnionInstruction inst);
    virtual std::string Serialize(Data data);

    std::string Serialize(InstructionR instR);
    std::string Serialize(InstructionRShamt isntRshamt);
    std::string Serialize(InstructionRJr isntRJr);
    std::string Serialize(InstructionI instI);
    std::string Serialize(InstructionIOffset instIOffset);
    std::string Serialize(InstructionIAddress instIAddress);
    std::string Serialize(InstructionILui instILui);
    std::string Serialize(InstructionILa instILa);
    std::string Serialize(InstructionJ instJ);
    std::string Serialize(UnionAddress address);
};

#endif