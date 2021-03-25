#ifndef __H_CODE_GENERATOR__
#define __H_CODE_GENERATOR__
#include "ISerializer.hh"

struct CodeGenerator : public ISerializer
{
    virtual std::string Serialize(UnionInstruction inst);
    virtual std::string Serialize(Data data);
};
#endif