#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "shared.h"

int 		opt_fs;
int 		opt_cpu;
int		opt_mem;
int		opt_blk;
int 		opt_net;
int		opt_all;

static char 	*exec;
static char	*args[MAX_NR_ARGS];
static pid_t	pid;

int main(int argc, char **argv){
        if(argc < 2)
        {
                usage(argv[0]);
                return EXIT_FAILURE;
        }

        int opt;
        int opt_pname 	= 0;
        int opt_pid   	= 0;
        int opt_exec  	= 0;

	int no_args 	= 0;

        opt_fs        	= 0;
        opt_cpu       	= 0;
        opt_mem       	= 0;
        opt_blk       	= 0;
        opt_all       	= 1;

        pid    		= -1;
        exec    	= NULL;

	while((opt = getopt (argc, argv, "p:n:e:hfcmbti:la:"))
                != -1)
                switch(opt)
                {
                        case 'p': pid = atoi(optarg); opt_pid=1;
                                break;
                        case 'e': exec = optarg; opt_exec=1; args[no_args++] = optarg;
                                break;
                        case 'f': opt_fs = 1; opt_all = 0;
                                break;
                        case 'c': opt_cpu = 1; opt_all = 0;
                                break;
                        case 'm': opt_mem = 1; opt_all = 0;
                                break;
                        case 'b': opt_blk = 1; opt_all = 0;
                                break;
                        case 't': opt_net = 1; opt_all = 0;
                                break;
                        case 'a': args[no_args++] = optarg;
                                break;
                        case 'h':
                        case '?':
                                USAGE_ERROR;
                }
	args[no_args] = (char *)0;

	if( !valid_opts(opt_pid,opt_pname,opt_exec) )
	{
		USAGE_ERROR;
		return EXIT_FAILURE;
	}

	if(probe_start(exec,args,pid))
		panic("Could not start probe.");
	
}
