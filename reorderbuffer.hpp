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
        unsigned stopnum;//default:0
        static const int N=10;
        std::deque<Executor> Q;
    public:
        ReorderBuffer(Memory *_mem,Register *_reg,Predictor *_prd,unsigned _num=0)
            :mem(_mem),reg(_reg),prd(_prd),cnt(0),stopnum(_num) {}
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

            if (exe.opt.num==stopnum) 
            {
                std::cout<<"inst num: "<<exe.opt.num<<std::endl;
                std::cout<<"inst type: "<<str[exe.gettype()]<<std::endl;
                std::cout<<"inst pc: "<<exe.opt.pc<<std::endl;
                system("pause"); //why can't it stop???
                // getchar();
            }

            //debug
            // if (exe.opt.pc==4920)
            // {
            //     puts("1");
            // }
            // std::cout<<str[exe.gettype()]<<std::endl;
            // std::cout<<exe.opt.pc<<std::endl;
            // std::cout<<"inst num: "<<exe.opt.num<<std::endl;

            Q.pop_front();
            exe.write_back(mem,reg);

            //debug
            // reg->printdata();
            // std::cout<<std::endl;

            if (isJump(exe.gettype()))
            {
                if (isJump(exe.gettype())==1)
                {
                    prd->update(exe.gettype(),exe.temp_resultpc!=exe.opt.pc+4?-1:1);
                    prd->push(exe.gettype(),exe.temp_resultpc!=exe.opt.pc+4);
                }
                if ((Q.empty()&&exe.temp_resultpc!=reg->getpc())
                    ||(!Q.empty()&&exe.temp_resultpc!=Q.front().opt.pc)) 
                {       //catastrophic condition
                    cnt++;
                    reg->getpc()=exe.temp_resultpc;
                    Instruction::instcnt=exe.opt.num;   //reset instcnt
                    return 1;
                }
            }
            return 0;
        }
        unsigned tot()
        {
            return cnt;
        }
        void setStopNum(unsigned num)
        {
            stopnum=num;
        }
};

#endif