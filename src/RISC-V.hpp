#include"RISC-V.h"
#include"register.hpp"
#include"memory.hpp"
#include"instructionfetch.hpp"
#include"instructiondecode.hpp"
#include"execute.hpp"
#include"memoryaccess.hpp"
#include"writeback.hpp"

class RISC_V        //mode  0(default):serial   1:parallel
{
    private:
        Register reg;
        Memory *mem;
        InstructionFetch IF;
        InstructionDecode ID;
        Execute EXE;
        MemoryAccess MEM;
        WriteBack WB;
        int mode;
        void putback()     //reset pipeline to last clk
        {
            reg.prevpc();
            ID.putback(IF);
            EXE.putback(ID);
            EXE.reset();
            ID.setJump();
        }
        void run_parallel()
        {
            bool MEM2WB;     //whether MEM->WB 
            mem->init_read();
            IF.init(mem,&reg);
            do {
                ++cnt;
                WB.run();
                MEM.run();
                MEM2WB=MEM.gettype()!=EMPTY;
                WB.init(MEM);
                if (!MEM.isLock()&&MEM.gettype()!=EMPTY) MEM.forwarding(EXE);
                EXE.run();
                if (!EXE.check()) putback();
                MEM.init(EXE);
                if (EXE.gettype()!=EMPTY||EXE.isEnd())  
                {
                    if (!isSL(EXE.gettype()))   //skip MEM if without SL
                    {
                        if (MEM2WB)     //WAW
                        {
                            EXE.putwclk(1);
                            ID.putwclk(2);
                            IF.putwclk(2);
                        }     
                        else WB.init(MEM);  
                        MEM.reset();    
                    }
                    else MEM.putwclk(3);
                }
                ID.run();
                if (ID.willJump())
                {
                    IF.reset();
                    IF.putwclk(3);  //1+1+1 without SL
                }
                if (isSL(ID.gettype()))
                {
                    IF.reset();
                    IF.putwclk(3);  //3+1 / 1 can forwarding
                }
                EXE.init(ID);
                IF.run();
                ID.init(IF);
            } while (!WB.isEnd());
        }
        void run_serial()
        {
            mem->init_read();
            IF.init(mem,&reg);
            do {
                IF.run(),++cnt;
                ID.init(IF);
                ID.run(),++cnt;
                EXE.init(ID);
                EXE.run();
                if (isSL(EXE.gettype())) cnt+=3;
                MEM.init(EXE);
                MEM.run(),++cnt;
                WB.init(MEM);
                WB.run(),++cnt;
            } while (!WB.isEnd());
        }
        //debug
        int cnt;
    public:
        RISC_V(Memory *_mem,int _mode=0):cnt(0),mem(_mem),mode(_mode) {}
        void run()
        {
            if (mode==0)
                run_serial();
            else
                run_parallel();
        }
        unsigned output()
        {
            return reg.output();
        }
        //debug
        int clktimes()
        {
            return cnt;
        }
};
