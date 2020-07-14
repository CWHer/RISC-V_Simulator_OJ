#ifndef __MEMORYACCESS__
#define __MEMORYACCESS__

#include"instruction.hpp"
#include"memory.hpp"
#include"register.hpp"
#include"executor.hpp"
#include"execute.hpp"

class MemoryAccess
{
    friend class WriteBack;
    private:
        Register *reg;
        Memory *mem;
        Executor exe;
        bool isend;
        int wait_clk;
        forward fwd;
    public:
        void init(Execute &EXE)
        {
            if (isLock()) return;
            reset();
            if (EXE.isLock()) return;
            reg=EXE.reg;
            mem=EXE.mem;
            exe=EXE.exe;
            isend=EXE.isend;
        }
        void reset()    //reset to EMPTY
        {
            Instruction opt;
            exe.init(opt);
            fwd.init();
        }
        void run()
        {
            if (isend) return;
            if (wait_clk>0)
            {
                --wait_clk;
                if (wait_clk) return;
            }
            exe.memory_access(mem,reg);
            if (gettype()!=SB&&gettype()!=SH&&gettype()!=SW) fwd=exe.genfwd();
        }
        void forwarding(Execute &EXE)
        {
            EXE.fwd=fwd;
            fwd.init();
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
};

#endif