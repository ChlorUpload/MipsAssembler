#include "Instruction.hh"

#include "NotImplementedException.hh"

Instruction::Instruction(Instruction&& inst) : id { inst.id } {}
Instruction::Instruction(InstructionId id) : id { id } {}

InstructionId Instruction::GetInstructionId(Token token)
{
    if (token.IsInstruction()) { return static_cast<InstructionId>(token.type); }
    else
    {
        throw NotImplementedException();
    }
}

Instruction Instruction::CreateNullInstruction()
{
    return Instruction(InstructionId::INST_NULL);
}

bool Instruction::IsNull() {
    return id != InstructionId::INST_NULL;
}