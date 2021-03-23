#include "Element.hh"
#include "ElementDefinition.hh"
#include "Parser.hh"
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

std::vector<std::tuple<TokenType, InstructionId, InstructionFormat>> instDefs = {
    { TokenType::INST_ADDIU, InstructionId::INST_ADDIU, InstructionFormat::I_FORMAT },
    { TokenType::INST_ADDU, InstructionId::INST_ADDU, InstructionFormat::R_FORMAT },
    { TokenType::INST_AND, InstructionId::INST_AND, InstructionFormat::R_FORMAT },
    { TokenType::INST_ANDI, InstructionId::INST_ANDI, InstructionFormat::I_FORMAT },
    { TokenType::INST_BEQ, InstructionId::INST_BEQ, InstructionFormat::I_FORMAT },
    { TokenType::INST_BNE, InstructionId::INST_BNE, InstructionFormat::I_FORMAT },
    { TokenType::INST_J, InstructionId::INST_J, InstructionFormat::J_FORMAT },
    { TokenType::INST_JAL, InstructionId::INST_JAL, InstructionFormat::J_FORMAT },
    { TokenType::INST_JR, InstructionId::INST_JR, InstructionFormat::R_FORMAT },
    { TokenType::INST_LUI, InstructionId::INST_LUI, InstructionFormat::I_FORMAT },
    { TokenType::INST_LW, InstructionId::INST_LW, InstructionFormat::I_FORMAT },
    { TokenType::INST_LB, InstructionId::INST_LB, InstructionFormat::I_FORMAT },
    { TokenType::INST_NOR, InstructionId::INST_NOR, InstructionFormat::R_FORMAT },
    { TokenType::INST_OR, InstructionId::INST_OR, InstructionFormat::R_FORMAT },
    { TokenType::INST_ORI, InstructionId::INST_ORI, InstructionFormat::I_FORMAT },
    { TokenType::INST_SLTIU, InstructionId::INST_SLTIU, InstructionFormat::I_FORMAT },
    { TokenType::INST_SLTU, InstructionId::INST_SLTU, InstructionFormat::R_FORMAT },
    { TokenType::INST_SLL, InstructionId::INST_SLL, InstructionFormat::R_FORMAT },
    { TokenType::INST_SRL, InstructionId::INST_SRL, InstructionFormat::R_FORMAT },
    { TokenType::INST_SW, InstructionId::INST_SW, InstructionFormat::I_FORMAT },
    { TokenType::INST_SB, InstructionId::INST_SB, InstructionFormat::R_FORMAT },
    { TokenType::INST_SUBU, InstructionId::INST_SUBU, InstructionFormat::R_FORMAT },
};

int main()
{
    std::string str = R"===(
       addu    $10,$9,  zero 
       or    $4,  $3,  $2       
       srl    $11,$6,  5         
)===";

    Tokenizer tokenizer = Tokenizer(str);

    for (auto& tokenDef : tokenDefs)
    {
        tokenizer.AddTokenDefinition(
            TokenDefinitionFactory::CreateTokenDefinition(tokenDef.second, tokenDef.first));
    }
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::RegisterTokenDefinition);
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::LabelTokenDefinition);
    tokenizer.AddTokenDefinition(TokenDefinitionFactory::ConstantTokenDefinition);

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
                std::get<InstructionId>(instDef), std::get<InstructionFormat>(instDef)));
    }

    for (auto iter = tokenizer.begin(); iter != iter.end();)
    {
        auto        instructionOrNull = parser.GetNextElement(ElementType::INSTRUCTION, iter);
        auto const& instruction       = std::get<UnionInstruction>(instructionOrNull.element);
        auto const& instructionR      = std::get<InstructionR>(instruction.instruction);
        std::cout << "id :" << (int)instruction.id << " | src1 :" << instructionR.registerSrc1
                  << " | src2 :" << instructionR.registerSrc2
                  << " | destOrValue :" << instructionR.destOrValue << std::endl;
    }

    return 0;
}