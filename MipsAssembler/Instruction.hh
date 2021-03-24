#ifndef __H_INSTRUCTION__
#define __H_INSTRUCTION__
#include "Address.hh"
#include "Element.hh"
#include <optional>
#include <variant>

enum class InstructionId
{
    INST_ADDIU,
    INST_ADDU,
    INST_AND,
    INST_ANDI,
    INST_BEQ,
    INST_BNE,
    INST_J,
    INST_JAL,
    INST_JR,
    INST_LUI,
    INST_LW,
    INST_LA,
    INST_NOR,
    INST_OR,
    INST_ORI,
    INST_SLTIU,
    INST_SLTU,
    INST_SLL,
    INST_SRL,
    INST_SW,
    INST_SUBU,
    INST_LB,
    INST_SB,
    INST_NULL,
};

enum class InstructionFormat
{
    R_FORMAT,
    J_FORMAT,
    I_FORMAT,
};

struct InstructionR
{
    int registerSrc1;
    int registerSrc2;
    int destOrValue;
};

struct InstructionI
{
    int registerSrc;
    int registerDest;
    int immediate;
};

struct InstructionJ
{
    UnionAddress target;
    InstructionJ(UnionAddress target) : target { target } {}
};

using UnionInstructionType = std::variant<InstructionR, InstructionI, InstructionJ>;

struct UnionInstruction : public Element
{
    InstructionId              id;
    InstructionFormat          format;
    std::optional<std::string> label;
    virtual ElementType        getType()
    {
        return ElementType::INSTRUCTION;
    }

    UnionInstructionType instruction;
    UnionInstruction(InstructionId              id,
                     InstructionFormat          format,
                     UnionInstructionType       type,
                     std::optional<std::string> label = std::nullopt) :
        Element { ElementType::INSTRUCTION },
        id { id },
        format { format },
        instruction { type },
        label { label }
    {}
};

#endif