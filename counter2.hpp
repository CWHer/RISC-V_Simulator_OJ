#ifndef __COUNTER2__
#define __COUNTER2__

#include"RISC-V.h"

class Counter2
{
    private:
        States state;
    public:
        Counter2():state(WT) {}
        // Counter2():state(SNT) {}
        void update(int dir)
        {
            if (state==SNT&&dir==1) return;
            if (state==ST&&dir==-1) return;
            state=(States)((int)state+dir); 
        }
        bool willJump()
        {
            switch (state)
            {
                case ST:
                case WT:return 1;
                case WNT:
                case SNT:return 0;
            }
        }
};

#endif