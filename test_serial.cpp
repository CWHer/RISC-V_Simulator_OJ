#include"register.hpp"
#include"memory.hpp"
#include"RISC-V.h"
#include"instructionfetch.hpp"
#include"instructiondecode.hpp"
#include"execute.hpp"
#include"memoryaccess.hpp"
#include"writeback.hpp"
#include<iostream>
Register reg;
Memory mem;
InstructionFetch IF;
InstructionDecode ID;
Execute EXE;
MemoryAccess MEM;
WriteBack WB;
int main()
{


    int cnt=0;
    // freopen("out","w",stdout);
    mem.init_read();
    IF.init(&mem,&reg);
    // freopen("in","r",stdin);
    while (!WB.isEnd())
    {
        // std::cout<<reg.output()<<std::endl;
        // if (cnt==31855)
        // {
            // puts("1");
        // }
        IF.run(),++cnt;
        ID.init(IF);
        ID.run(),++cnt;

        // std::cout<<++cnt<<' '<<str[ID.gettype()]<<std::endl;
        // std::cout<<reg.getpc()<<std::endl;
        
        EXE.init(ID);
        EXE.run();
        if (isSL(EXE.gettype())) cnt+=3;
        MEM.init(EXE);
        MEM.run(),++cnt;
        WB.init(MEM);
        WB.run(),++cnt;

        // reg.printdata();
    }
    std::cout<<cnt<<std::endl;
    std::cout<<reg.output()<<std::endl;
    return 0;
}