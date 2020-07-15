#ifndef __COMMONDATABUS__
#define __COMMONDATABUS__

#include"RISC-V.h"
#include"executor.hpp"
#include"register.hpp"
#include"reservationstation.hpp"
#include"ALunit.hpp"
#include"SLunit.hpp"
#include"reorderbuffer.hpp"

class CommonDataBus
{
    private:
        Register *reg;
        struct cmp
        {
            bool operator() (const Executor &a,const Executor &b)
            {
                return a.opt.num>b.opt.num;    
            } 
        };
        std::priority_queue<Executor,std::vector<Executor>,cmp> Q;
    public:
        CommonDataBus(Register *_reg):reg(_reg) {}
        bool isReady(ReorderBuffer *ROB)
        {
            if (ROB->empty()) return 1;
            std::deque<Executor>::iterator it;
            for(it=ROB->Q.begin();it->opt.num!=Q.top().opt.num;++it)
                if (!it->isReady) return 0;
            return 1;
        }
        void push(SLUnit SLU)
        {
            Q.push(SLU.exe);
        }
        void push(ALUnit ALU)
        {
            Q.push(ALU.exe);
        }
        void reset()
        {
            while (!Q.empty()) Q.pop();
        }
        void run(ReservationStation *res,ReorderBuffer *ROB)
        {
            Executor exe=Q.top();
            exe.isReady=1;
            Q.pop();
            res->update(exe.ptr,exe.temp_result);   //upd res
            if (reg->getQi(exe.opt.rd)==exe.ptr) //upd reg
                reg->setQi(exe.opt.rd,NULL);
            res->remove(exe.ptr);   //remove ptr in res
            ROB->update(exe);
        }
        bool empty()
        {
            return Q.empty();
        }
        //debug
        unsigned size()
        {
            return Q.size();
        }
};

#endif