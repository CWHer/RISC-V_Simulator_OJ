#ifndef __REGISTER__
#define __REGISTER__

#include"RISC-V.h"

class Register
{
    private:
        unsigned data[32];
        unsigned pc;
    public:
        Register()
        {
            pc=0;
            std::memset(data,0,sizeof(data));
        }
        unsigned getdata(unsigned pos)
        {
            return data[pos];
        }
        void setdata(unsigned pos,unsigned val)
        {
            if (pos==0) return;     //avoid modify reg[0]
            data[pos]=val;
        }
        unsigned &getpc() 
        {
            return pc;
        }
        void nextpc()
        {
            pc+=4;
        }
        void prevpc()
        {
            pc-=4;
        }
        unsigned output()  //end
        {
            return data[10]&255;
        }    
        //debug
        void printdata()
        {
            for(int i=0;i<32;++i)  
                printf("%u ",data[i]);
            putchar('\n');
        }
};
#endif