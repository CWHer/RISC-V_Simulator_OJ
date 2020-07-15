#ifndef __INSTRUCTIONFETCH__
#define __INSTRUCTIONFETCH__

#include"instruction.hpp"
#include"memory.hpp"
#include"register.hpp"

class InstructionFetch
{
    friend class InstructionDecode;
    friend class Execute;
    private:
        Instruction opt;
        Register *reg;
        Memory *mem;
        bool isend;
        int wait_clk;
        forward fwd;
    public:
        InstructionFetch():isend(0),wait_clk(0) {}
        void init(Memory *_mem,Register *_reg)
        {
            mem=_mem;
            reg=_reg;
        }
        void reset()    //reset to EMPTY
        {
            opt.reset();
            fwd.reset();
            wait_clk=0;
        }
        void run()
        {
            if (wait_clk>0)
            {
                --wait_clk;
                if (wait_clk) return;
            }
            isend=opt.fetch(mem,reg,fwd);
            fwd.reset();
            if (isend) reset();
        }
        void putwclk(int clk)  //put wait clk
        {
            wait_clk+=clk;
        }
        bool isEnd()
        {
            return isend;
        }
        Instructiontypes gettype()
        {
            return opt.gettype();
        }
        bool isLock()
        {
            return wait_clk>0;
        }
};

#endif