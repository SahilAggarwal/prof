#include <stdlib.h>
#include <stdio.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/stat.h>


#include "probe.h"
#include "shared.h"
#include "event_map.h"
#include "mmap_page.h"
#include "probe_buff.h"

#define SNAME "notify"

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

	sem_t *sem = sem_open(SNAME, O_CREAT, 0644, 0);
	
	pid_t pid = fork();
	if(pid == 0) {

		sem_t *sem = sem_open(SNAME, 0);
		sem_wait(sem);

		if(execv(exec,args) == -1)
			fprintf(stderr,"Failed to exec\n");
		exit(0);

	} else if(pid > 0) {
		struct probe *probe = probe_init(pid);
		if(probe == NULL) {
			fprintf(stderr,"Failed to initiate probe");
			return -1;
		}
		if(thread_start(probe->thread)) {
			fprintf(stderr,"Failed to start threads");
			return -1;
		}
		probe_enable(probe);
		sem_post(sem);
	
		pthread_join(probe->thread->tid,NULL);
	
		probe_buff__flush(probe->buff);
	}
	return 0;
}

static int probe_start_pid(pid_t pid)
{
	struct probe *probe = probe_init(pid);
	if(probe == NULL) {
		fprintf(stderr,"Failed to initiate probe");
		return -1;
	}
	if(thread_start(probe->thread)) {
		fprintf(stderr,"Failed to start threads");
		return -1;
	}

	probe_enable(probe);
	
	pthread_join(probe->thread->tid,NULL);
	
	probe_buff__flush(probe->buff);

	return 0;
}

static struct probe *probe_init(pid_t pid)
{
	page_size = sysconf(_SC_PAGE_SIZE);

	struct probe *probe = malloc(sizeof(*probe));

	probe->buff = probe_buff__init();
	if(probe->buff == NULL) {
		fprintf(stderr,"Failed to init probe buff\n");
		return NULL;
	}

	probe->thread = thread_init(probe->buff,pid);
	if(probe->thread == NULL) {
		fprintf(stderr,"Faield to map threads\n");
		return NULL;
	}

	return probe;
}

void probe_enable(struct probe *probe)
{
	int i = 0;
	struct event_map *event_map = probe->thread->event_map;
	for(; i < event_map->nr; i++)
	if(syscall(__NR_ioctl, event_map->events[i].mmap_pages->fd, PERF_EVENT_IOC_ENABLE, 0)) {
		fprintf(stderr,"Failed to enable sampling\n");
		continue;
	}
	printf("Probe Enabled\n");
}
