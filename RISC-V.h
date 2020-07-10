#ifndef __RISC_V__
#define __RISC_V__

enum States{ST,WT,WNT,SNT}; //S:strongly    W:weakly    N:not   T:take
enum Basictypes{R,I,S,B,U,J};
enum Instructiontypes
{
    LUI,AUIPC,JAL,JALR,
    BEQ,BNE,BLT,BGE,BLTU,BGEU,
    LB,LH,LW,LBU,LHU,SB,SH,SW,
    ADDI,SLTI,SLTIU,XORI,ORI,ANDI,
    SLLI,SRLI,SRAI,
    ADD,SUB,SLL,SLT,SLTU,XOR,SRL,SRA,OR,AND,
    EMPTY
};
struct forward
{
    Instructiontypes type;
    unsigned rd,temp_result;
    forward() 
    {
        type=EMPTY;
        rd=temp_result=0;
    }
    forward(Instructiontypes _type,unsigned _rd,unsigned _temp_result)
        :type(_type),rd(_rd),temp_result(_temp_result) {}
    void init()
    {
        type=EMPTY;
        rd=temp_result=0;
    }
};
const char *str[]=
{
    "LUI","AUIPC","JAL","JALR",
    "BEQ","BNE","BLT","BGE","BLTU","BGEU",
    "LB","LH","LW","LBU","LHU","SB","SH","SW",
    "ADDI","SLTI","SLTIU","XORI","ORI","ANDI",
    "SLLI","SRLI","SRAI",
    "ADD","SUB","SLL","SLT","SLTU","XOR","SRL","SRA","OR","AND",
    "EMPTY"
};
int isJump(Instructiontypes type)
{
    int ret=0;
    switch (type)
    {
        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:ret=1;break;
        case JAL:
        case JALR:ret=2;break;
    }
    return ret;
}
int isSL(Instructiontypes type)
{
    int ret=0;
    switch (type)
    {
        case LB:
        case LH:
        case LW:
        case LBU:
        case LHU:ret=1;break;
        case SB:
        case SH:
        case SW:ret=2;break;
    }
    return ret;
}
#endif