#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

//���涨�屣���ֱ�Ϊ�򻯳���ʵ���ַ�ָ�����鱣�����б�����
//��������ӱ����֣��ɼ�����ӣ����޸ı�������Ŀ keywordSum
#define keywordSum 10
char * keyword[keywordSum]={"if","else", "for", "while", "do", "int","read", "write"};
//���涨�崿���ֽ��
char singleword[50] = "+-*(){};,:";
//���涨��˫�ֽ�������ַ�
char doubleword[10]= "><=!";
extern char Filein[300], Resout[300];
//���ڽ�����������ļ������� test_main.c �ж���

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

        if(c==' ' || c==10 || c=='\t' || c=='\n')//��
        {
            dex++;
            continue;
        }
        else if(isalpha(c))//��ĸ
        {
            
            p=0;
            word[p++]=c;
            
            //������һ���ַ�����һ���ַ�����ĸ������
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
        else if(isdigit(c))//����
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
        else if(strchr(singleword,c)!=NULL)//�Ǵ����ֽ��
        {
            //printf("%c %c\n",c,c);
            sprintf(tmp,"%c %c\n",c,c);
            strcat(Resout,tmp);
            dex++;
        }
        else if(strchr(doubleword,c)!=NULL)//��˫�ֽ��
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
                //ע��
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