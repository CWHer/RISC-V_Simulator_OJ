#ifndef __ALunit__
#define __ALunit__

#include"RISC-V.h"
#include"executor.hpp"

class ALUnit
{
    friend class CommonDataBus;
    private:
        Executor exe;
        int wait_clk;
    public:
        ALUnit():wait_clk(0) {}
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
        }   
        bool empty()
        {
            return exe.gettype()==EMPTY;
        }
};

#endif