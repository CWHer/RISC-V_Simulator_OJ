#ifndef __REORDERBUFFER__
#define __REORDERBUFFER__

#include"RISC-V.h"
#include"memory.hpp"
#include"register.hpp"
#include"predictor.hpp"
#include"instruction.hpp"
#include"executor.hpp"

class ReorderBuffer
{
    friend class CommonDataBus;
    private:
        Memory *mem;
        Register *reg;
        Predictor *prd;
        unsigned cnt;//incorrect prediction cnt
        static const int N=10;
        std::deque<Executor> Q;
        void refresh(unsigned pc)
        {
            
        }
    public:
        ReorderBuffer(Memory *_mem,Register *_reg,Predictor *_prd)
            :mem(_mem),reg(_reg),prd(_prd),cnt(0) {}
        bool stall()    //JALR & S-type(SW)
        {
            std::deque<Executor>::iterator it;
            for(it=Q.begin();it!=Q.end();++it)
                if (it->gettype()==JALR||isSL(it->gettype())==2) return 1;
            return 0;
        }
        bool full()
        {
            return Q.size()==N;
        }
        bool empty()
        {
            return Q.empty();
        }
        bool isReady()
        {
            return Q.front().isReady;
        }
        void reset()
        {
            Q.clear();
        }
        void push(Instruction opt)//push to res&ROB simultaneously
        {
            Executor t;
            t.opt.num=opt.num;
            t.opt.Op=opt.type;
            Q.push_back(t);
        }
        void update(Executor exe)
        {
            std::deque<Executor>::iterator it;
            it=Q.begin();
            while (it->opt.num!=exe.opt.num) ++it;
            *it=exe;
        }
        bool run()
        {
            Executor exe=Q.front();
            if (!exe.isReady) return 0;

            //debug
            // if (exe.opt.pc+4==4464)
            // {
                // puts("1");
            // }
            // std::cout<<str[exe.gettype()]<<std::endl;
            // std::cout<<exe.opt.pc+4<<std::endl;
            //let it consist with the only ans from serial...

            Q.pop_front();
            exe.write_back(mem,reg);

            //debug
            // reg->printdata();
            // std::cout<<std::endl;

            if (isJump(exe.gettype()))
            {
                if (isJump(exe.gettype())==1)
                {
                    prd->update(exe.gettype(),exe.temp_resultpc!=0?-1:1);
                    prd->push(exe.gettype(),exe.temp_resultpc!=0);
                }
                if ((Q.empty()&&exe.temp_resultpc!=reg->getpc())
                    ||(!Q.empty()&&exe.temp_resultpc!=Q.front().opt.pc)) 
                {       //catastrophic condition
                    cnt++;
                    reg->getpc()=exe.temp_resultpc;
                    return 1;
                }
            }
            return 0;
        }
        unsigned tot()
        {
            return cnt;
        }
};

#endif