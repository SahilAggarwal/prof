#include <stdlib.h>
#include <stdio.h>
#include <asm/unistd.h>
#include <unistd.h>


#include "probe.h"
#include "event_list.h"

__u64 	page_size;
extern char *events;

int probe_start(char *exec,char *args,pid_t pid)
{
	if(exec != NULL) {
		if(probe_start_exec(exec,args)) {
			fprintf(stderr,"Failed to start probe on exec\n");
			return -1;
		}
	}

	else if(pid != -1) {
		if(probe_start_pid(pid)) {
			fprintf(stderr,"Failed to start probe on pid\n");
                        return -1;
		}
	}
	return 0;
}

static int probe_start_exec(char *exec, char *args)
{
	return 0;
}

static int probe_start_pid(pid_t pid)
{
	if(probe_initiate(pid)) {
		fprintf(stderr,"Failed to initiate probe\n");
		return -1;
	}
}


static int probe_initiate(pid_t pid)
{
	page_size = sysconf(_SC_PAGE_SIZE);

	struct perf_probe_thread 	p_thread;

	struct event_list_node *e_list = event_list__init(-1,1);
	if(e_list == NULL) {
		fprintf(stderr,"Failed to init event list\n");
		return -1;
	}
	p_thread.e_list = e_list;

	if(event_list__open_events(p_thread.e_list)) {
		fprintf(stderr,"Failed to open events\n");
		return -1;
	}
	
	return 0;
}
