#include <stdio.h>
#include <stdlib.h>

#include "event_map.h"
#include "perf_event.h"
#include "mmap_page.h"
#include "shared.h"

char *ftrace_events[] = {
			"sched/sched_switch/"			,
			"task/task_newtask/"			,
			"raw_syscalls/sys_enter/"		,
			"syscalls/sys_enter_open/"		,
			"syscalls/sys_enter_read/"		,
			"syscalls/sys_exit_read/"		,
			"syscalls/sys_enter_write/"		,
			"syscalls/sys_enter_lseek/"		,

			"syscalls/sys_enter_mmap/"		, 
			"syscalls/sys_exit_brk/"		, 

			/* Heavy activity for these events may indicate 
			 * that a specific cache is justified, particularly 
			 * if kmalloc slab pages are getting significantly
			 * internal fragmented as a result of the allocation 
			 * pattern. By correlating kmalloc with kfree, it may 
                         * be possible to identify memory leaks and where
			 * the allocation sites were.
			*/
	
			"kmem/kmalloc/"				, 
		
			/* These events are similar in usage to the kmalloc-
			 * related events except that it is likely easier to 
                         * pin the event down to a specific cache. At the time
			 * of writing, no information is available on what slab 
			 * is being allocated from, but the call_site can usually
                         *  be used to extrapolate that information
			 */	
			"kmem/kmem_cache_alloc/"		,

			/* The return value of this event can be compared to check if 
			 * fault was major/minor. 
			 */	
			"kmem/mm_page_fault_handle/"		,

			/* mm_page_alloc is a simple indicator of page 
                         * allocator activity. Pages may be allocated 
                         * from the per-CPU alloca tor (high performance)
                         * or the buddy allocator
                         */
			"kmem/mm_page_alloc/"			,

			/* If pages are allocated directly from the buddy allocator,
			 * the mm_page_alloc_zone_locked event is triggered. This 
			 * event is important as high amounts of activity imply 
			 * high activity on the zone->lock. Taking this lock
			 * impairs performance by disabling interrupts, dirtying 
			 * cache lines between CPUs and serialising many CPUs.
			 */
			"kmem/mm_page_alloc_zone_locked/"	,

			/* When a page is freed directly by the caller, the only
			 * mm_page_free event is triggered. Significant amounts of 
			 * activity here could indicate that the callers should be 
			 * batching their activities
			 */
			"/kmem/mm_page_free/"			,

			/* When pages are freed in batch, the also mm_page_free_batched
			 * is triggered.Broadly speaking, pages are taken off the LRU
			 * lock in bulk and freed in batch with a page list. Significant 
			 * amounts of activity here could indicate that the system is 
                         * under memory pressure and can also indicate contention on 
                         * the zone->lru_lock
			 */
			"kmem/mm_page_free_batched/"		,
			
			/* External fragmentation affects whether a high-order allocation
			 * will be successful or not. For some types of hardware, this is
			 * important although it is avoided where possible. If the system 
                         * is using huge pages and needs to be able to resize the pool 
                         * over the lifetime of the system, this value is important
			 */
	//		"kmem/mm_page_alloc_extfrag/"		,

			"block/block_rq_issue/"			,
			"block/block_rq_insert/"
                 };

__u64 soft_events[]  = {
		};

struct event_list_map *event_list_map__init() 
{
	int nr_fevents = ARR_SIZE(ftrace_events);
	int nr_sevents = ARR_SIZE(soft_events);

	struct event_list *tmp;
	struct event_list_map *elist_map  = malloc(sizeof(struct event_list_map));
	elist_map->elist = malloc(sizeof(struct event_list));
	elist_map->nr    = 0;

	INIT_LIST_HEAD(&(elist_map->elist->list));

	// Ftrace events
	int i;
	for(i=0; i < nr_fevents; i++) {

                int id = event__get_id(ftrace_events[i]);
                if(id < 0)  {
                        fprintf(stderr,"Failed to get %s id.\n",ftrace_events[i]);
			return NULL;
		}
		else {
			tmp         = malloc(sizeof(struct event_list));
			tmp->config = id;
			tmp->e_type = 1<<elist_map->nr;
			elist_map->nr++;
			tmp->type   = PERF_TYPE_TRACEPOINT;
			list_add_tail(&(tmp->list), &(elist_map->elist->list));
		}
	}

	// Software inbuild events
	for(i=0;i < nr_sevents; i++) {
		
		tmp         = malloc(sizeof(struct event_list));
		tmp->config = soft_events[i];
		tmp->type   = PERF_TYPE_SOFTWARE;
		tmp->e_type = 1<<elist_map->nr;
		elist_map->nr++;
		list_add_tail(&(tmp->list), &(elist_map->elist->list)); 
	}

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
	
		event_map->events[i].e_type	 	 = tmp->e_type;
		event_map->events[i].e_open.cpu  	 = cpu;
		event_map->events[i].e_open.group_id	 = -1;
		event_map->events[i].e_open.flags	 = 0;
		event_map->events[i].e_open.pid  	 = pid;
		
		event_map->events[i].e_open.attr.config  = tmp->config;
		event_map->events[i].e_open.attr.type    = tmp->type;

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
