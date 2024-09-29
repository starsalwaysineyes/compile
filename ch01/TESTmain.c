#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

extern int TESTscan();

char Scanin[300],Scanout[300];

char Filein[300], Resout[300];

FILE * fin, *fout;

void main() 
{

    char c;
    int p=0;

    char filename[20]="AAA.T";
    char target[20]="BBB.T";

    printf("是否自定义输入文件路径？(1/0)\n");
    int readfilename=0;
    scanf("%d",&readfilename);

    if(readfilename==0)
    {
        printf("默认文件路径为：%s\n",filename);
    }
    else
    {
        printf("请输入文件名：");
        scanf("%s",filename);
    }
    printf("是否自定义输出文件路径？(1/0)\n");

    int writetarget=0;
    scanf("%d",&writetarget);
    if(writetarget==0)
    {
        printf("默认文件路径为：%s\n",target);
    }
    else
    {
        printf("请输入文件名：");
        scanf("%s",target);
    }

    fin = fopen(filename, "r");
    fout = fopen(target, "w");

    while(c!=EOF) 
    {
        c=fgetc(fin);
        Filein[p++]=c;
    }
    Filein[p]='\0';

    //printf("123");
    int es = 0;
    es = TESTscan();
    if(es > 0) 
        printf("词法分析有误，编译停止！");
    else 
        printf("词法分析成功！\n");

    if(es==0)
        printf("%s",Resout);

    //fout = fopen("file.txt", "w");
    if (fout == NULL)
    {
        perror("error opening file");
        exit(0);
    }
    fputs(Resout, fout);
    fclose(fin);
    fclose(fout);

    
}