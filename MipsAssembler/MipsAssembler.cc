#include "CodeGenerator.hh"
#include "Element.hh"
#include "ElementDefinition.hh"
#include "NotImplementedException.hh"
#include "Parser.hh"
#include "ReadableSerializer.hh"
#include "Token.hh"
#include "Tokenizer.hh"
#include <fstream>
#include <iostream>
#include <sstream>
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
std::vector<std::pair<TokenType, ElementType>> elemDefs
    = { { TokenType::CONST_DECI, ElementType::TOKEN_CONST },
        { TokenType::CONST_HEX, ElementType::TOKEN_CONST },
        { TokenType::REGISTER, ElementType::TOKEN_REGISTER },
        { TokenType::LABEL, ElementType::TOKEN_LABEL },
        { TokenType::LABEL_ID, ElementType::TOKEN_LABEL_ID },
        { TokenType::BRAKET_LEFT, ElementType::TOKEN_BRAKET_LEFT },
        { TokenType::BRAKET_RIGHT, ElementType::TOKEN_BRAKET_RIGHT },
        { TokenType::COMMA, ElementType::TOKEN_COMMA },
        { TokenType::DIRE_DATA, ElementType::TOKEN_DIRE_DATA },
        { TokenType::DIRE_WORD, ElementType::TOKEN_DIRE_WORD },
        { TokenType::DIRE_TEXT, ElementType::TOKEN_DIRE_TEXT } };
std::vector<std::tuple<TokenType, InstructionId, InstructionType>> instDefs = {
    { TokenType::INST_ADDIU, InstructionId::INST_ADDIU, InstructionType::I_FORMAT },
    { TokenType::INST_ADDU, InstructionId::INST_ADDU, InstructionType::R_FORMAT },
    { TokenType::INST_AND, InstructionId::INST_AND, InstructionType::R_FORMAT },
    { TokenType::INST_ANDI, InstructionId::INST_ANDI, InstructionType::I_FORMAT },
    { TokenType::INST_BEQ, InstructionId::INST_BEQ, InstructionType::I_FORMAT_ADDRESS },
    { TokenType::INST_BNE, InstructionId::INST_BNE, InstructionType::I_FORMAT_ADDRESS },
    { TokenType::INST_J, InstructionId::INST_J, InstructionType::J_FORMAT },
    { TokenType::INST_JAL, InstructionId::INST_JAL, InstructionType::J_FORMAT },
    { TokenType::INST_JR, InstructionId::INST_JR, InstructionType::R_FORMAT_JR },
    { TokenType::INST_LUI, InstructionId::INST_LUI, InstructionType::I_FORMAT_LUI },
    { TokenType::INST_LW, InstructionId::INST_LW, InstructionType::I_FORMAT_OFFSET },
    { TokenType::INST_LA, InstructionId::INST_LA, InstructionType::I_FORMAT_LA },
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

std::string _HexToStr(unsigned int hex)
{
    std::ostringstream ss;
    ss << "0x" << std::hex << hex;
    return ss.str();
}

int main(int argc, char* argv[])
{
    std::string str = R"===(
      .data
var:  .word   5
        .text
main:
    la $8, var
    lw $9, 0($8)
    addu $2, $0, $9
    jal sum
    j exit

sum: sltiu $1, $2, 1
    bne $1, $0, sum_exit
    addu $3, $3, $2
    addiu $2, $2, -1
    j sum
sum_exit:
    addu $4, $3, $0
    jr $31
exit:
)===";

    if (argc == 2)
    {
        std::ifstream     file(argv[1]);
        std::stringstream ss;

        ss << file.rdbuf();
        str = ss.str();
    }

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
    // token element??? ??????
    for (auto& elemDef : elemDefs)
    {
        parser.AddElementDefinition(
            elemDef.second,
            ElementDefinitionFactory::CreateTokenElementDefinition(elemDef.first, elemDef.second));
    }
    for (auto& instDef : instDefs)
    {
        // token element ??? instruction??? ????????? element??? ??????
        parser.AddElementDefinition(ElementType::TOKEN_INST_ID,
                                    ElementDefinitionFactory::CreateTokenElementDefinition(
                                        std::get<TokenType>(instDef), ElementType::TOKEN_INST_ID));

        // instruction element ??????
        parser.AddElementDefinition(
            ElementType::INSTRUCTION,
            ElementDefinitionFactory::CreateInstructionDefinition(
                std::get<InstructionId>(instDef), std::get<InstructionType>(instDef)));
    }

    parser.AddElementDefinition(ElementType::OFFSET_ADDRESS,
                                ElementDefinitionFactory::CreateOffsetAddressDefinition());
    parser.AddElementDefinition(ElementType::ADDRESS,
                                ElementDefinitionFactory::CreateLabelAddressDefinition());
    parser.AddElementDefinition(ElementType::DATA,
                                ElementDefinitionFactory::CreateDataDefinition());

#pragma endregion

    std::vector<Data>             dataList;
    std::vector<UnionInstruction> instructions;
    auto                          iter = tokenizer.begin();

    // data ?????? parse
    auto direDataOrNull = parser.GetNextElement(ElementType::TOKEN_DIRE_DATA, iter);
    if (direDataOrNull.IsNull()) throw NotImplementedException();
    while (iter != iter.end())
    {
        auto direTextOrNull = parser.GetNextElement(ElementType::TOKEN_DIRE_TEXT, iter);
        if (!direTextOrNull.IsNull()) break;
        auto dataOrNull = parser.GetNextElement(ElementType::DATA, iter);
        if (dataOrNull.IsNull()) throw NotImplementedException();
        else
            dataList.push_back(std::get<Data>(dataOrNull.element));
    }
    // text ?????? parse
    while (iter != iter.end())
    {
        auto instructionOrNull = parser.GetNextElement(ElementType::INSTRUCTION, iter);
        if (instructionOrNull.IsNull()) { throw NotImplementedException(); }
        auto const& instruction = std::get<UnionInstruction>(instructionOrNull.element);
        instructions.push_back(instruction);
    }

    // ????????? ?????? ??? ?????? ????????? ?????? ?????? ??????
    ReadableSerializer serializer;
    for (auto& data : dataList) { std::cout << serializer.Serialize(data) << std::endl; }
    for (auto& instruction : instructions)
    { std::cout << serializer.Serialize(instruction) << std::endl; }

    // ?????? ??????
    std::unordered_map<std::string, unsigned int> dataLabelMap, textLabelMap;
    unsigned int                                  address = 0x10000000;
    for (auto const& data : dataList)
    {
        if (data.label.has_value())
            dataLabelMap.insert(std::make_pair(data.label.value(), address));
        address += 4;
    }
    address = 0x00400000;
    for (auto& inst : instructions)
    {
        inst.__selfAddress = address;

        if (inst.label.has_value())
            textLabelMap.insert(std::make_pair(inst.label.value(), address));
        if (inst.id != InstructionId::INST_EMPTY) address += 4;
        if (inst.id == InstructionId::INST_LA)
        {
            auto const& la      = std::get<InstructionILa>(inst.instruction);
            auto const& labelId = std::get<LabelAddress>(la.address.element).labelId;
            auto        iter    = dataLabelMap.find(labelId);
            if (iter == dataLabelMap.end()) throw NotImplementedException();

            if ((iter->second & 0xFFFF) != 0) { address += 4; }
        }
    }
    // ????????? ?????? ??????, ?????? Instruction ??????, Empty Instruction ??????
    std::vector<UnionInstruction> newInstructions;
    for (auto const& inst : instructions)
    {
        UnionInstruction newInst = UnionInstruction(inst);
        switch (inst.instType)
        {
        case InstructionType::I_FORMAT_ADDRESS:
        {
            auto instAddress = std::get<InstructionIAddress>(inst.instruction);
            auto labelId     = std::get<LabelAddress>(instAddress.address.element).labelId;
            auto iter        = dataLabelMap.find(labelId);
            if (iter == dataLabelMap.end())
            {
                iter = textLabelMap.find(labelId);
                if (iter == textLabelMap.end()) throw NotImplementedException();
            }
            ConstantAddress ca;
            ca.address          = iter->second;
            instAddress.address = UnionAddress(ca);
            newInst             = UnionInstruction(inst.id,
                                       inst.instType,
                                       InstructionIAddress(instAddress),
                                       inst.label,
                                       inst.__selfAddress);
            newInstructions.push_back(newInst);
            break;
        }
        case InstructionType::I_FORMAT_LA:
        {
            auto instLa  = std::get<InstructionILa>(inst.instruction);
            auto labelId = std::get<LabelAddress>(instLa.address.element).labelId;
            auto iter    = dataLabelMap.find(labelId);
            if (iter == dataLabelMap.end()) throw NotImplementedException();
            unsigned int    addressHi = (iter->second >> 16);
            InstructionILui instILui;
            instILui.rt        = instLa.rt;
            instILui.immediate = addressHi;
            newInstructions.push_back(UnionInstruction(InstructionId::INST_LUI,
                                                       InstructionType::I_FORMAT_LUI,
                                                       instILui,
                                                       inst.label,
                                                       inst.__selfAddress));
            if ((iter->second & 0xFFFF) != 0)
            {
                InstructionI instIOri;
                instIOri.rt = instIOri.rs = instLa.rt;
                instIOri.immediate        = iter->second & 0xFFFF;
                newInstructions.push_back(UnionInstruction(InstructionId::INST_ORI,
                                                           InstructionType::I_FORMAT,
                                                           instIOri,
                                                           std::nullopt,
                                                           inst.__selfAddress + 4));
            }
            break;
        }
        case InstructionType::J_FORMAT:
        {
            auto instJ   = std::get<InstructionJ>(inst.instruction);
            auto labelId = std::get<LabelAddress>(instJ.target.element).labelId;
            auto iter    = textLabelMap.find(labelId);
            if (iter == textLabelMap.end()) throw NotImplementedException();
            ConstantAddress ca;
            ca.address   = iter->second;
            instJ.target = UnionAddress(ca);
            newInstructions.push_back(
                UnionInstruction(inst.id, inst.instType, instJ, inst.label, inst.__selfAddress));
            break;
        }
        case InstructionType::EMPTY: break;
        default: newInstructions.push_back(newInst); break;
        }
    }

    std::cout << std::endl << "semantic analyzing completed." << std::endl << std::endl;

    // ????????? ?????? ??? ?????? ????????? ?????? ?????? ?????? ??????
    for (auto& data : dataList) { std::cout << serializer.Serialize(data) << std::endl; }
    for (auto& newInst : newInstructions)
    { std::cout << serializer.Serialize(newInst) << std::endl; }

    // ????????? ??????
    CodeGenerator codegen;

    std::cout << std::endl << "codegen completed." << std::endl << std::endl;

    std::cout << _HexToStr(newInstructions.size() * 4) << std::endl;
    std::cout << _HexToStr(dataList.size() * 4) << std::endl;
    for (auto& newInst : newInstructions) { std::cout << codegen.Serialize(newInst) << std::endl; }
    for (auto& data : dataList) { std::cout << codegen.Serialize(data) << std::endl; }

    if (argc == 2)
    {
        std::string argv1     = argv[1];
        std::string   filename = argv1.substr(0, argv1.size() - 1) + "o";
        std::ofstream os(filename);

        os << _HexToStr(newInstructions.size() * 4) << std::endl;
        os << _HexToStr(dataList.size() * 4) << std::endl;
        for (auto& newInst : newInstructions) { os << codegen.Serialize(newInst) << std::endl; }
        for (auto& data : dataList) { os << codegen.Serialize(data) << std::endl; }
    }

    return 0;
}