#include"RISC-V.hpp"
#include"memory.hpp"
Memory mem;
RISC_V unit(&mem);    //mode  OoOE
int main()
{
    // unit.setStopNum(300);
    unit.run();
//    unit.prdrate();
//    std::cout<<unit.clktimes()<<std::endl;
    std::cout<<unit.output()<<std::endl;
    // unit1.run();
    // std::cout<<unit1.clktimes()<<std::endl;
    // std::cout<<unit1.output()<<std::endl;
    return 0;
}
