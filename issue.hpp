#ifndef __ISSUE__
#define __ISSUE__

#include"RISC-V.h"
#include"instruction.hpp"
#include"register.hpp"
#include"memory.hpp"
#include"reservationstation.hpp"
#include"reorderbuffer.hpp"
#include"predictor.hpp"

class Issue
{
    private:
        Memory *mem;
        Register *reg;
        Predictor *prd;
        bool isEmpty;
    public:
        Issue(Memory *_mem,Register *_reg,Predictor *_prd)
            :mem(_mem),reg(_reg),prd(_prd),isEmpty(0) {}
        void run(ReservationStation *res,ReorderBuffer *ROB) 
        {    //if res&ROB not full && no JALR & S-type in ROB
            Instruction opt;
            if (opt.fetch(mem,reg))
            {
                isEmpty=1;
                opt.reset();
                return;
            }
            opt.decode();
            if (ROB->full()||res->full(opt.type)) return;
            if (isJump(opt.type)&&opt.type!=JALR)
            {
                if (isJump(opt.type)==1)
                {
                    if (prd->willJump(opt.type))
                        reg->getpc()=opt.imm+opt.pc;
                    else
                        reg->nextpc();
                }
                else    //JAL
                    reg->getpc()=opt.imm+opt.pc;
            }
            else reg->nextpc();
            res->push(opt);
            ROB->push(opt);
            //debug
            // std::cout<<str[opt.type]<<std::endl;
            // if (opt.type==BNE)
            // {
            //     puts("1");
            // }
        }
        bool empty()
        {
            return isEmpty;
        }
};

#endif