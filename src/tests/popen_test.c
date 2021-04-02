/*
 * This file is part of GOOD.
 * 
 * MIT License
 * 
 */
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "libutil/general.h"


int main(int argc, char **argv)
{
    
    if(argc >1)
   // if(1)
    {
        printf("aaaa=%s\n",getenv("aaaa"));
        printf("bbbb=%s\n",getenv("bbbb"));

        int i = -1;
        int o = -1;
        int p = good_popen("bash", NULL, &o, &i, NULL);

        char cmd[] = {"ls /tmp/ -l"};
        good_write(o, cmd, strlen(cmd));
        good_write(o, "\n", 1);

        while (1)
        {
            if(good_poll(i,1,5000)<=0)
                break;

            char buf[2] = {0};
            ssize_t chk = good_read(i, buf, 1);
            if (chk <= 0)
                break;

            printf("%s", buf);
        }

        printf("aa\n");

        fflush(stdout);

        kill(p,15);

        waitpid(p, NULL, 0);
    }
    else
    {
        char name[NAME_MAX]= {0};
        good_proc_pathfile(name);

        char cmd[PATH_MAX] = {0};
        sprintf(cmd,"%s asdadf",name);

        char* envp[] = {"aaaa=cccc","bbbb=dddd",NULL};
        int i = -1;
        int o = -1;
        int p = good_popen(cmd, envp, NULL,&i,NULL);

        while (1)
        {
            if(good_poll(i,1,10000)<=0)
                break;

            char buf[2] = {0};
            ssize_t chk = good_read(i, buf, 1);
            if (chk <= 0)
                break;

            printf("%s", buf);
        }

        printf("ccc\n");

     
        waitpid(p, NULL, 0);
    }

    return 0;
}