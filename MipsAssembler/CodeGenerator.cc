#include "CodeGenrator.hh"
#include "NotImplementedException.hh"
#include <sstream>
#include <unordered_map>

CodeGenerator::CodeGenerator() {}

std::string CodeGenerator::Serialize(UnionInstruction inst)
{
    switch (inst.instType)
    {
    case InstructionType::R_FORMAT:
        return Serialize(std::get<InstructionR>(inst.instruction), inst);
    case InstructionType::R_FORMAT_SHAMT:
        return Serialize(std::get<InstructionRShamt>(inst.instruction), inst);
    case InstructionType::R_FORMAT_JR:
        return Serialize(std::get<InstructionRJr>(inst.instruction), inst);
    case InstructionType::I_FORMAT:
        return Serialize(std::get<InstructionI>(inst.instruction), inst);
    case InstructionType::I_FORMAT_OFFSET:
        return Serialize(std::get<InstructionIOffset>(inst.instruction), inst);
    case InstructionType::I_FORMAT_ADDRESS:
        return Serialize(std::get<InstructionIAddress>(inst.instruction), inst);
    case InstructionType::I_FORMAT_LUI:
        return Serialize(std::get<InstructionILui>(inst.instruction), inst);
    case InstructionType::J_FORMAT:
        return Serialize(std::get<InstructionJ>(inst.instruction), inst);

    case InstructionType::I_FORMAT_LA:
    case InstructionType::EMPTY: throw NotImplementedException();
    }
}
std::string CodeGenerator::Serialize(Data data)
{
    return _HexToStr(data.value);
}
std::string CodeGenerator::Serialize(InstructionR instR, UnionInstruction inst)
{
    static const std::unordered_map<InstructionId, int> functMap
        = { { InstructionId::INST_ADDU, 0x21 }, { InstructionId::INST_AND, 0x24 },
            { InstructionId::INST_NOR, 0x27 },  { InstructionId::INST_OR, 0x25 },
            { InstructionId::INST_SLTU, 0x2b }, { InstructionId::INST_SUBU, 0x23 } };

    auto const& iter = functMap.find(inst.id);
    if (iter == functMap.end()) throw NotImplementedException();
    return _HexToStr(_MakeRInstruction(instR.rs, instR.rt, instR.rd, 0, iter->second));
}
std::string CodeGenerator::Serialize(InstructionRShamt instRshamt, UnionInstruction inst)
{
    static const std::unordered_map<InstructionId, int> functMap
        = { { InstructionId::INST_SLL, 0x00 }, { InstructionId::INST_SRL, 0x02 } };

    auto const& iter = functMap.find(inst.id);
    if (iter == functMap.end()) throw NotImplementedException();
    return _HexToStr(
        _MakeRInstruction(0, instRshamt.rt, instRshamt.rd, instRshamt.shamt, iter->second));
}
std::string CodeGenerator::Serialize(InstructionRJr instRJr, UnionInstruction inst)
{
    return _HexToStr(_MakeRInstruction(0, instRJr.rs, 0, 0, 0x08));
}
std::string CodeGenerator::Serialize(InstructionI instI, UnionInstruction inst)
{
    static const std::unordered_map<InstructionId, int> opMap
        = { { InstructionId::INST_ADDIU, 0x09 },
            { InstructionId::INST_ANDI, 0x0c },
            { InstructionId::INST_ORI, 0x0d },
            { InstructionId::INST_SLTIU, 0x0b } };

    auto const& iter = opMap.find(inst.id);
    if (iter == opMap.end()) throw NotImplementedException();

    unsigned int immediate = instI.immediate;

    if (iter->first == InstructionId::INST_ADDIU || iter->first == InstructionId::INST_SLTIU)
    {
        if (instI.immediate > 32767 || instI.immediate < -32768) throw NotImplementedException();
        else
        {
            immediate = 0x0000FFFF & instI.immediate;
        }
    }

    return _HexToStr(_MakeIInstruction(iter->second, instI.rs, instI.rt, immediate));
}
std::string CodeGenerator::Serialize(InstructionIOffset instIOffset, UnionInstruction inst)
{
    static const std::unordered_map<InstructionId, int> opMap
        = { { InstructionId::INST_LW, 0x23 },
            { InstructionId::INST_LB, 0x20 },
            { InstructionId::INST_SW, 0x2b },
            { InstructionId::INST_SB, 0x28 } };

    auto const& iter = opMap.find(inst.id);
    if (iter == opMap.end()) throw NotImplementedException();

    return _HexToStr(
        _MakeIInstruction(iter->second, instIOffset.rs, instIOffset.rt, instIOffset.offset));
}
std::string CodeGenerator::Serialize(InstructionIAddress instIAddress, UnionInstruction inst)
{
    static const std::unordered_map<InstructionId, int> opMap
        = { { InstructionId::INST_BEQ, 0x04 }, { InstructionId::INST_BNE, 0x05 } };

    auto const& iter = opMap.find(inst.id);
    if (iter == opMap.end()) throw NotImplementedException();
    unsigned int address = std::get<ConstantAddress>(instIAddress.address.element).address;
    int          offset  = (address - inst.__selfAddress) >> 2;

    return _HexToStr(_MakeIInstruction(iter->second, instIAddress.rs, instIAddress.rt, offset));
}
std::string CodeGenerator::Serialize(InstructionILui instILui, UnionInstruction inst)
{
    return _HexToStr(_MakeIInstruction(0x0f, 0, instILui.rt, instILui.immediate));
}
std::string CodeGenerator::Serialize(InstructionJ instJ, UnionInstruction inst)
{
    static const std::unordered_map<InstructionId, int> opMap
        = { { InstructionId::INST_J, 0x02 }, { InstructionId::INST_JAL, 0x03 } };

    auto const& iter = opMap.find(inst.id);
    if (iter == opMap.end()) throw NotImplementedException();
    auto address = std::get<ConstantAddress>(instJ.target.element).address;
    return _HexToStr(_MakeJInstruction(iter->second, address));
}

std::string CodeGenerator::_HexToStr(unsigned int hex)
{
    std::ostringstream ss;
    ss << "0x" << std::hex << hex;
    return ss.str();
}

unsigned int CodeGenerator::_MakeRInstruction(unsigned int rs,
                                              unsigned int rt,
                                              unsigned int rd,
                                              unsigned int shamt,
                                              unsigned int funct)
{
    unsigned int ret = 0;
    ret |= rs;
    ret <<= 5;
    ret |= rt;
    ret <<= 5;
    ret |= rd;
    ret <<= 5;
    ret |= shamt;
    ret <<= 6;
    ret |= funct;
    return ret;
}

unsigned int CodeGenerator::_MakeIInstruction(unsigned int op,
                                              unsigned int rs,
                                              unsigned int rt,
                                              unsigned int immediate)
{
    unsigned int ret = op;
    ret <<= 5;
    ret |= rs;
    ret <<= 5;
    ret |= rt;
    ret <<= 16;
    ret |= immediate;
    return ret;
}

unsigned int CodeGenerator::_MakeJInstruction(unsigned int op, unsigned int target)
{
    unsigned int ret = op;
    ret <<= 26;
    ret |= target;
    return ret;
}