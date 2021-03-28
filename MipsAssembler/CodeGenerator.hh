#ifndef __H_CODE_GENERATOR__
#define __H_CODE_GENERATOR__
#include "ISerializer.hh"

/*
    CodeGenerator는 Address가 반드시 ConstantAddress라고 생각하고 작동합니다.
*/
class CodeGenerator : public ISerializer
{
  private:
    std::string  _HexToStr(unsigned int hex);
    unsigned int _MakeRInstruction(unsigned int rs,
                                   unsigned int rt,
                                   unsigned int rd,
                                   unsigned int shamt,
                                   unsigned int funct);
    unsigned int
    _MakeIInstruction(unsigned int op, unsigned int rs, unsigned int rt, unsigned int immediate);
    unsigned int _MakeJInstruction(unsigned int op, unsigned int target);

  public:
    virtual std::string Serialize(UnionInstruction inst);
    virtual std::string Serialize(Data data);

    std::string Serialize(InstructionR instR, UnionInstruction inst);
    std::string Serialize(InstructionRShamt isntRshamt, UnionInstruction inst);
    std::string Serialize(InstructionRJr isntRJr, UnionInstruction inst);
    std::string Serialize(InstructionI instI, UnionInstruction inst);
    std::string Serialize(InstructionIOffset instIOffset, UnionInstruction inst);
    std::string Serialize(InstructionIAddress instIAddress, UnionInstruction inst);
    std::string Serialize(InstructionILui instILui, UnionInstruction inst);
    std::string Serialize(InstructionJ instJ, UnionInstruction inst);

    CodeGenerator();
};
#endif