#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"
#include "event_map.h"
#include "probe_buff.h"
#include "events.h"

#define GET_PROBE_DATA(type)  \
	struct type *data = (struct type *) raw->data;

__u64 write_output(void *buf, __u64 size, void *out_buff)
{
	
	int nread = 0;
	struct output  *out	   	   = (struct output *)out_buff;
	struct perf_event_attr attr 	   = out->attr;
	struct probe_buff      *probe_buff = out->probe_buff;
	while(nread < size) {

		struct perf_event_header *header = buf;
		void *sample = (void *)(header + 1);
		
		if(nread + header->size > size) 
			break;

		switch(header->type) {

		char str[1024];
		case PERF_RECORD_SAMPLE:
			if(attr.sample_type & PERF_SAMPLE_TID) 
				sample_tid_entry(&sample,		\
						 str + 0		);

			if(attr.sample_type & PERF_SAMPLE_TIME)
				sample_time_entry(&sample, 		\
						  str + strlen(str)	);

			if(attr.sample_type & PERF_SAMPLE_CPU)
				sample_cpu_entry(&sample,		\
						 str + strlen(str)	);
		
			if(attr.sample_type & PERF_SAMPLE_RAW) {
				struct perf_record_sample_raw *tmp = sample;
				void *raw = tmp->data;

				switch(out->e_type) {

				case SCHED_SWITCH:
					sample_raw_sched_switch_entry(&raw,  \
						         str + strlen(str)   );
					break;

				case TASK_NEW:
					sample_raw_newtask_entry(&raw,	     \
						    str + strlen(str)        );
					break;

				case SYS_ENTER_OPEN:
					sample_raw_sys_enter_entry(&raw,     \
                                                      str + strlen(str)      );
					break;

				case SYS_ENTER_READ:
					sample_raw_enter_read_entry(&raw,    \
                                                       str + strlen(str)     );
                                        break;

				case SYS_EXIT_READ:
					sample_raw_exit_read_entry(&raw,      \
                                                       str + strlen(str)      );
                                        break;

				case SYS_ENTER_WRITE:
					sample_raw_enter_write_entry(&raw,    \
                                                        str + strlen(str)     );
                                        break;

				case SYS_ENTER_LSEEK:
					sample_raw_enter_lseek_entry(&raw,    \
                                                        str + strlen(str)     );
                                        break;

				case SYS_ENTER:
					sample_raw_sys_enter_entry(&raw,      \
                                                      str + strlen(str)       );
                                        break;

				case SYS_ENTER_MMAP:
					sample_raw_enter_mmap_entry(&raw,      \
                                                      str + strlen(str)        );
                                        break;

				case SYS_EXIT_BRK:
					sample_raw_sys_exit_brk_entry(&raw,    \
							str + strlen(str)      );
                                        break;

				case KMALLOC:
					sample_raw_kmalloc_entry(&raw, 	       \
							str + strlen(str)      );
					break;

				case KMEM_CACHE_ALLOC:
					sample_raw_kmem_cache_entry(&raw,      \
							str + strlen(str)      );
					break;

				case PAGE_FAULT_HANDLE:
					sample_raw_page_fault_entry(&raw,      \
                                                      str + strlen(str)        );
                                        break;					

				case MM_PAGE_ALLOC:
					sample_raw_page_alloc_entry(&raw,      \
                                                      str + strlen(str)        );
                                        break;
				
				case MM_PAGE_ALLOC_ZONE:
					sample_raw_page_alloc_zone_entry(&raw, \
						      str + strlen(str)        );
					break;

				case MM_PAGE_FREE:
                                        sample_raw_page_free_entry(&raw,       \
                                                      str + strlen(str)        );
                                        break;

				case MM_PAGE_FREE_BATCHED:
                                        sample_raw_page_free_batched_entry(&raw, \
                                                      str + strlen(str)          );
                                        break;

				case BLOCK_ISSUE:
					sample_raw_block_issue_entry(&raw,     \
                                                      str + strlen(str)        );
                                        break;

				case BLOCK_INSRT:
					sample_raw_block_insert_entry(&raw,    \
                                                      str + strlen(str)        );
                                        break;

					
				sample += sizeof(*raw);
				}
				probe_buff->write(probe_buff,str,strlen(str));
			}
		}
		nread += header->size;
		buf   += header->size;
	}
	return nread;
}		
