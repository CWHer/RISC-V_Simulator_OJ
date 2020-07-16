#include"RISC-V.h"
#include"register.hpp"
#include"memory.hpp"
#include"predictor.hpp"
#include"issue.hpp"
#include"reservationstation.hpp"
#include"ALunit.hpp"
#include"SLunit.hpp"
#include"commondatabus.hpp"
#include"reorderbuffer.hpp"

class RISC_V       
{
    private:
        int clkcnt;
        Register reg;
        Memory *mem;
        Predictor prd;
        Issue IS;
        ReservationStation res;
        ALUnit ALU;
        SLUnit SLU;
        CommonDataBus CDB;
        ReorderBuffer ROB;
        void refresh()
        {
            reg.resetQi();
            IS.reset();
            res.reset();
            ALU.reset();
            SLU.reset();
            CDB.reset();
            ROB.reset();
        }
        void run_OoOE()
        {
            bool isRE=0,isfull=0;
            mem->init_read();
            while (!IS.empty()||!ROB.empty())
            {
                ++clkcnt,isRE=isfull=0;
                //deal with full condition inside run
                while (!isfull&&!ROB.stall()&&!IS.empty()) 
                    isfull=IS.run(&res,&ROB);  //IF->Res
                res.check(&ALU,&SLU);   //Res->unit
                ALU.run(),SLU.run();
                if (!ALU.isLock()&&!ALU.empty()) //unit->CDB
                    CDB.push(ALU),ALU.reset();
                if (!SLU.isLock()&&!SLU.empty()) 
                    CDB.push(SLU),SLU.reset();
                while (!CDB.empty()&&CDB.isReady(&ROB)) CDB.run(&res,&ROB); //CDB->ROB
                while (!ROB.empty()&&ROB.isReady()&&!isRE) isRE=ROB.run(); //commit
                if (isRE) refresh();
            }
        }
    public:
        RISC_V(Memory *_mem)
            :clkcnt(0),mem(_mem),IS(_mem,&reg,&prd),
            res(&reg),SLU(_mem),CDB(&reg),ROB(_mem,&reg,&prd) {}
        void run()
        {
            run_OoOE();
        }
        unsigned output()
        {
            return reg.output();
        }
        int clktimes()
        {
            return clkcnt;
        }
        void prdrate()
        {
            int num=prd.tot-ROB.tot();
            printf("%d/%d %.2lf%\n",num,prd.tot,num*100.0/prd.tot);
        }
        void setStopNum(unsigned num)
        {
            ROB.setStopNum(num);
        }
};
