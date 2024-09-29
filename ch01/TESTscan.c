#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

//下面定义保留字表，为简化程序，实用字符指针数组保存所有保留字
//如果想增加保留字，可继续添加，并修改保留字数目 keywordSum
#define keywordSum 10
char * keyword[keywordSum]={"if","else", "for", "while", "do", "int","read", "write"};
//下面定义纯单分界符
char singleword[50] = "+-*(){};,:";
//下面定义双分界符的首字符
char doubleword[10]= "><=!";
extern char Filein[300], Resout[300];
//用于接收输入输出文件名，在 test_main.c 中定义

char word[50];
char tmp[50];
int p=0;

bool isKeyword(char * str)
{
    int i;
    for(i=0;i<keywordSum;i++)
    {
        if(keyword[i]==NULL)
            continue;
        //printf("fk %s\n",keyword[i]);
        if(strcmp(str,keyword[i])==0)
            return true;
    }
    return false;
}




int TESTscan()
{
    
    printf("work in scan\n");
    //return 0;
    int dex=0;
    int cnt=0;
    while(dex<300 && cnt++<300)
    {
        char c=Filein[dex];
        //printf("fk %c\n",c);
        if(c=='\0' || c==-1)
        {
            return 0;
        }

        if(c==' ' || c==10 || c=='\t' || c=='\n')//空
        {
            dex++;
            continue;
        }
        else if(isalpha(c))//字母
        {
            
            p=0;
            word[p++]=c;
            
            //存在下一个字符，下一个字符是字母或数字
            while(dex+1<300 && (isalpha(Filein[++dex]) || isdigit(Filein[dex])))
            {
                word[p++]=Filein[dex];
            }

            word[p]='\0';


            //printf("%s\n",word);

            if(isKeyword(word))
            {
                //printf("%s %s\n",word,word);
                sprintf(tmp,"%s %s\n",word,word);
                strcat(Resout,tmp);
            }
            else
            {
                //printf("ID %s\n",word);
                sprintf(tmp,"ID %s\n",word);
                strcat(Resout,tmp);
            }


        }//
        else if(isdigit(c))//数字
        {
            p=0;
            word[p++]=c;
            while(dex+1<300 && isdigit(Filein[++dex]))
            {
                word[p++]=Filein[dex];
            }
            word[p]='\0';
            //printf("NUM %s\n",word);
            sprintf(tmp,"NUM %s\n",word);
            strcat(Resout,tmp);
        }
        else if(strchr(singleword,c)!=NULL)//是纯单分界符
        {
            //printf("%c %c\n",c,c);
            sprintf(tmp,"%c %c\n",c,c);
            strcat(Resout,tmp);
            dex++;
        }
        else if(strchr(doubleword,c)!=NULL)//是双分界符
        {
            dex++;
            char next=Filein[dex];
            if(next=='=')
            {
                //printf("%c%c %c%c\n",c,next,c,next);
                sprintf(tmp,"%c%c %c%c\n",c,next,c,next);
                strcat(Resout,tmp);
            }
            else
            {
                //printf("%c %c\n",c,c);
                sprintf(tmp,"%c %c\n",c,c);
                strcat(Resout,tmp);
            }

        }
        else if(c=='/')
        {
            dex++;
            char next=Filein[dex];
            if(next=='*')
            {
                //注释
                while(dex+1<300 && Filein[++dex]!='*')
                {
                    //pass
                }
                if(dex+1<300 && Filein[++dex]=='/')
                {
                    continue;
                }
            }
        }
        else
        {
            printf("error when scan char: %c with ascii: %d",c,c);
            return 3;//error
        }

    }

}