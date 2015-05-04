#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "event_map.h"
#include "perf_event.h"
#include "mmap_page.h"
#include "shared.h"
#include "events.h"

#define SCHED_EVENTS	"sched/sched_switch/"                   ,	\
                        "task/task_newtask/"                    ,

#define FS_EVENTS	"raw_syscalls/sys_enter/"               ,	\
                        "syscalls/sys_enter_open/"              ,	\
                        "syscalls/sys_enter_read/"              ,	\
                        "syscalls/sys_exit_read/"               ,	\
                        "syscalls/sys_enter_write/"             ,	\
                        "syscalls/sys_enter_lseek/"             ,	

/*
Kmalloc 	       : Heavy activity for these events may indicate
                         that a specific cache is justified, particularly
                         if kmalloc slab pages are getting significantly
                         internal fragmented as a result of the allocation
                         pattern. By correlating kmalloc with kfree, it may
                         be possible to identify memory leaks and where
                         the allocation sites were

Kmem_cache_alloc       : These events are similar in usage to the kmalloc-
                         related events except that it is likely easier to
                         pin the event down to a specific cache. At the time
                         of writing, no information is available on what slab
                         is being allocated from, but the call_site can 
                         usually be used to extrapolate that information

mm_page_fault_handle   : The return value of this event can be compared to 
			 check if fault was major/minor(Not availabe in kernel,
			 added later.) If this event is skipped you need to
			 add this event in include/trace/events/kmem.h and 
			 add function 'trace_mm_page_fault_handle(ret)' in
			 function handle_mm_fault()(mm/memory.c). Recompile
			 the kernel, now this event will be visible under
			 kmem.

mm_page_alloc	       : mm_page_alloc is a simple indicator of page
                         allocator activity. Pages may be allocated
                         from the per-CPU allocator (high performance)
                         or the buddy allocator

mm_page_alloc_zone_   : If pages are allocated directly from the buddy 
locked			allocator, the mm_page_alloc_zone_locked event is 
			triggered. This event is important as high amounts 
			of activity imply high activity on the zone->lock. 
			Taking this lock impairs performance by disabling 
			interrupts, dirtying cache lines between CPUs and 
			serialising many CPUs.

mm_page_free	      : When a page is freed directly by the caller, the only
                        mm_page_free event is triggered. Significant amounts 
			of activity here could indicate that the callers 
			should be batching their activities

mm_page_free_batched  : When pages are freed in batch, the also 
			mm_page_free_batched is triggered.Broadly speaking, 
			pages are taken off the LRU lock in bulk and freed 
			in batch with a page list. Significant amounts of 
			activity here could indicate that the system is
                        under memory pressure and can also indicate 
			contention on the zone->lru_lock

mm_page_alloc_extfrag : External fragmentation affects whether a high-order
			allocation will be successful or not. For some types 
			of hardware, this is important although it is avoided 
			where possible. If the system is using huge pages and
			needs to be able to resize the pool over the lifetime 
			of the system, this value is important

mm_page_swap_in	      : It is called when page fault happens and the fields are
			non zero in PTE and contain address in swap area.
			(Indicate presence of page in swap). Returns 1 if page 
			found in swap cache(MINOR), 2 if not found in 
			cache(MAJOR), -1 if error.(Not present in kernel,
			added later. include/trace/events/kmem.h and 
			mm/memory.c)

mm_page_swap_out      : Called when pages are being reclaimed under memory
			pressure, it allocates new page slot in swap area.
			Returns 1 on success and 0 on failure.( Not present
			in kernel, added later)
			include/trace/events/kmem.h and mm/swap_state.c


*/

#define MM_EVENTS	"syscalls/sys_enter_mmap/"              ,	\
                        "syscalls/sys_exit_brk/"                ,	\
                        "kmem/kmalloc/"                         ,	\
			"kmem/kmem_cache_alloc/"                ,	\
			"kmem/mm_page_fault_handle/"            ,	\
			"kmem/mm_page_alloc/"                   ,	\
			"kmem/mm_page_alloc_zone_locked/"       ,	\
			"kmem/mm_page_free/"			,	\
			"kmem/mm_page_free_batched/"            ,	\
			"kmem/mm_page_swap_out/"		,	\
			"kmem/mm_page_swap_in/"			,
//			"kmem/mm_page_alloc_extfrag/"           ,

#define BLK_EVENTS 	"block/block_rq_issue/"                 ,	\
                        "block/block_rq_insert/"		,


/*
net_dev_queue          : Entry point to Device Driver Interface to xmit.
			 Queue a buffer for transmission to a network device.
			 Packet recieved from IP layer. 

net_dev_xmit	       : Transmit skb to device. Called directly if queue is
			 empty.

netif_receive_skb      : Entry point to DDI to process received data.

netif_rx	       : Post buffer(skb) to the network code. This function
			 receives a packet from Device Driver and queues it 
			 for upper levels to process.

 */ 
#define NET_EVENTS	"net/net_dev_queue/"			,	\
			"net/net_dev_xmit/"			,	\
			"net/netif_receive_skb/"		,	\
			"net/netif_rx/"				,	\
			"syscalls/sys_enter_recvfrom/"		,	\
			"syscalls/sys_enter_sendto/"		,


// From src/main.c
extern int opt_sched;
extern int opt_fs;
extern int opt_mm;
extern int opt_blk;
extern int opt_net;
extern int opt_all;
			

// fevents = ftrace tracepoint events
char *sched_fevents[]  = { SCHED_EVENTS  NULL };
char *fs_fevents[]     = { FS_EVENTS     NULL };
char *mm_fevents[]     = { MM_EVENTS     NULL };
char *blk_fevents[]    = { BLK_EVENTS    NULL };	
char *net_fevents[]    = { NET_EVENTS    NULL };

char *all_fevents[] = { 
			  SCHED_EVENTS 
			  FS_EVENTS
			  MM_EVENTS
			  BLK_EVENTS 
			  NET_EVENTS
			  NULL
                      };

// sevents = inbuild software events
__u64 all_sevents[]  = {
		};

int add_fevent_to_list(struct list_head *list, char **events) {
	int nr_events = 0;
	struct event_list *tmp;
	int i = 0;
	while(events[i] != NULL ) {

                int id = event__get_id(events[i]);
                if(id < 0)  {
                        fprintf(stderr,"Failed to get %s id. -- Skipped\n",
				events[i]);
                }
                else {
                        tmp         = malloc(sizeof(struct event_list));
                        tmp->config = id;
			tmp->print_output = get_output_function(events[i],
								&tmp->title);
			nr_events++;
                        tmp->type   = PERF_TYPE_TRACEPOINT;
                        list_add_tail(&(tmp->list),list);
                }
		i++;
        }	
	return nr_events;
}

struct event_list_map *event_list_map__init() 
{

	struct event_list *tmp;
	struct event_list_map *elist_map  = malloc(sizeof(struct event_list_map));
	elist_map->elist = malloc(sizeof(struct event_list));
	elist_map->nr    = 0;

	INIT_LIST_HEAD(&(elist_map->elist->list));

	// Ftrace events
	int i;
	if(opt_all) {
		int ret = add_fevent_to_list(&(elist_map->elist->list),
					     all_fevents);
		if(!ret)
			return NULL;
		elist_map->nr += ret;
	}
	else {
		int ret;
		if(opt_sched) {
			ret = add_fevent_to_list(&(elist_map->elist->list),
						 sched_fevents);
			if(!ret)
				return NULL;
			elist_map->nr += ret;
		}
		if(opt_fs) {
			ret = add_fevent_to_list(&(elist_map->elist->list),
						 fs_fevents);
			if(!ret)
				return NULL;
			elist_map->nr += ret;
		}
		if(opt_mm) {
			ret = add_fevent_to_list(&(elist_map->elist->list),
						 mm_fevents);
			if(!ret)
				return NULL;
			elist_map->nr += ret;
		}
		if(opt_blk) {
			ret = add_fevent_to_list(&(elist_map->elist->list),
						 blk_fevents);
			if(!ret)
				return NULL;
			elist_map->nr += ret;
		}
		if(opt_net) {
			ret = add_fevent_to_list(&(elist_map->elist->list),
						 net_fevents);
			if(!ret)
                                return NULL;
                        elist_map->nr += ret;
		}
	}
/*
	// Software inbuild events
	for(i=0;i < nr_sevents; i++) {
		
		tmp         = malloc(sizeof(struct event_list));
		tmp->config = all_sevents[i];
		tmp->type   = PERF_TYPE_SOFTWARE;
		tmp->e_type = 1<<elist_map->nr;
		elist_map->nr++;
		list_add_tail(&(tmp->list), &(elist_map->elist->list)); 
	}
*/

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
	
		event_map->events[i].print_output	 = tmp->print_output;
		event_map->events[i].title		 = strdup(tmp->title);
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
