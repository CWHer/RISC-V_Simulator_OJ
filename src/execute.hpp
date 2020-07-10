#include"instruction.hpp"
#include"memory.hpp"
#include"register.hpp"
#include"executor.hpp"
#include"instructiondecode.hpp"

#ifndef __EXECUTE__
#define __EXECUTE__

class Execute
{
    friend class MemoryAccess;
    private:
        Register *reg;
        Memory *mem;
        Executor exe;
        bool isend;
        int wait_clk;
        forward fwd;
    public:
        Execute():wait_clk(0) {}
        void init(InstructionDecode &ID)
        {
            if (isLock()) return;
            reset();
            if (ID.isLock()) return;
            reg=ID.reg;
            mem=ID.mem;
            exe.init(ID.opt);
            isend=ID.isend;
        }
        void putback(InstructionDecode &ID)
        {
            ID.reg=reg;
            ID.mem=mem;
            ID.opt=exe.opt;
        }
        void reset()    //reset to EMPTY
        {
            Instruction opt;
            exe.init(opt);
        }
        void run()
        {
            if (wait_clk>0) 
            {
                --wait_clk;
                if (wait_clk) return;
            }
            if (isend) return;
            exe.run(reg,fwd);
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
            return exe.gettype();
        }
        bool isLock()
        {
            return wait_clk>0;
        }
        bool check()
        {
            if (!exe.opt.willJump()&&exe.temp_resultpc!=0) return 0;
            return 1;
        }
};

#endif