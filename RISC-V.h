#ifndef __RISC_V__
#define __RISC_V__

#include<utility>
#include<queue>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<cstring>
#include<iostream>
#include<iomanip>

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
struct Resnode  //node in res station
{
    unsigned rd;
    unsigned num;   //fetch timestamp
    unsigned pc;    //pc location
    Instructiontypes Op;
    Resnode *Qj,*Qk;
    unsigned Vj,Vk;  //operand value
    unsigned A;  //addr
    bool isBusy;    //indicate whether it's in unit, unlike what in CAAQA 
    Resnode()
    {
        Op=EMPTY;
        Qj=Qk=NULL;
        Vj=Vk=A=isBusy=0;
    }
    void reset()
    {
        rd=num=pc=0;
        Op=EMPTY;
        Qj=Qk=NULL;
        Vj=Vk=A=isBusy=0;
    }
};
// struct ROBnode  //node in ROB
// {
//     Resnode opt;
//     unsigned temp_result,temp_resultpc;
//     bool isReady;
//     ROBnode() 
//     {
//         temp_result=0;
//         temp_resultpc=0;
//         isReady=0;
//     }
// };
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
unsigned sext(unsigned x,int n) //sign-extend
{
    return (x>>n)&1?x|0xffffffff>>n<<n:x;
}     
unsigned setlow0(unsigned x) {return (x|1)^1;}
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