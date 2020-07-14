#ifndef __PREDICTOR__
#define __PREDICTOR__

#include"counter2.hpp"
#include<cstdio>

class Predictor
{
    private:
        static const int N=3;
        Counter2 cnt[6][1<<N];  //BEQ BNE BLT BGE BLTU BGEU 6types
        bool buf[6][N];
    public:
        int tot;
        Predictor():tot(0) {}
        void push(Instructiontypes type,bool cur)
        {
            int k=type-BEQ;
            for(int i=1;i<N;++i)
                buf[k][i-1]=buf[k][i];
            buf[k][N-1]=cur;
        }
        void update(Instructiontypes type,int dir)
        {
            int k=type-BEQ;
            unsigned num=0;
            for(int i=0;i<N;++i)
                num=num<<1|buf[k][i];
            cnt[k][num].update(dir);
        }
        bool willJump(Instructiontypes type)
        {
            ++tot;
            int k=type-BEQ;
            unsigned num=0;
            for(int i=0;i<N;++i)
                num=num<<1|buf[k][i];
            return cnt[k][num].willJump();
        }
};

#endif