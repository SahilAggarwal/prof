#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"
#include "event_map.h"
#include "probe_buff.h"

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

		char str[1024];	
		switch(header->type) {

			case PERF_RECORD_SAMPLE:
				if(attr.sample_type & PERF_SAMPLE_TID) {
                                        struct perf_record_sample_tid *tid = sample;
                                        sprintf(str," TID: %d, PID: %d",    		\
                                                           tid->tid,   			\
                                                           tid->pid    			);
					sample += sizeof(*tid);
                                }

                                if(attr.sample_type & PERF_SAMPLE_TIME) {
                                        struct perf_record_sample_time *time = sample;
                                        sprintf(str + strlen(str)," TIME: %ld",time->time);
					sample += sizeof(*time);
                                }

				if(attr.sample_type & PERF_SAMPLE_STREAM_ID) {
					struct perf_record_sample_stream_id *id = sample;
					sprintf(str +strlen(str),"ID: %d",id->stream_id);
					sample += sizeof(*id);
				}

				if(attr.sample_type & PERF_SAMPLE_CPU) {
					struct perf_record_sample_cpu *cpu = sample;
					sprintf(str + strlen(str)," CPU: %d",cpu->cpu);
					sample += sizeof(*cpu);
				}
			
				if(attr.sample_type & PERF_SAMPLE_RAW) {
					struct perf_record_sample_raw *raw = sample;
					if(out->e_type & CONTEXT_SWITCH) {
						sprintf(str + strlen(str)," CONTEXT_SWITCH\n");
					}

					if(out->e_type & SCHED_SWITCH) {
						GET_PROBE_DATA(sched_switch);

						sprintf(str + strlen(str), 	
						" PrevPID: %d NextPID:%d\n",
								data->prev_pid,
								data->next_pid);

					}
					if(out->e_type & SCHED_WAKEUP) {
						GET_PROBE_DATA(sched_wakeup);
						
						sprintf(str + strlen(str)," SCHED_WAKE CPU: %d\n",data->target_cpu);
					}
					if(out->e_type & SYS_CLONE) {
						GET_PROBE_DATA(sys_clone);
						sprintf(str + strlen(str), " CLONE PID: %d\n",data->ret);
					}
					if(out->e_type & SYS_ENTER_OPEN) {
						GET_PROBE_DATA(sys_enter_open);
						sprintf(str + strlen(str)," EN_OPEN No: %lx Mode: %d\n",data->nr,  \
										 	 data->mode);
					}			
					if(out->e_type & SYS_ENTER_READ) {
						GET_PROBE_DATA(sys_enter_read)
					
						sprintf(str + strlen(str), " EN_READ FD: %d RCount: %d\n",data->fd,
											    	  data->count);
					}
					if(out->e_type & SYS_EXIT_READ) {
						GET_PROBE_DATA(sys_exit_read);
						
						sprintf(str + strlen(str)," EX_READ Read: %d\n",data->ret);
					}
					if(out->e_type & SYS_ENTER_WRITE) {
						GET_PROBE_DATA(sys_enter_write);

						sprintf(str + strlen(str), " WRITE  FD: %d WCount: %d\n",data->fd,
												  data->count); 
					}
					if(out->e_type & SYS_ENTER_LSEEK) {
						GET_PROBE_DATA(sys_enter_lseek);
	
						sprintf(str + strlen(str), " LSEEK FD: %d Off: %d Orig: %d\n",
												data->fd,
												data->offset,
												data->origin); 
					}
					if(out->e_type & SYS_ENTER) {
						GET_PROBE_DATA(sys_enter);

						sprintf(str + strlen(str), " SYS_ENTER Id: %d\n",data->id);
					}
					if(out->e_type & SYS_ENTER_MMAP) {
						GET_PROBE_DATA(sys_enter_mmap);
						
						sprintf(str + strlen(str)," MMAP Len: %d Fd: %d Off: %d\n",
												data->len,
												data->fd,
												data->offset);
					}
					if(out->e_type & MM_PAGE_ALLOC) {
						GET_PROBE_DATA(mm_page_alloc);

						sprintf(str + strlen(str)," PAGEALLOC \n");
					}
					if(out->e_type & BLOCK_ISSUE) {
						GET_PROBE_DATA(block_rq_issue);
						sprintf(str + strlen(str)," BLK_ISSUE Sectors: %d  Mode: %s\n",
													data->nr_sector,
													data->rwbs);
					}
                                        if(out->e_type & BLOCK_INSRT) {
                                                GET_PROBE_DATA(block_rq_issue);

                                                sprintf(str + strlen(str)," BLK_INSERT Sectors: %d  Mode: %s\n",
                                                                                                        data->nr_sector,
                                                                                                        data->rwbs);
                                        }
                                        if(out->e_type & BLOCK_COMPL) {
                                                GET_PROBE_DATA(block_rq_complete);
                                                sprintf(str + strlen(str)," BLK_COMPLT\n");
                                        }
						
					sample += sizeof(*raw);
				}
				probe_buff->write(probe_buff,str,strlen(str));
		};
		nread += header->size;
		buf   += header->size;
	}
	return nread;
		
}		
