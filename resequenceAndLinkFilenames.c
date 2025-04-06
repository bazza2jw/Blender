#include <string.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int ac, char **av)
{
    if(ac == 3)
    {
        int i = 0;
        DIR * d = opendir(av[1]); // open the path
        if(d==NULL) return -1; // if was not able, return
        struct dirent * dir; // for the directory entries
        while ((dir = readdir(d)) != NULL) // if we were able to read somehting from the directory
        {
            if(dir-> d_type != DT_DIR) // if the type is not directory just print it with blue color
            {
                char f[2048];
                char s[2048];
                //
                char *ext = strrchr(dir->d_name,'.');
                //
                sprintf(f,"%s/%s",av[1], dir->d_name); // target for link
                sprintf(s,"%s/%06d%s",av[2], i++, ext?ext:"");
                if(symlink(f, s))
                {
                    perror("Failed linking");
                    return -1;
                }
            }
        }
        closedir(d); // finally close the directory
        return 0;
    }
}
