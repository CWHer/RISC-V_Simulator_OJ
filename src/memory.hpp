#include<cstdio>
#include<iostream>
#include<cstring>

#ifndef __MEMORY__
#define __MEMORY__

class Memory
{
    private:
        // static const int maxN=100;
        static const int maxN=1<<20;
        unsigned data[maxN+50],cnt;
        char seq[20];
        char get()
        {
            char ch=getchar();
            while ((~ch)&&ch!='@'&&!isdigit(ch)&&!isupper(ch)) ch=getchar();
            return ch;
        }
        unsigned seq2int(char *seq)   //base 16
        {
            int len=std::strlen(seq);
            unsigned ret=0;
            for(int i=0;i<len;++i)
                ret=ret*16+(unsigned)(seq[i]>='A'?seq[i]-'A'+10:seq[i]-'0');
            return ret;
        }
    public:
        Memory()
        {
            cnt=0;
            freopen("in","r",stdin);
            std::memset(data,0,sizeof(data));
            std::memset(seq,0,sizeof(seq));
        }
        Memory(const char *fname)
        {
            cnt=0;
            freopen(fname,"r",stdin);
            std::memset(data,0,sizeof(data));
            std::memset(seq,0,sizeof(seq));
        }
        ~Memory()
        {
            fclose(stdin);
        }
        void init_read()
        {
            char ch=getchar();
            while (~ch)
            {
                if (ch=='@')
                {
                    for(int i=0;i<8;++i) seq[i]=get();
                    cnt=seq2int(seq);
                }
                else
                {
                    seq[0]=ch,seq[1]=get(),seq[2]=0;
                    data[cnt++]=seq2int(seq);
                } 
                ch=get();
            }
        }
        void store(unsigned pos,unsigned val,int sz)
        {
            for(int i=0;i<sz;++i)
                data[pos+i]=val&255,val>>=8;
        }
        unsigned load(unsigned pos,int sz)
        {
            unsigned ret=0;
            for(int i=sz-1;~i;--i)
                ret=(ret<<8)+data[pos+i];
            return ret;
        }
};

#endif