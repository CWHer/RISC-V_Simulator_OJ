#ifndef __WRITEBACK__
#define __WRITEBACK__

#include"RISC-V.h"
#include"instruction.hpp"
#include"memory.hpp"
#include"register.hpp"
#include"executor.hpp"
#include"memoryaccess.hpp"

class WriteBack
{
    private:
        Register *reg;
        Memory *mem;
        Executor exe;
        bool isend;
    public:
        WriteBack():isend(0) {}
        void init(MemoryAccess &MEM)
        {
            reset();
            if (MEM.isLock()) return;
            reg=MEM.reg;
            mem=MEM.mem;
            exe=MEM.exe;
            isend=MEM.isend;
        }
        void reset()    //reset to EMPTY
        {
            exe.reset();
        }
        void run()
        {
            if (isend) return;
            exe.write_back(reg);
        }
        bool isEnd()
        {
            return isend;
        }
        Instructiontypes gettype()
        {
            return exe.gettype();
        }
};

#endif