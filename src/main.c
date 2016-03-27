/*
*
* COPYRIGHT: Copyright (c) 2015 Sahil Aggarwal.
*
*  This program is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to Free Software Foundation, 
*  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
*
*  (http://www.gnu.org/copyleft/gpl.html)
*/


#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>

#include "shared.h"

int 		opt_fs;
int 		opt_sched;
int		opt_mm;
int		opt_blk;
int 		opt_net;
int		opt_all;

static char 	*exec;
static char	*args[MAX_NR_ARGS];
static pid_t	pid;

struct wakeup wakeup = {
        .ctrlc = 0,
        .rd_fd = -1,
        .wr_fd = -1
};

void signal_handler() {
        char data[1] = {1};
        __sync_add_and_fetch(&wakeup.ctrlc,1);
        write(wakeup.wr_fd,data,1);
}

int main(int argc, char **argv){
        if(argc < 2)
        {
                usage(argv[0]);
                return EXIT_FAILURE;
        }

        int opt;
	int pipe_fd[2];
        int opt_pid   	= 0;
        int opt_exec  	= 0;

	int no_args 	= 0;

        opt_fs        	= 0;
        opt_sched      	= 0;
        opt_mm       	= 0;
        opt_blk       	= 0;
        opt_all       	= 1;

        pid    		= -1;
        exec    	= NULL;

	while((opt = getopt (argc, argv, "p:e:hfsmbni:la:"))
                != -1)
                switch(opt)
                {
                        case 'p': pid = atoi(optarg); opt_pid=1;
                                  break;
                        case 'e': exec = optarg; opt_exec=1; args[no_args++] = optarg;
                                  break;
                        case 'f': opt_fs = 1; opt_all = 0;
                                  break;
                        case 's': opt_sched = 1; opt_all = 0;
                                  break;
                        case 'm': opt_mm = 1; opt_all = 0;
                                  break;
                        case 'b': opt_blk = 1; opt_all = 0;
                                  break;
                        case 'n': opt_net = 1; opt_all = 0;
                                  break;
                        case 'a': args[no_args++] = optarg;
                                  break;
                        case 'h':
                        case '?':
                                USAGE_ERROR;
                }
	args[no_args] = (char *)0;

	if( !valid_opts(opt_pid,opt_exec) )
	{
		USAGE_ERROR;
		return EXIT_FAILURE;
	}

	if(pipe2(pipe_fd, O_NONBLOCK)) {
		fprintf(stderr,"Failed to create wakeup pipe\n");
		return EXIT_FAILURE;
	}
	wakeup.rd_fd = pipe_fd[0];
	wakeup.wr_fd = pipe_fd[1];
	signal(SIGINT, signal_handler);
        signal(SIGCHLD, signal_handler);

	if(probe_start(exec,args,pid))
		panic("Could not start probe.");
	
}
