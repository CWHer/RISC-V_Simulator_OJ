#ifndef __WRITEBACK__
#define __WRITEBACK__

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
            Instruction opt;
            exe.init(opt);
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