#include"RISC-V.hpp"
#include"memory.hpp"
#include<iostream>
Memory mem;
RISC_V unit(&mem,1);    //mode  0(default):serial   1:parallel
int main()
{
    unit.run();
    std::cout<<unit.clktimes()<<std::endl;
    std::cout<<unit.output()<<std::endl;
    // unit1.run();
    // std::cout<<unit1.clktimes()<<std::endl;
    // std::cout<<unit1.output()<<std::endl;
    return 0;
}