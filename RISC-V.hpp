#include"RISC-V.h"
#include"register.hpp"
#include"memory.hpp"
#include"predictor.hpp"
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
        Predictor prd;
        InstructionFetch IF;
        InstructionDecode ID;
        Execute EXE;
        MemoryAccess MEM;
        WriteBack WB;
        int mode;
        void run_parallel()
        {
            bool MEM2WB,isPB;     //whether MEM->WB 
            mem->init_read();
            IF.init(mem,&reg);
            do {
                ++clkcnt,isPB=0;
                WB.run();
                MEM.run();
                MEM2WB=MEM.gettype()!=EMPTY;
                WB.init(MEM);
                if (!MEM.isLock()&&MEM.gettype()!=EMPTY) MEM.forwarding(EXE);
                EXE.run();
                if (!EXE.check(wcnt))    //put pipeline to last clk
                {
                    reg.prevpc();
                    // ID.putback(IF);
                    EXE.putback(ID);
                    EXE.reset();
                    ID.setJump();
                    isPB=1;
                }
                EXE.update(&prd);
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
                if (!isPB) ID.run();
                if (ID.willJump()||isSL(ID.gettype()))
                {
                    IF.reset();     //1+1+1 without SL  jump
                    IF.putwclk(3);  //3+1 / 1 can forwarding    s&L
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
                IF.run(),++clkcnt;
                ID.init(IF);
                ID.run(),++clkcnt;
                EXE.init(ID);
                EXE.run();
                if (isSL(EXE.gettype())) clkcnt+=3;
                MEM.init(EXE);
                MEM.run(),++clkcnt;
                WB.init(MEM);
                WB.run(),++clkcnt;
            } while (!WB.isEnd());
        }
        //debug
        int clkcnt,wcnt;
    public:
        RISC_V(Memory *_mem,int _mode=0):clkcnt(0),wcnt(0),mem(_mem),mode(_mode),ID(&prd) {}
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
            return clkcnt;
        }
        void prdrate()
        {
            int rcnt=prd.tot-wcnt;
            printf("%d/%d %.2lf%\n",rcnt,prd.tot,rcnt*100.0/prd.tot);
        }
};
