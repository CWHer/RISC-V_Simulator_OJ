#include"register.hpp"
#include"memory.hpp"
#include"predictor.hpp"
#include"RISC-V.h"
#include"instructionfetch.hpp"
#include"instructiondecode.hpp"
#include"execute.hpp"
#include"memoryaccess.hpp"
#include"writeback.hpp"
#include<iostream>
#include<iomanip>
#include<cstdio>
Register reg;
Memory mem;
Predictor prd;
InstructionFetch IF;
InstructionDecode ID(&prd);
Execute EXE;
MemoryAccess MEM;
WriteBack WB;
void putback()     //reset pipeline to last clk
{
    reg.prevpc();
    // ID.putback(IF);
    EXE.putback(ID);
    EXE.reset();
    ID.setJump();
}
int main()
{
    // freopen("ans","w",stdout);
    // freopen("out","w",stdout);
    bool MEM2WB,isPB;     //whether MEM->WB 
    mem.init_read();
    IF.init(&mem,&reg);
    int cnt=0,wcnt=0;
    do {
        // std::cout<<++cnt<<' '<<str[ID.gettype()]<<std::endl;
        // std::cout<<reg.output()<<std::endl;
        // if (cnt==31855)
        // {
            // puts("1");
        // }
        // if (opt.empty()) break;
        ++cnt,isPB=0;
        WB.run();
        MEM.run();
        MEM2WB=MEM.gettype()!=EMPTY;
        WB.init(MEM);
        if (!MEM.isLock()&&MEM.gettype()!=EMPTY) MEM.forwarding(EXE);
        EXE.run();
        if (!EXE.check(wcnt)) putback(),isPB=1;
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
        // if (!EXE.gettype()==EMPTY&&!isSL(EXE.gettype())) //when pipeline is not stalled for MEM
        //     WB.init(MEM),MEM.reset();
        // else 
        //     MEM.putwclk(3);
        if (!isPB) ID.run();

        // if (ID.gettype()!=EMPTY)
        // {
        //     std::cout<<str[ID.gettype()]<<std::endl;
        //     reg.printdata();
        // }
        // std::cout<<str[ID.gettype()]<<std::endl;
        // reg.printdata();

        // if (cnt%6==2)
        // {
        //     IF.putback();
        //     IF.putwclk(5);
        // }
        // if (isJump(ID.gettype()))
        // if (ID.willJump())
        // {
        //     IF.reset();
        //     IF.putwclk(3);  //1+1+1 without SL
        //     // IF.putwclk(6);
        // }
        if (ID.willJump()||isSL(ID.gettype()))
        {
            IF.reset();
            IF.putwclk(3);  //3+1

            // if (ID.gettype()!=SB&&ID.gettype()!=SH&&ID.gettype()!=SW)
            //     IF.putwclk(3);
            // else  
            //     IF.putwclk(5);

            // IF.putwclk(6);
        }
        EXE.init(ID);
        IF.run();
        // std::cout<<reg.getpc()<<std::endl;
        ID.init(IF);
    } while (!WB.isEnd());
    std::cout<<prd.tot-wcnt<<'/'<<prd.tot<<' ';
    std::cout<<std::setprecision(2)<<(double)(prd.tot-wcnt)/prd.tot<<std::endl;
    std::cout<<cnt<<std::endl;
    std::cout<<reg.output()<<std::endl;
    return 0;
}