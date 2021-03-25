#include "ReadableSerializer.hh"

#include "NotImplementedException.hh"
#include <sstream>
#include <vector>

std::string InstructionIdToStr(InstructionId id)
{
    static std::vector<std::string> instNames
        = { "ADDIU", "ADDU", "AND",  "ANDI", "BEQ", "BNE",  "J",     "JAL",  "JR",
            "LUI",   "LW",   "LA",   "NOR",  "OR",  "ORI",  "SLTIU", "SLTU", "SLL",
            "SRL",   "SW",   "SUBU", "LB",   "SB",  "NULL", "EMPTY" };

    return instNames[(int)id];
}

std::string ReadableSerializer::Serialize(UnionInstruction inst)
{
    std::ostringstream ss;
    ss << "================================================" << std::endl;
    ss << "Instruction ID : " << InstructionIdToStr(inst.id) << std::endl;
    ss << "Label : " << inst.label.value_or("No label") << std::endl;

    switch (inst.instType)
    {
    case InstructionType::R_FORMAT:
        ss << Serialize(std::get<InstructionR>(inst.instruction));
        break;
    case InstructionType::R_FORMAT_SHAMT:
        ss << Serialize(std::get<InstructionRShamt>(inst.instruction));
        break;
    case InstructionType::R_FORMAT_JR:
        ss << Serialize(std::get<InstructionRJr>(inst.instruction));
        break;
    case InstructionType::I_FORMAT:
        ss << Serialize(std::get<InstructionI>(inst.instruction));
        break;
    case InstructionType::I_FORMAT_OFFSET:
        ss << Serialize(std::get<InstructionIOffset>(inst.instruction));
        break;
    case InstructionType::I_FORMAT_ADDRESS:
        ss << Serialize(std::get<InstructionIAddress>(inst.instruction));
        break;
    case InstructionType::I_FORMAT_LUI:
        ss << Serialize(std::get<InstructionILui>(inst.instruction));
        break;
    case InstructionType::I_FORMAT_LA:
        ss << Serialize(std::get<InstructionILa>(inst.instruction));
        break;
    case InstructionType::J_FORMAT:
        ss << Serialize(std::get<InstructionJ>(inst.instruction));
        break;
    case InstructionType::EMPTY: break;
    }

    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionRShamt isntRshamt)
{
    std::ostringstream ss;
    ss << "rd : " << isntRshamt.rd << "\t"
       << "rt : " << isntRshamt.rt << "\t"
       << "shamt : " << isntRshamt.shamt;
    return ss.str();
}
std::string ReadableSerializer::Serialize(InstructionRJr isntRJr)
{
    std::ostringstream ss;
    ss << "rs : " << isntRJr.rs;
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionI inst)
{
    std::ostringstream ss;
    ss << "rt : " << inst.rt << "\t"
       << "rs : " << inst.rs << "\t"
       << "immediate : " << inst.immediate;
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionIOffset instIOffset)
{
    std::ostringstream ss;
    ss << "rt : " << instIOffset.rt << "\t"
       << "rs : " << instIOffset.rs << "\t"
       << "offset : " << instIOffset.offset;
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionIAddress instIAddress)
{
    std::ostringstream ss;
    ss << "rt : " << instIAddress.rt << "\t"
       << "rs : " << instIAddress.rs << "\t"
       << "address : " << Serialize(instIAddress.address);
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionILui instILui)
{
    std::ostringstream ss;
    ss << "rt : " << instILui.rt << "\t"
       << "immediate : " << instILui.immediate;
    return ss.str();
}

std::string ReadableSerializer::Serialize(InstructionILa instILa)
{
    std::ostringstream ss;
    ss << "rt : " << instILa.rt << "\t"
       << "address : " << Serialize(instILa.address);
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
    ss << "rd : " << inst.rd << "\t"
       << "rs : " << inst.rs << "\t"
       << "rt : " << inst.rt;
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

std::string ReadableSerializer::Serialize(Data data)
{
    std::ostringstream ss;
    ss << "================================================" << std::endl;
    ss << "Label : " << data.label.value_or("No label") << std::endl;
    ss << "Value : " << data.value;
    return ss.str();
}