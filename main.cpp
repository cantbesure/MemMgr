/****************************************************
   内存管理实验
   #mem_max 内存总大小
   #method 选取算法：
   ----0：first-fit
   ----1：next-fit
   ----2：best-fit
   ----3：worst-fit
   #sim_steps 模拟次数
*****************************************************/
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <vector>
#include <cstring>
#include <algorithm>
//c++ 11
#include <random>
#include <chrono>

#define mem_max 10
#define method 0
#define sim_steps 1
using namespace std;

//块结构体
struct BLOCK
{
    int firstADDR;
    int size;
    int lastADDR;
};

//生成指定块
BLOCK make_block(int ff,int ss)
{
    BLOCK b;
    b.firstADDR=ff;
    b.size=ss;
    b.lastADDR=ff+ss-1;
    return b;
}
//first-fit
bool cmp_first(BLOCK  a,BLOCK b)
{
    return a.firstADDR<b.firstADDR;
}
//best-fit
bool cmp_best(BLOCK  a,BLOCK b)
{
    return a.size<b.size;
}
//worst-fit
bool cmp_worst(BLOCK  a,BLOCK b)
{
    return a.size>b.size;
}

vector <BLOCK> emptyBlocks;
vector <BLOCK> occupiedBlocks;

int mem[mem_max];//模拟内存
int mem_index=0;//存next-fit
//请求内存
bool mem_request(int request_size)
{
    int index=-2;
    switch (method)//选择算法
    {
    case 0:
        sort(emptyBlocks.begin(),emptyBlocks.end(),cmp_first);
        break;
    case 1:
        index=mem_index;
        sort(emptyBlocks.begin(),emptyBlocks.end(),cmp_first);
        break;
    case 2:
        sort(emptyBlocks.begin(),emptyBlocks.end(),cmp_best);
        break;
    case 3:
        sort(emptyBlocks.begin(),emptyBlocks.end(),cmp_worst);
        break;
    }
    if (index==-2)
        for (int i=0; i<emptyBlocks.size(); i++)
        {
            if (emptyBlocks[i].size>=request_size)
            {
                if (emptyBlocks[i].size==request_size)//直接写块
                {
                    BLOCK b=emptyBlocks[i];
                    occupiedBlocks.push_back(b);
                    for (int j=b.firstADDR; j<=b.lastADDR; j++)
                        mem[j]=1;
                    emptyBlocks.erase(emptyBlocks.begin()+i);
                }
                else//切割写块
                {
                    BLOCK b=make_block(emptyBlocks[i].firstADDR,request_size);
                    occupiedBlocks.push_back(b);
                    for (int j=b.firstADDR; j<=b.lastADDR; j++)
                        mem[j]=1;
                    emptyBlocks[i].size-=request_size;
                    emptyBlocks[i].firstADDR+=request_size;

                }
                return true;
            }
        }
    else
    {
        if (index==-1)
            if (emptyBlocks.size()) index=0;
            else return false;
        for (int cnt=0; cnt<emptyBlocks.size(); cnt++)
        {
            int i=(cnt+index)%emptyBlocks.size();
            if (emptyBlocks[i].size>=request_size)
            {
                if (emptyBlocks[i].size==request_size)//直接写块
                {
                    BLOCK b=emptyBlocks[i];
                    occupiedBlocks.push_back(b);
                    for (int j=b.firstADDR; j<=b.lastADDR; j++)
                        mem[j]=1;
                    emptyBlocks.erase(emptyBlocks.begin()+i);
                    i--;
                }
                else//切割写块
                {
                    BLOCK b=make_block(emptyBlocks[i].firstADDR,request_size);
                    occupiedBlocks.push_back(b);
                    for (int j=b.firstADDR; j<=b.lastADDR; j++)
                        mem[j]=1;
                    emptyBlocks[i].size-=request_size;
                    emptyBlocks[i].firstADDR+=request_size;

                }
                if (emptyBlocks.size())
                    mem_index=(i+1)%emptyBlocks.size();
                else
                    mem_index=-1;
                return true;
            }
        }
    }
    return false;
}
//释放内存
void mem_release(int release_block)
{
    BLOCK b=occupiedBlocks[release_block];
    //printf("%d\t",b.size);
    if (b.firstADDR>0&&mem[b.firstADDR-1]!=1&&b.lastADDR<mem_max-1&&mem[b.lastADDR+1]!=1)
    {
        //左右都空
        BLOCK n=make_block(0,b.size);
        int fi=-1,li=-1;
        for (int i=0; i<emptyBlocks.size(); i++)
        {
            if (emptyBlocks[i].lastADDR==b.firstADDR-1)
            {
                n.firstADDR=emptyBlocks[i].firstADDR;
                n.size+=emptyBlocks[i].size;
                fi=i;
            }
            if (emptyBlocks[i].firstADDR==b.lastADDR+1)
            {
                n.lastADDR=emptyBlocks[i].lastADDR;
                n.size+=emptyBlocks[i].size;
                li=i;
            }
            if (li!=-1&&fi!=-1)
            {
                emptyBlocks[fi]=n;
                emptyBlocks.erase(emptyBlocks.begin()+li);
                break;
            }
        }
    }
    else if (b.firstADDR>0&&mem[b.firstADDR-1]!=1)
    {
        //左空
        for (int i=0; i<emptyBlocks.size(); i++)
        {
            if (emptyBlocks[i].lastADDR==b.firstADDR-1)
            {
                emptyBlocks[i].lastADDR=b.lastADDR;
                emptyBlocks[i].size+=b.size;
                break;
            }
        }

    }
    else if (b.lastADDR<mem_max-1&&mem[b.lastADDR+1]!=1)
    {
        for (int i=0; i<emptyBlocks.size(); i++)
        {
            if (emptyBlocks[i].firstADDR==b.lastADDR+1)
            {
                emptyBlocks[i].firstADDR=b.firstADDR;
                emptyBlocks[i].size+=b.size;
                break;
            }
        }
        //右空
    }
    else
    {
        //都不空
        emptyBlocks.push_back(b);

    }
    memset(mem+b.firstADDR,0,b.size*(sizeof (int)));
    occupiedBlocks.erase(occupiedBlocks.begin()+release_block);
}
//初始化
void mem_init()
{
    emptyBlocks.clear();
    occupiedBlocks.clear();
    memset(mem,0,sizeof mem);
    emptyBlocks.push_back(make_block(0,mem_max-1));

    //sort(emptyBlocks.begin(),emptyBlocks.end(),cmp_best);
}
int main()
{
    //freopen("normal.txt","w",stdout);
    //取时间种子
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    //正态分布
    normal_distribution<double> distribution(mem_max/2,2);

    mem_init();

    for (int sim_cnt=0; sim_cnt<sim_steps; sim_cnt++)
    {
        int request_size=0;
        do
        {
//            request_size=(int) distribution(generator);
            request_size=3;
            printf("%d\t",request_size);
        }
        while (mem_request(request_size));
        if (occupiedBlocks.size())
        {
            printf("count:%d\n",occupiedBlocks.size());
//            int release_block=rand()%occupiedBlocks.size();
            int release_block=2;
            mem_release(release_block);
            for (int i=0; i<mem_max; i++)
                printf("%d",mem[i]);
        }
        else
        {
            printf("error");
        }
//        for (int i=0; i<=mem_max; i++)
//            printf("%d",mem[i]);

    }
    return 0;
}
