#ifndef __EXECUTOR__
#define __EXECUTOR__

#include"RISC-V.h"
#include"instruction.hpp"
#include"register.hpp"
#include"memory.hpp"

class Executor
{
    // friend class Execute;
    friend class CommonDataBus;
    friend class ReorderBuffer;
    private:
        Resnode opt;
        Resnode *ptr;   //point at its place in Res
        unsigned temp_result,temp_resultpc;
        bool isReady;
    public:
        Executor()
        {
            ptr=NULL;
            temp_result=0;
            temp_resultpc=0;
            isReady=0;
        }
        void init(Resnode *_opt)
        {
            opt=*_opt,ptr=_opt;
            temp_result=temp_resultpc=0;
            isReady=0;
        }
        void reset()
        {
            opt.reset();
            ptr=NULL;
            temp_result=temp_resultpc=0;
            isReady=0;
        }
        void run()   
        {                              
            unsigned shamt=opt.Vk,imm=opt.A;     
            temp_result=temp_resultpc=0;
            switch (opt.Op)
            {
                case LUI:temp_result=imm;break;
                case AUIPC:temp_result=opt.pc+imm;break;
                //control instructions begin
                //jump
                case JAL:   //J type
                {
                    temp_result=opt.pc+4;
                    temp_resultpc=opt.pc+imm;
                    break;
                }
                case JALR:  //I type
                {
                    temp_result=opt.pc+4;
                    temp_resultpc=setlow0(opt.Vj+imm);
                    break;
                }
                //branch    //B type
                //if not jump pc will also goto next
                case BEQ:temp_resultpc=opt.pc+4+(imm-4)*(opt.Vj==opt.Vk);break;
                case BNE:temp_resultpc=opt.pc+4+(imm-4)*(opt.Vj!=opt.Vk);break;
                case BLT:temp_resultpc=opt.pc+4+(imm-4)*((int)opt.Vj<(int)opt.Vk);break;
                case BGE:temp_resultpc=opt.pc+4+(imm-4)*((int)opt.Vj>=(int)opt.Vk);break;
                case BLTU:temp_resultpc=opt.pc+4+(imm-4)*(opt.Vj<opt.Vk);break;
                case BGEU:temp_resultpc=opt.pc+4+(imm-4)*(opt.Vj>=opt.Vk);break;
                //load&store instructions begin   //I type
                case LB: 
                case LH: 
                case LW: 
                case LBU: 
                case LHU:
                case SB:    //S type
                case SH:
                case SW:opt.A=opt.Vj+imm;break;
                //arithmetic and logic instructions begin
                //I type
                case ADDI:temp_result=opt.Vj+imm;break;
                case SLTI:temp_result=((int)opt.Vj<(int)imm);break;
                case SLTIU:temp_result=(opt.Vj<imm);break;
                case XORI:temp_result=opt.Vj^imm;break;
                case ORI:temp_result=opt.Vj|imm;break;
                case ANDI:temp_result=opt.Vj&imm;break;
                case SLLI:temp_result=opt.Vj<<shamt;break;    
                case SRLI:temp_result=opt.Vj>>shamt;break;
                case SRAI:temp_result=(opt.Vj>>shamt)|(opt.Vj>>31<<31);break;
                //the original sign bit is copied into the vacated upper bits   ???
                //B type
                case ADD:temp_result=opt.Vj+opt.Vk;break;
                case SUB:temp_result=opt.Vj-opt.Vk;break;
                case SLL:temp_result=opt.Vj<<opt.Vk;break;
                case SLT:temp_result=((int)opt.Vj<(int)opt.Vk);break;
                case SLTU:temp_result=(opt.Vj<opt.Vk);break;
                case XOR:temp_result=opt.Vj^opt.Vk;break;
                case SRL:temp_result=opt.Vj>>opt.Vk;break;
                case SRA:temp_result=(opt.Vj>>opt.Vk)|(opt.Vj>>31<<31);break;
                case OR:temp_result=opt.Vj|opt.Vk;break;
                case AND:temp_result=opt.Vj&opt.Vk;break;
            }
        }
        void memory_access(Memory *mem)
        {
            switch (opt.Op)
            {
                //load and store instructions begin   
                //I type
                case LB:temp_result=sext(mem->load(opt.A,1),7);break;
                case LH:temp_result=sext(mem->load(opt.A,2),15);break;
                case LW:temp_result=mem->load(opt.A,4);break;
                case LBU:temp_result=mem->load(opt.A,1);break;
                case LHU:temp_result=mem->load(opt.A,2);break;
                //S type    //move to write_back in OoOE 
                // case SB:mem->store(opt.A,opt.Vk,1);break;
                // case SH:mem->store(opt.A,opt.Vk,2);break;
                // case SW:mem->store(opt.A,opt.Vk,4);break;
            }
        } 
        void write_back(Memory *mem,Register *reg)
        {   //pc will be modified earlier in fetch
            switch (opt.Op)
            {
                //control instructions begin
                //jump
                case JAL:   //J type    //pc in JAL&B type were dealed in IS
                {
                    reg->setdata(opt.rd,temp_result);
                    // reg->getpc()=temp_resultpc;    //!!! += --> = in OoOE
                    break;
                }
                case JALR:  //I type
                {
                    reg->setdata(opt.rd,temp_result);
                    reg->getpc()=temp_resultpc;   //nextpc in simultaneous IF(forwarding)
                    break;
                }
                //store
                case SB:mem->store(opt.A,opt.Vk,1);break;
                case SH:mem->store(opt.A,opt.Vk,2);break;
                case SW:mem->store(opt.A,opt.Vk,4);break;
                //branch    //B type
                // case BEQ:;
                // case BNE:
                // case BLT:
                // case BGE:
                // case BLTU:
                // case BGEU:reg->getpc()=temp_resultpc;break;  //!!! += --> = in OoOE
                //load and store instructions begin   
                //I type
                case LB: 
                case LH: 
                case LW: 
                case LBU: 
                case LHU:
                //arithmetic and logic instructions begin
                //U type
                case LUI:
                case AUIPC:   
                //I type
                case ADDI:
                case SLTI:
                case SLTIU:
                case XORI:
                case ORI:
                case ANDI:
                case SLLI:
                case SRLI:
                case SRAI:
                //the original sign bit is copied into the vacated upper bits   ???
                //B type
                case ADD:
                case SUB:
                case SLL:
                case SLT:
                case SLTU:
                case XOR:
                case SRL:
                case SRA:
                case OR:
                case AND:reg->setdata(opt.rd,temp_result);break;
            }
        }
        Instructiontypes gettype()
        {
            return opt.Op;
        }
};
#endif

