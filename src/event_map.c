#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "event_map.h"
#include "perf_event.h"
#include "mmap_page.h"
#include "shared.h"

char *events[] = {
//			"sched/sched_wakeup/",
			"sched/sched_switch/",
//			"kprobes/clone/",
//			"raw_syscalls/sys_enter/",
//			"syscalls/sys_enter_open/",
//			"syscalls/sys_enter_read/",
//			"syscalls/sys_exit_read/",
//			"syscalls/sys_enter_write/",
//			"syscalls/sys_enter_lseek/",
//			"syscalls/sys_enter_mmap/"
                 };

struct event_list_map *event_list_map__init() 
{
	int nr_events = ARR_SIZE(events);
	struct event_list *tmp;
	struct event_list_map *elist_map  = malloc(sizeof(struct event_list_map));
	elist_map->elist = malloc(sizeof(struct event_list));

	INIT_LIST_HEAD(&(elist_map->elist->list));

	int nr = 0;
	int i;
	for(i=0; i < nr_events; i++) {

                int id = event__get_id(events[i]);
                if(id < 0)  {
                        fprintf(stderr,"Failed to get %s id -- Skipping\n",events[i]);
		}
		else {
			tmp       = malloc(sizeof(struct event_list));
			tmp->id   = id;
			tmp->type = 1<<i;
			++nr;
			list_add_tail(&(tmp->list), &(elist_map->elist->list));
		}
	}
	elist_map->nr = nr;
	return elist_map;
}

struct event_map *event_map__init(struct event_list_map *elist_map,int cpu,pid_t pid)
{
	int nr_events = elist_map->nr;
	struct list_head  *pos;
	struct event_list *tmp;
	struct event_map *event_map = malloc(sizeof(int) +  \
				      (sizeof(struct event)*nr_events));
	
	event_map->nr = nr_events;
	
	int  i=0;
	list_for_each(pos, &(elist_map->elist->list)) {

		tmp = list_entry(pos, struct event_list, list);	
	
		event_map->events[i].type	 	 = tmp->type;
		event_map->events[i].e_open.id    	 = tmp->id;
		event_map->events[i].e_open.cpu  	 = cpu;
		event_map->events[i].e_open.group_id	 = -1;
		event_map->events[i].e_open.flags	 = 0;
		event_map->events[i].e_open.pid  	 = pid;

		event_map->events[i].mmap_pages = perf_event__open(
                                                  &event_map->events[i].e_open
                                                                  );
                if(event_map->events[i].mmap_pages == NULL) {
                        fprintf(stderr,"Failed to set mmap_pages\n");
                        return NULL;
                }
		i++;
	}
	return event_map;
}

int event__get_id(char *event)
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
