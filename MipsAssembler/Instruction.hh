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
    INST_EMPTY,
};

enum class InstructionType
{
    R_FORMAT,
    R_FORMAT_SHAMT,
    R_FORMAT_JR,

    I_FORMAT,
    I_FORMAT_OFFSET,
    I_FORMAT_BRANCH,
    I_FORMAT_LUI,

    J_FORMAT,

    EMPTY,
};

struct InstructionR
{
    int rd;
    int rs;
    int rt;
};

struct InstructionRShamt
{
    int rd;
    int rt;
    int shamt;
};

struct InstructionRJr
{
    int rs;
};

struct InstructionI
{
    int rt;
    int rs;
    int immediate;
};

struct InstructionIOffset
{
    int rt;
    int rs;
    int offset;
};

struct InstructionIBranch
{
    int          rt;
    int          rs;
    UnionAddress branch;
    InstructionIBranch(int rt, int rs, UnionAddress branch) :
        rt { rt },
        rs { rs },
        branch { branch }
    {}
};

struct InstructionILui
{
    int rt;
    int immediate;
};

struct InstructionJ
{
    UnionAddress target;
    InstructionJ(UnionAddress target) : target { target } {}
};

struct InstructionEmpty
{};

using UnionInstructionType = std::variant<InstructionR,
                                          InstructionRShamt,
                                          InstructionRJr,
                                          InstructionI,
                                          InstructionIOffset,
                                          InstructionIBranch,
                                          InstructionILui,
                                          InstructionJ,
                                          InstructionEmpty>;

struct UnionInstruction : public Element
{
    InstructionId              id;
    InstructionType            instType;
    std::optional<std::string> label;
    virtual ElementType        getType()
    {
        return ElementType::INSTRUCTION;
    }

    UnionInstructionType instruction;
    UnionInstruction(InstructionId              id,
                     InstructionType            instType,
                     UnionInstructionType       instruction,
                     std::optional<std::string> label = std::nullopt) :
        Element { ElementType::INSTRUCTION },
        id { id },
        instType { instType },
        instruction { instruction },
        label { label }
    {}
};

#endif