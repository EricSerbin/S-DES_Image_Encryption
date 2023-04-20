#include <fcntl.h>
#include <unistd.h>
#include <bitset>
#include <iostream>
#include <string>
using namespace std;


int main(int argc, char *argv[]);
#define OFFSET 54
#define realbuffsize 3
#define OUTPUT_MODE 0700
#define TRUE 1

string decryption(string a,string b,string k,int n);

string s1box[2][8]={{"101","010","001","110", "011","100","111","000"},{"001","100","110","010","000","111","101","011"}};
string s2box[2][8]={{"100","000","110","101","111","001","011","010"},{"101","011","000","111","110","010","001","100"}};


int main(int argc, char *argv[])
{
    int in_fd, out_fd, rd_count, wt_count;
    int iterations=0;

    string encryptString="";
    string left="";
    string right="";
    string key="010011001";
    int leftOps, rightOps;

    
    //"010011001",4
    leftOps=atoi(argv[3]);
    //rightOpsChar=argv[5];
    //stoi(rightOpsChar); stoi doesn't work with char pointers
    rightOps=atoi(argv[4]);

    char buffer[OFFSET];
    if(argc!=5) exit (1);
    in_fd=open(argv[1],O_RDONLY);
    if(in_fd<0)exit(2);
    out_fd=creat(argv[2],OUTPUT_MODE);
    if(out_fd < 0) exit (3);

    char headerBufff[OFFSET];
    char bufferSmall[realbuffsize];
    //string stringBuffer="crying";
    char encodeBufffer[12];

    rd_count=read(in_fd,headerBufff,OFFSET);
    wt_count=write(out_fd,headerBufff,OFFSET);

    string plaintext=" ";
    string ciphertext=" ";
    string plaintext2=" ";
    string ciphertext2=" ";

    while(TRUE)
    {        
        rd_count=read(in_fd,bufferSmall,realbuffsize);
        iterations++;
        //string bufferBeforeFunc="";
        //string bufferAfterFunc="";

        ciphertext = bitset<8>(bufferSmall[0]).to_string() + bitset<8>(bufferSmall[1]).to_string() + bitset<8>(bufferSmall[2]).to_string();
        //ciphertext=encryption((plaintext.substr(0,6)),plaintext.substr(6,6),key,4);
        plaintext=decryption((ciphertext.substr(0,6)),ciphertext.substr(6,6),key,leftOps)+decryption((ciphertext.substr(12,6)),ciphertext.substr(18,6),key,rightOps);
        
        bufferSmall[0] = strtol(plaintext.substr(0,8).c_str(), 0, 2);
        bufferSmall[1] = strtol(plaintext.substr(8,8).c_str(), 0, 2);
        bufferSmall[2] = strtol(plaintext.substr(16,8).c_str(), 0, 2);

       
        //bufferSmall[0]=strtol("11111111",0,2);
        if(rd_count<=0) break;
  
        wt_count=write(out_fd, bufferSmall, rd_count);
        if(wt_count<=0) exit(4);

         //bufferBeforeFunc="";
         //bufferAfterFunc="";
    }
    close(in_fd);
    close(out_fd);
    if(rd_count==0)
    exit(0);
    else
    exit(5);

    cout<<"Total number of increments of 24 bits read: "<<iterations;
    return 0;
    //lseek(in_fd, 54, SEEK_CUR);
}

string decryption(string l2,string r2,string k,int n)
{
    while(k.length()<n+8)
    {
        k+=k;
    }
   //lengths key so user never exceeds allowed length
   
    int counter=n;
    string defaultReturn="";
    while(counter>0){

    int key=stoi(k.substr(counter-1,8)); //shifts main key value to gain key
    string keySt=(k.substr(counter-1,8));   
    //int er0i=0;
    string expan1, expan2;
    expan1=l2.substr(0,2);
    expan2=l2.substr(4,2);
    string expanMid=l2.substr(2,2);
    
    swap(expanMid[0],expanMid[1]);
    string el0=expan1+expanMid+expanMid+expan2;    
    //cout<<"expanded r1 (l2) is "<<el0<<"\n"; unit test
    string expanRes="";
    for(int i=0;i<8;i++)
    {
        if(keySt[i]==el0[i])
        {
            expanRes=expanRes+"0";
        }
        else{
            expanRes=expanRes+"1";
        }
    }
    //cout<<"Xored key is "<<expanRes<<"\n";
    int s0r=stoi((expanRes.substr(0,1)),nullptr,2);
    int s0c=stoi((expanRes.substr(1,3)),nullptr,2);
    int s1r=stoi((expanRes.substr(4,1)),nullptr,2);
    int s1c=stoi((expanRes.substr(5,3)),nullptr,2);
    string s0=s1box[s0r][s0c];
    string s1=s2box[s1r][s1c];
    string sboxes=s1box[s0r][s0c]+s2box[s1r][s1c];

    expanRes="";
    for(int i=0;i<6;i++)
    {
        if(sboxes[i]==r2[i])//xor of (expended r1 with k2) and r2
        {
            expanRes=expanRes+"0";
        }
        else{
            expanRes=expanRes+"1";
        }
    }    

    //cout<<"sboxes are "<<s0<<"and "<<s1;
    //cout<<"\nexpanRes is"<<expanRes<<"\n";
    //cout<<"\tRound "<<n-counter+1<<" Key for decryption: "<<keySt<<"\n";
    if(counter==1)
    {
        //cout<<n<<" Round(s) of decryption, plainText is "<<expanRes<<l2<<"\n\n";
        return expanRes+l2;
    }
    counter--;
    r2=l2;      //switching feistel blocks
    l2=expanRes; 
    
    //cout<<"and r1 is "<<r0;
    //cout<<"\nand l1 is "<<l0<<"\n";
    defaultReturn=expanRes+l2;
    el0="";
    expanRes="";
    
    }
    return defaultReturn;
}