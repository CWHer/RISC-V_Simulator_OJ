#ifndef __PREDICTOR__
#define __PREDICTOR__

#include"counter2.hpp"
#include<cstdio>

class Predictor
{
    private:
        Counter2 cnt;
    public:
        int tot;
        Predictor():tot(0) {}
        void update(int dir)
        {
            cnt.update(dir);
        }
        bool willJump()
        {
            ++tot;
            return cnt.willJump();
        }
};

#endif