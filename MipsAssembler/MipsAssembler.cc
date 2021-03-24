#include "Element.hh"
#include "ElementDefinition.hh"
#include "Parser.hh"
#include "ReadableSerializer.hh"
#include "Token.hh"
#include "Tokenizer.hh"
#include <iostream>
#include <vector>

std::vector<std::pair<TokenType, std::vector<std::string>>> tokenDefs = {
    { TokenType::INST_ADDIU, { "ADDIU", "addiu" } },
    { TokenType::INST_ADDU, { "ADDU", "addu" } },
    { TokenType::INST_AND, { "AND", "and" } },
    { TokenType::INST_ANDI, { "ANDI", "andi" } },
    { TokenType::INST_BEQ, { "BEQ", "beq" } },
    { TokenType::INST_BNE, { "BNE", "bne" } },
    { TokenType::INST_J, { "J", "j" } },
    { TokenType::INST_JAL, { "JAL", "jal" } },
    { TokenType::INST_JR, { "JR", "jr" } },
    { TokenType::INST_LUI, { "LUI", "lui" } },
    { TokenType::INST_LW, { "LW", "lw" } },
    { TokenType::INST_LA, { "LA", "la" } },
    { TokenType::INST_NOR, { "NOR", "nor" } },
    { TokenType::INST_OR, { "OR", "or" } },
    { TokenType::INST_ORI, { "ORI", "ori" } },
    { TokenType::INST_SLTIU, { "SLTIU", "sltiu" } },
    { TokenType::INST_SLTU, { "SLTU", "sltu" } },
    { TokenType::INST_SLL, { "SLL", "sll" } },
    { TokenType::INST_SRL, { "SRL", "srl" } },
    { TokenType::INST_SW, { "SW", "sw" } },
    { TokenType::INST_SUBU, { "SUBU", "subu" } },
    { TokenType::INST_LB, { "LB", "lb" } },
    { TokenType::INST_SB, { "SB", "sb" } },
    { TokenType::DIRE_TEXT, { ".TEXT", ".text" } },
    { TokenType::DIRE_DATA, { ".DATA", ".data" } },
    { TokenType::DIRE_WORD, { ".WORD", ".word" } },
    { TokenType::COMMA, { "," } },
    { TokenType::BRAKET_LEFT, { "(" } },
    { TokenType::BRAKET_RIGHT, { ")" } },
};
std::vector<std::pair<TokenType, ElementType>> elemDefs = {
    { TokenType::CONST_DECI, ElementType::TOKEN_CONST },
    { TokenType::CONST_HEX, ElementType::TOKEN_CONST },
    { TokenType::REGISTER, ElementType::TOKEN_REGISTER },
    { TokenType::LABEL, ElementType::TOKEN_LABEL },
    { TokenType::LABEL_ID, ElementType::TOKEN_LABEL_ID },
    { TokenType::BRAKET_LEFT, ElementType::TOKEN_BRAKET_LEFT },
    { TokenType::BRAKET_RIGHT, ElementType::TOKEN_BRAKET_RIGHT },
    { TokenType::COMMA, ElementType::TOKEN_COMMA },
};
std::vector<std::tuple<TokenType, InstructionId, InstructionType>> instDefs = {
    { TokenType::INST_ADDIU, InstructionId::INST_ADDIU, InstructionType::I_FORMAT },
    { TokenType::INST_ADDU, InstructionId::INST_ADDU, InstructionType::R_FORMAT },
    { TokenType::INST_AND, InstructionId::INST_AND, InstructionType::R_FORMAT },
    { TokenType::INST_ANDI, InstructionId::INST_ANDI, InstructionType::I_FORMAT },
    { TokenType::INST_BEQ, InstructionId::INST_BEQ, InstructionType::I_FORMAT_BRANCH },
    { TokenType::INST_BNE, InstructionId::INST_BNE, InstructionType::I_FORMAT_BRANCH },
    { TokenType::INST_J, InstructionId::INST_J, InstructionType::J_FORMAT },
    { TokenType::INST_JAL, InstructionId::INST_JAL, InstructionType::J_FORMAT },
    { TokenType::INST_JR, InstructionId::INST_JR, InstructionType::R_FORMAT_JR },
    { TokenType::INST_LUI, InstructionId::INST_LUI, InstructionType::I_FORMAT_LUI },
    { TokenType::INST_LW, InstructionId::INST_LW, InstructionType::I_FORMAT_OFFSET },
    { TokenType::INST_LB, InstructionId::INST_LB, InstructionType::I_FORMAT_OFFSET },
    { TokenType::INST_NOR, InstructionId::INST_NOR, InstructionType::R_FORMAT },
    { TokenType::INST_OR, InstructionId::INST_OR, InstructionType::R_FORMAT },
    { TokenType::INST_ORI, InstructionId::INST_ORI, InstructionType::I_FORMAT },
    { TokenType::INST_SLTIU, InstructionId::INST_SLTIU, InstructionType::I_FORMAT },
    { TokenType::INST_SLTU, InstructionId::INST_SLTU, InstructionType::R_FORMAT },
    { TokenType::INST_SLL, InstructionId::INST_SLL, InstructionType::R_FORMAT_SHAMT },
    { TokenType::INST_SRL, InstructionId::INST_SRL, InstructionType::R_FORMAT_SHAMT },
    { TokenType::INST_SW, InstructionId::INST_SW, InstructionType::I_FORMAT_OFFSET },
    { TokenType::INST_SB, InstructionId::INST_SB, InstructionType::I_FORMAT_OFFSET },
    { TokenType::INST_SUBU, InstructionId::INST_SUBU, InstructionType::R_FORMAT },
};

int main()
{
    std::string str = R"===(
       addu    $10,$9,  $0 
       or    $4,  $3,  $2  
       srl    $11,$6,  5  
       lw $9, 0($8)
       addu $2, $0, $9
       jal sum
       j exit
empty_inst_label:
sum:    sltiu $1, $2, 1
        bne $1, $0, sum_exit
        addu $3, $3, $2
        addiu $2, $2, -1
        j sum
sum_exit:
        addu $4, $3, $0
        jr $31
exit:
        addu $10, $9, $0      
empty_inst_label2:

)===";

#pragma region Tokenizer

    Tokenizer tokenizer = Tokenizer(str);

    for (auto& tokenDef : tokenDefs)
    {
        tokenizer.AddTokenDefinition(
            TokenDefinitionFactory::CreateTokenDefinition(tokenDef.second, tokenDef.first));
    }
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::RegisterTokenDefinition);
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::LabelTokenDefinition);
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::ConstantTokenDefinition);
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::LabelIdTokenDefinition);

#pragma endregion

#pragma region Parser

    Parser parser;
    // token element를 추가
    for (auto& elemDef : elemDefs)
    {
        parser.AddElementDefinition(
            elemDef.second,
            ElementDefinitionFactory::CreateTokenElementDefinition(elemDef.first, elemDef.second));
    }

    for (auto& instDef : instDefs)
    {
        // token element 중 instruction을 뜻하는 element를 추가
        parser.AddElementDefinition(ElementType::TOKEN_INST_ID,
                                    ElementDefinitionFactory::CreateTokenElementDefinition(
                                        std::get<TokenType>(instDef), ElementType::TOKEN_INST_ID));
        // instruction element 추가
        parser.AddElementDefinition(
            ElementType::INSTRUCTION,
            ElementDefinitionFactory::CreateInstructionDefinition(
                std::get<InstructionId>(instDef), std::get<InstructionType>(instDef)));
    }

    parser.AddElementDefinition(ElementType::OFFSET_ADDRESS,
                                ElementDefinitionFactory::CreateOffsetAddressDefinition());
    parser.AddElementDefinition(ElementType::ADDRESS,
                                ElementDefinitionFactory::CreateAddressDefinition());

#pragma endregion

    ReadableSerializer serializer;

    // for (auto iter = tokenizer.begin(); iter != iter.end(); iter++)
    //{ std::cout << "Type : " << (int)iter->type << "\tValue : " << iter->value << std::endl; }

    for (auto iter = tokenizer.begin(); iter != iter.end();)
    {
        auto instructionOrNull = parser.GetNextElement(ElementType::INSTRUCTION, iter);
        if (instructionOrNull.IsNull())
        {
            std::cout << "Instruction Parse Error" << std::endl;
            iter++;
            continue;
        }
        auto const& instruction = std::get<UnionInstruction>(instructionOrNull.element);
        std::cout << serializer.Serialize(instruction) << std::endl;
    }

    return 0;
}