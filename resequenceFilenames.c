#include <stdio.h>
#include <string.h>
int main(int ac , char ** av)
{
    int i;
    for(i = 1; i < ac; i++)
    {
        char c[1500];
        char f[1024];
        char *ext;
        char *dir;
        char *p = av[i];
        strcpy(f,p);
        ext = strrchr(f,'.');
        dir = strrchr(f,'/');
        if(dir)
        {
            *dir = 0;
        }
        if(ext)
        {
            *ext = 0;
            ext++;
        }
        //
        if(dir)
        {
            sprintf(c,"%s/%06d%c%s",f,i,'.',(ext)?ext:"");
        }
        else
        {
            sprintf(c,"%06d%c%s",i,'.',(ext)?ext:"");
        }
        rename(av[i], c);
        //
    }

    return 0;
}
