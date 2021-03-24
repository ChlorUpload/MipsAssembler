#include "ReadableSerializer.hh"

#include "NotImplementedException.hh"
#include <sstream>
#include <vector>

std::string InstructionIdToStr(InstructionId id)
{
    static std::vector<std::string> instNames
        = { "ADDIU", "ADDU", "AND", "ANDI",  "BEQ",  "BNE", "J",   "JAL", "JR",   "LUI", "LW", "LA",
            "NOR",   "OR",   "ORI", "SLTIU", "SLTU", "SLL", "SRL", "SW",  "SUBU", "LB",  "SB" };

    return instNames[(int)id];
}

std::string ReadableSerializer::Serialize(UnionInstruction inst)
{
    std::ostringstream ss;
    ss << "Instruction ID : " << InstructionIdToStr(inst.id) << std::endl;
    ss << "Label : " << inst.label.value_or("No label") << std::endl;

    switch (inst.format)
    {
    case InstructionFormat::I_FORMAT:
        ss << Serialize(std::get<InstructionI>(inst.instruction));
        break;
    case InstructionFormat::J_FORMAT:
        ss << Serialize(std::get<InstructionJ>(inst.instruction));
        break;
    case InstructionFormat::R_FORMAT:
        ss << Serialize(std::get<InstructionR>(inst.instruction));
        break;
    }

    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionI inst)
{
    std::ostringstream ss;
    ss << "regSrc : " << inst.registerSrc << "\t"
       << "regDesc : " << inst.registerDest << "\t"
       << "immediate : " << inst.immediate;
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionJ inst)
{
    std::ostringstream ss;
    ss << "target : " << Serialize(inst.target);
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionR inst)
{
    std::ostringstream ss;
    ss << "regSrc1 : " << inst.registerSrc1 << "\t"
       << "regSrc2 : " << inst.registerSrc2 << "\t"
       << "destOrValue : " << inst.destOrValue;
    return ss.str();
}

std::string ReadableSerializer::Serialize(UnionAddress address)
{
    std::ostringstream ss;
    switch (address.element.index())
    {
    case 0: // ConstantAddress
        ss << std::hex << std::get<ConstantAddress>(address.element).address;
        break;
    case 1: // LabelAddress
        ss << "LabelId : " << std::get<LabelAddress>(address.element).labelId;
        break;
    default: throw NotImplementedException();
    }
    return ss.str();
}