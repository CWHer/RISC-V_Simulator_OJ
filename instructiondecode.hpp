#ifndef __INSTRUCTIONDECODE__
#define __INSTRUCTIONDECODE__

#include"instruction.hpp"
#include"memory.hpp"
#include"register.hpp"
#include"predictor.hpp"
#include"instructionfetch.hpp"

class InstructionDecode
{
    friend class Execute;
    private:
        Instruction opt;
        Register *reg;
        Memory *mem;
        Predictor *prd;
        bool isend;
        int wait_clk;
    public:
        InstructionDecode(Predictor *_prd):wait_clk(0),prd(_prd) {}
        void init(InstructionFetch &IF)
        {
            if (isLock()) return;
            reset();
            if (IF.isLock()) return;
            opt=IF.opt;
            reg=IF.reg;
            mem=IF.mem;
            isend=IF.isend;
        }
        void putback(InstructionFetch &IF)
        {
            IF.reg=reg;
            IF.mem=mem;
            IF.opt=opt;
        }
        void reset()    //reset to EMPTY
        {
            opt.init();
        }
        void run()
        {
            if (wait_clk>0)
            {
                --wait_clk;
                if (wait_clk) return;
            }
            if (isend) return;
            opt.decode(prd);
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
        bool willJump()
        {
            return opt.willJump();
        }
        void setJump()
        {
            opt.willJump()=1;
        }
};

#endif