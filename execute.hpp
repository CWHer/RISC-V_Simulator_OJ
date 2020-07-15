#ifndef __EXECUTE__
#define __EXECUTE__

#include"instruction.hpp"
#include"memory.hpp"
#include"register.hpp"
#include"executor.hpp"
#include"predictor.hpp"
#include"instructionfetch.hpp"
#include"instructiondecode.hpp"

class Execute
{
    friend class MemoryAccess;
    private:
        Register *reg;
        Memory *mem;
        Executor exe;
        bool isend;
        int wait_clk;
        int cnt;
        forward fwd;
    public:
        Execute():isend(0),wait_clk(0),cnt(0) {}
        void init(InstructionDecode &ID)
        {
            if (isLock()) return;
            reset();
            if (ID.isLock()) return;
            reg=ID.reg;
            mem=ID.mem;
            exe.init(ID.opt);
            isend=ID.isend;
        }
        void forwarding(InstructionFetch &IF)
        {
            IF.fwd=exe.genfwd();
        }
        void putback(InstructionDecode &ID)
        {
            ID.reg=reg;
            ID.mem=mem;
            ID.opt=exe.opt;
        }
        void reset()    //reset to EMPTY
        {
            exe.reset();
            fwd.reset();
            wait_clk=0;
        }
        void run()
        {
            if (wait_clk>0) 
            {
                --wait_clk;
                if (wait_clk) return;
            }
            if (isend) return;
            exe.run(reg,fwd);
        }
        void putwclk(int clk)  //put wait clk
        {
            wait_clk+=clk;
        }
        bool isEnd()
        {
            return isend;
        }
        Instructiontypes gettype()
        {
            return exe.gettype();
        }
        bool isLock()
        {
            return wait_clk>0;
        }
        bool willJump()     //can only use after run!
        {
            return exe.temp_resultpc!=0;
        }
        bool check()
        {
            if ((!exe.opt.willJump())^(exe.temp_resultpc==0)) //inconsistent
            {
                ++cnt;
                if (exe.temp_resultpc!=0) return 0;
            }
            return 1;
        }
        void update(Predictor *prd)     //feedback predictor
        {
            Instructiontypes type=gettype();
            if (isJump(type)!=1) return;
            prd->update(exe.gettype(),willJump()?-1:1);
            prd->push(exe.gettype(),willJump());
        }
        int tot()
        {
            return cnt;
        }
};

#endif