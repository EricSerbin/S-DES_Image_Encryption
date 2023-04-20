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

string encryption(string a,string b,string k,int n);
string s1box[2][8]={{"101","010","001","110", "011","100","111","000"},{"001","100","110","010","000","111","101","011"}};
string s2box[2][8]={{"100","000","110","101","111","001","011","010"},{"101","011","000","111","110","010","001","100"}};


int main(int argc, char *argv[])
{
    int in_fd, out_fd, rd_count, wt_count;
    int iterations=0;

    string encryptString="";
    string left="";
    string right="";
    int leftOps, rightOps;
    char leftOpsChar, rightOpsChar;
    string key="010011001";

    leftOps=atoi(argv[3]);
    //stoi(rightOpsChar); stoi doesn't work with char pointers
    rightOps=atoi(argv[4]);

    char buffer[OFFSET];
    if(argc!=5) exit (1);
    in_fd=open(argv[1],O_RDONLY);
    if(in_fd<0) exit(2);
    out_fd=creat(argv[2],OUTPUT_MODE);
    if(out_fd < 0) exit (3);

    char headerBufff[OFFSET];
    char bufferSmall[realbuffsize];
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

        plaintext = bitset<8>(bufferSmall[0]).to_string() + bitset<8>(bufferSmall[1]).to_string() + bitset<8>(bufferSmall[2]).to_string();
        ciphertext=encryption((plaintext.substr(0,6)),plaintext.substr(6,6),key,leftOps);
        ciphertext2=encryption((plaintext.substr(12,6)),plaintext.substr(18,6),key,rightOps);
        ciphertext=ciphertext+ciphertext2;
        bufferSmall[0] = strtol(ciphertext.substr(0,8).c_str(), 0, 2);
        bufferSmall[1] = strtol(ciphertext.substr(8,8).c_str(), 0, 2);
        bufferSmall[2] = strtol(ciphertext.substr(16,8).c_str(), 0, 2);


        //this is where we modify the bits, in this case shuffling them
            
        if(rd_count<=0) break;
  
        wt_count=write(out_fd, bufferSmall, rd_count);
        if(wt_count<=0) exit(4);

    }
    cout<<"Total number of 24 bit lines read: "<<iterations<<"\n";
    close(in_fd);
    close(out_fd);
    if(rd_count==0)
    exit(0);
    else
    exit(5);
    
    
    return 0;
}

string encryption(string l0,string r0,string k,int n)
{   
    while(k.length()<n+8)
    {
        k+=k;
    } //lengthens key for ease of use
    int counter=0; 
    string defaultReturn="";
    while(counter<n)//ensures flexibility with rounds of S-DES
    {

    string keySt=(k.substr(counter,8));//shifts main key value to gain key dynamically
    
    string expan1, expan2; //110101
    expan1=r0.substr(0,2);
    expan2=r0.substr(4,2);
    string expanMid=r0.substr(2,2);
    
    swap(expanMid[0],expanMid[1]);
    //cout<<"expanMid is "<<expanMid; test
    string er0=expan1+expanMid+expanMid+expan2; //creating substring, expanding middle two digits twice

    string expanRes="";
    for(int i=0;i<8;i++)
    {
        if(keySt[i]==er0[i])
        {
            expanRes=expanRes+"0";
        }
        else{
            expanRes=expanRes+"1"; //xor key and expanded function
        }
    }
    //logical xor ended up being impractical, likely better computational
    int s0r=stoi((expanRes.substr(0,1)),nullptr,2); //stoi does not preserve leading zeros, meant usages was best for
    int s0c=stoi((expanRes.substr(1,3)),nullptr,2); //base 10 needs
    int s1r=stoi((expanRes.substr(4,1)),nullptr,2);
    int s1c=stoi((expanRes.substr(5,3)),nullptr,2);

    string s0=s1box[s0r][s0c];
    string s1=s2box[s1r][s1c];
    string sboxes=s1box[s0r][s0c]+s2box[s1r][s1c];
    expanRes="";
    for(int i=0;i<6;i++)    //logical xor of 6 length sbox answer and l0
    {
        if(sboxes[i]==l0[i])
        {
            expanRes=expanRes+"0";
        }
        else{
            expanRes=expanRes+"1";
        }
    }    
    if(counter==n-1){
        return r0+expanRes;
    }
    counter++;
    l0=(r0);    //switching feistel blocks
    r0=expanRes;
    defaultReturn=r0+expanRes;
    er0="";
    }
    return defaultReturn;
    
}
