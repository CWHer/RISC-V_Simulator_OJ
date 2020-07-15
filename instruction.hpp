#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include"RISC-V.h"
#include"memory.hpp"
#include"register.hpp"
#include"predictor.hpp"

class Instruction
{
    friend class Executor;
    private:
        unsigned seq;
        unsigned rs1,rs2,rd;
        unsigned imm;
        bool willjump;
        Basictypes basictype;
        Instructiontypes type;
    public:
        Instruction()
        {
            imm=seq=0;
            rs1=rs2=rd=0;
            type=EMPTY;
            basictype=R;
            willjump=0;
        }
        void reset()
        {
            imm=seq=0;
            rs1=rs2=rd=0;
            type=EMPTY;
            basictype=R;
            willjump=0;
        }
        bool fetch(Memory *mem,Register *reg,forward fwd)
        {
            reset();
            unsigned pc=reg->getpc()+fwd.temp_resultpc;
            seq=mem->load(pc,4);
            reg->nextpc();
            return seq==0x0ff00513; 
        }
        void decode(Predictor *prd)
        {
            unsigned opcode=seq&127;
            unsigned func3=seq>>12&7;
            unsigned func7=seq>>25&127;
            switch (opcode)
            {
                case 55:basictype=U,type=LUI;break;
                case 23:basictype=U,type=AUIPC;break;
                case 111:basictype=J,type=JAL,willjump=1;break;
                case 103:basictype=I,type=JALR,willjump=1;break;
                case 99:
                {
                    switch (func3) 
                    {
                        case 0:type=BEQ;break;
                        case 1:type=BNE;break;
                        case 4:type=BLT;break;
                        case 5:type=BGE;break;
                        case 6:type=BLTU;break;
                        case 7:type=BGEU;break;
                    }
                    willjump=prd->willJump(type);
                    basictype=B;
                    break;
                }
                case 3:	
                {
                    switch (func3) 
                    {
                        case 0:type=LB;break;
                        case 1:type=LH;break;
                        case 2:type=LW;break;
                        case 4:type=LBU;break;
                        case 5:type=LHU;break;
                    }
                    basictype=I;
                    break;
                }
                case 35:
                {
                    switch (func3) 
                    {
                        case 0:type=SB;break;
                        case 1:type=SH;break;
                        case 2:type=SW;break;
                    }
                    basictype=S;
                    break;
                }
                case 19:
                {
                    switch (func3) 
                    {
                        case 0:type=ADDI;break;
                        case 1:type=SLLI;break;
                        case 2:type=SLTI;break;
                        case 3:type=SLTIU;break;
                        case 4:type=XORI;break;
                        case 5:type=func7?SRAI:SRLI;break;
                        case 6:type=ORI;break;
                        case 7:type=ANDI;break;
                    }
                    basictype=I;
                    break;
                }
                case 51:
                {
                    switch(func3) 
                    {
                        case 0:type=func7?SUB:ADD;break;
                        case 1:type=SLL;break;
                        case 2:type=SLT;break;
                        case 3:type=SLTU;break;
                        case 4:type=XOR;break;
                        case 5:type=func7?SRA:SRL;
                        case 6:type=OR;break;
                        case 7:type=AND;break;
                    }
                    basictype=R;
                    break;
                }
            }
            rs1=seq>>15&31;
            rs2=seq>>20&31;
            rd=seq>>7&31;
            switch (basictype)  //calc imm
            {
                case I:imm=sext(seq>>20,11);break;
                case S:imm=sext((seq>>7&31)+(seq>>25<<5),11);break;
                case B:imm=sext((seq>>8<<1&31)+(seq>>25<<5&2047)+((seq>>7&1)<<11)+(seq>>31<<12),12);break;
                case U:imm=seq>>12<<12;break;
                case J:imm=sext((seq>>21<<1&2047)+((seq>>20&1)<<11)+((seq>>12&255)<<12)+(seq>>31<<20),20);break;
            }
        }
        // debug
        Instructiontypes gettype()
        {
            return type;
        }
        bool &willJump()
        {
            return willjump;
        }
};
#endif