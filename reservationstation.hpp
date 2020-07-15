#ifndef __RESERVATIONSTATION__
#define __RESERVATIONSTATION__

#include"RISC-V.h"
#include"register.hpp"
#include"instruction.hpp"
#include"ALunit.hpp"
#include"SLunit.hpp"

class ReservationStation
{
    friend class Executor;
    private:
        Register *reg;
        static const unsigned SLN=2;
        static const unsigned ALN=2;
        std::deque<Resnode> SLres,ALres;
    public:
        ReservationStation(Register *_reg):reg(_reg) {}
        bool full(Instructiontypes type)
        {   
            return isSL(type)?SLres.size()==SLN:ALres.size()==ALN;
        }
        void reset()
        {
            SLres.clear();
            ALres.clear();
        }
        void push(Instruction opt)
        {
            Resnode t;
            t.num=opt.num,t.pc=opt.pc;
            t.Op=opt.type,t.rd=opt.rd;
            //notice:not all inst has 2 rs
            if (opt.basictype!=U&&opt.basictype!=J)
            {
                std::pair<unsigned,Resnode*> data;
                data=reg->getdata(opt.rs1);
                t.Vj=data.first,t.Qj=data.second;
                if (opt.basictype!=I)
                {
                    data=reg->getdata(opt.rs2);
                    t.Vk=data.first,t.Qk=data.second;
                }
                else t.Vk=opt.rs2;  //shamt in I-type
            }
            t.A=opt.imm,t.isBusy=0;
            if (isSL(opt.type))
                SLres.push_back(t);
            else
                ALres.push_back(t);
            if (isSL(t.Op)!=2&&isJump(t.Op)!=1) //JAL also modify reg
                reg->setQi(t.rd,isSL(t.Op)?&SLres.back():&ALres.back());
        }
        void remove(Resnode *ptr)
        {
            std::deque<Resnode>::iterator it;
            if (isSL(ptr->Op))
            {
                it=SLres.begin();
                while (&(*it)!=ptr) ++it;
                SLres.erase(it);
            }
            else
            {
                it=ALres.begin();
                while (&(*it)!=ptr) ++it;
                ALres.erase(it);
            }
        }
        void update(Resnode *opt,unsigned val)   //update by opt
        {
            std::deque<Resnode>::iterator it;
            for(it=SLres.begin();it!=SLres.end();++it)
            {
                if (it->Qj==opt)
                    it->Qj=NULL,it->Vj=val;
                if (it->Qk==opt)
                    it->Qk=NULL,it->Vk=val;
            }
            for(it=ALres.begin();it!=ALres.end();++it)
            {
                if (it->Qj==opt)
                    it->Qj=NULL,it->Vj=val;
                if (it->Qk==opt)
                    it->Qk=NULL,it->Vk=val;
            }
        }
        void check(ALUnit *ALU,SLUnit *SLU)    //when V ready goto unit
        {
            std::deque<Resnode>::iterator it;
            for(it=SLres.begin();it!=SLres.end();++it)
                if (it->Qj==NULL&&it->Qk==NULL)
                    if (!it->isBusy&&!SLU->isLock()) 
                    {
                        SLU->init(&(*it));
                        SLU->putwclk(3);
                        it->isBusy=1;
                    }
            for(it=ALres.begin();it!=ALres.end();++it)
                if (it->Qj==NULL&&it->Qk==NULL)
                    if (!it->isBusy&&!ALU->isLock()) 
                    {
                        ALU->init(&(*it));
                        ALU->putwclk(1);
                        it->isBusy=1;
                    }
        }
};

#endif
