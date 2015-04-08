#include <stdlib.h>
#include <stdio.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>


#include "probe.h"
#include "event_list.h"
#include "mmap_page.h"
#include "shared.h"

__u64 	page_size;
extern int nr_events; // From src/event_list.c
extern struct wakeup wakeup; // From src/main.c


int probe_start(char *exec,char **args,pid_t pid)
{
	printf("Starting probe...\n");
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

static int probe_start_exec(char *exec, char **args)
{
	int ntfy_pipe[2];
	if(pipe2(ntfy_pipe, O_NONBLOCK)) {
		fprintf(stderr,"Failed to create notify pipe\n");
		return -1;
	}
	
	pid_t pid = fork();
	if(pid == 0) {

		close(ntfy_pipe[1]);
		char c[1] = {'0'};
		while(read(ntfy_pipe[0],c,1) != 1);

		if(*c == '1') {
			if(execv(exec,args) == -1)
				fprintf(stderr,"Failed to exec\n");
			exit(0);
		}

	} else if(pid > 0) {
		close(ntfy_pipe[0]);
		struct perf_probe_thread *p_thread = probe_initiate(pid);
		if(p_thread == NULL) {
			fprintf(stderr,"Failed to initiate probe");
			return -1;
		}
		write(ntfy_pipe[1],"1",1);
	
		int i;	
		for(i=0; i<nr_events; i++) {
			pthread_join(p_thread->e_list[i].thread,NULL);
		}

		probe_buff__flush(p_thread->e_list[0].probe_buff);
	}
	return 0;
}

static int probe_start_pid(pid_t pid)
{
	struct perf_probe_thread *p_thread = probe_initiate(pid);
	if(p_thread == NULL) {
		fprintf(stderr,"Failed to initiate probe");
		return -1;
	}

	int i;	
	for(i=0; i<nr_events; i++) {
                pthread_join(p_thread->e_list[i].thread,NULL);
        }

        probe_buff__flush(p_thread->e_list[0].probe_buff);
	return 0;
}


static struct perf_probe_thread  *probe_initiate(pid_t pid)
{
	page_size = sysconf(_SC_PAGE_SIZE);

	struct perf_probe_thread *p_thread = malloc(sizeof(*p_thread));

	struct event_list_node *e_list = event_list__init(pid,-1);
	if(e_list == NULL) {
		fprintf(stderr,"Failed to init event list\n");
		return NULL;
	}
	p_thread->e_list = e_list;

	if(event_list__open_events(p_thread->e_list)) {
		fprintf(stderr,"Failed to open events\n");
		return NULL;
	}

	int i;
        for(i=0; i<nr_events; i++) {
                if(syscall(__NR_ioctl, p_thread->e_list[i].mmap_pages->fd, \
                           PERF_EVENT_IOC_ENABLE, 0)) {
                        fprintf(stderr,"Failed to enable sampling\n");
                        continue;
                }
        }

	printf("Probe initiated.\n");
	return p_thread;
}

