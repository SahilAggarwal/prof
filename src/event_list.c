#include <stdio.h>
#include <stdlib.h>

#include "event_list.h"
#include "shared.h"
#include "mmap_page.h"
#include "perf_event.h"

#define SYS_ENTER_OPEN  1<<1
#define SCHED_SWITCH    1<<0

char *events[] = {
                        "syscalls/sys_enter_open/",
			"sched/sched_switch/"
                 };

struct event_list_node *event_list__init(pid_t pid,int cpu)
{
	struct event_list_node 	*e_list;
	
	int i;
	int nr_events 	= ARR_SIZE(events);

	e_list = malloc(nr_events*sizeof(struct event_list_node));

	for(i=0; i<nr_events; i++) {

		int id = event_list__get_id(events[i]);
		if(id < 0) {
			fprintf(stderr,"Failed to get %s event id\n",events[i]);
			return NULL;
		}

		e_list[i].type  	= 1<<i;
		e_list[i].id    	= id;
		e_list[i].cpu  		= cpu;
		e_list[i].group_id	= -1;
		e_list[i].flags		= 0;
		e_list[i].pid  		= pid;
	}
	return e_list;
}

int event_list__open_events(struct event_list_node *e_list)
{
	int nr_events   = ARR_SIZE(events);
	int i;
	for(i=0; i<nr_events; i++) {
		e_list[i].mmap_pages = perf_event__open(&e_list[i]);
		if(e_list[i].mmap_pages == NULL) {
			fprintf(stderr,"Failed to set mmap_pages\n");
			return -1;
		}
	}
	return 0;
}

int event_list__get_id(char *event)
{
	char *event_path 	= join(DEBUGFS_EVENTS_PATH,event);	
	char *event_id_path 	= join(event_path,"id");
	
	int id = -1;
	FILE *fp = fopen(event_id_path,"r");
	if(fp == NULL) {
		fprintf(stderr,"Failed to open file %s\n",event_id_path);
		return -1;
	}
	fscanf(fp,"%d",&id);
	
	free(event_path);
	free(event_id_path);
	fclose(fp);

	return id;
}
