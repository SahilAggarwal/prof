#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage()
{
        fprintf(stderr,"perf-prof [OPTION]\n\
                                \n\
                                -p <pid>                Trace pid and its children\n\
                                -e <exec>               Execute exec and trace it\n\
                                -s                      Trace only CPU events\n\
                                -f                      Trace only FileSystem event\n\
                                -m                      Trace only Memory events\n\
                                -b                      Trace only Block events\n\
                                -n                      Trace only Network events\n\
                                -a <arg>                Pass argument to the exec\n\
                                                                                    \n\
                                eg:  app-prof -ce test  Trace the CPU events of exec \n\
                                                        test                         \n\
                                     app-prof -fe test  Trace the File System events \n\
                                                        of exec test \n\
                                     app-prof -fbe /bin/ping -a google.com  Trace FS and\n\
                                                                        mem events\n"
                                );
}

int valid_opts(int opt_pid,int opt_exec)
{
        if(!opt_pid && !opt_exec)
                return 0;


        if(opt_exec && opt_pid) {
                fprintf(stderr,"Both exec and pid not allowed\n");
                return 0;
        }
        return 1;
}

char *join(char *str1,char *str2)
{
        char *str = (char *)malloc(strlen(str1) + strlen(str2) +1);
        str[0] = '\0';
        strcat(strcat(str,str1),str2);
        return str;
}

void panic(char *reason)
{
	fprintf(stderr,reason);
	fprintf(stderr,"\n");
	exit(EXIT_FAILURE);
}
