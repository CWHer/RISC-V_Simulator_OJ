#ifndef __SLUnit__
#define __SLUnit__

#include"RISC-V.h"
#include"executor.hpp"
#include"memory.hpp"

class SLUnit
{
    friend class CommonDataBus;
    private:
        Memory *mem;
        Executor exe;
        int wait_clk;
    public:
        SLUnit(Memory *_mem):mem(_mem),wait_clk(0) {}
        void init(Resnode *_opt)
        {
            exe.init(_opt);
            wait_clk=0;
        }
        void reset()
        {
            exe.reset();
            wait_clk=0;
        }
        void putwclk(int clk)
        {
            wait_clk+=clk;
        }
        bool isLock()
        {
            return wait_clk>0;
        }
        void run()
        {
            if (wait_clk>0)
            {
                --wait_clk;
                if (wait_clk>0) return;
            }
            exe.run();
            exe.memory_access(mem);
        }
        bool empty()
        {
            return exe.gettype()==EMPTY;
        }
};

#endif 