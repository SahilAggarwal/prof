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
	struct perf_event_attr *attr 	   = out->attr;
	struct probe_buff      *probe_buff = out->probe_buff;
	while(nread < size) {

		struct perf_event_header *header = buf;
		void *sample = (void *)(header + 1);
		
		if(nread + header->size > size) 
			break;

		char str[1024];	
		switch(header->type) {

			case PERF_RECORD_SAMPLE:
				if(attr->sample_type & PERF_SAMPLE_TID) {
                                        struct perf_record_sample_tid *tid = sample;
                                        sprintf(str,"TID: %d, PID: %d",    		\
                                                           tid->tid,   			\
                                                           tid->pid    			);
					sample += sizeof(*tid);
                                }

                                if(attr->sample_type & PERF_SAMPLE_TIME) {
                                        struct perf_record_sample_time *time = sample;
                                        sprintf(str + strlen(str)," TIME: %ld",time->time);
					sample += sizeof(*time);
                                }

				if(attr->sample_type & PERF_SAMPLE_STREAM_ID) {
					struct perf_record_sample_stream_id *id = sample;
					sprintf(str +strlen(str),"ID: %d",id->stream_id);
					sample += sizeof(*id);
				}
			
				if(attr->sample_type & PERF_SAMPLE_RAW) {
					struct perf_record_sample_raw *raw = sample;
					if(out->e_type & SCHED_SWITCH) {
						GET_PROBE_DATA(sched_switch);
						sprintf(str + strlen(str), 	
						" PrevPID: %d PrevComm: %s NextComm: %s NextPID:%d\n",
											data->prev_pid,
											data->prev_comm,
											data->next_comm,
											data->next_pid);

					}
					if(out->e_type & SCHED_WAKEUP) {
						GET_PROBE_DATA(sched_wakeup);
						
						sprintf(str + strlen(str)," Target CPU: %d\n",data->target_cpu);
					}
					if(out->e_type & SYS_ENTER_OPEN) {
						GET_PROBE_DATA(sys_enter_open);
						sprintf(str + strlen(str)," SyscallNo: %lx Mode: %d\n",data->nr,  \
										 	 data->mode);
					}			
					if(out->e_type & SYS_ENTER_READ) {
					//	GET_PROBE_DATA(sys_enter_read)
						struct sys_enter_read *read = (struct sys_enter_read *)raw->data;
					
						sprintf(str + strlen(str), " FD: %d RCount: %d\n",read->fd,
											    	  read->count);
					}
					if(out->e_type & SYS_EXIT_READ) {
						GET_PROBE_DATA(sys_exit_read);
						
						sprintf(str + strlen(str)," Read: %d\n",data->ret);
					}
					if(out->e_type & SYS_ENTER_WRITE) {
						GET_PROBE_DATA(sys_enter_write);

						sprintf(str + strlen(str), " FD: %d WCount: %d\n",data->fd,
												  data->count); 
					}
					if(out->e_type & SYS_ENTER_LSEEK) {
						GET_PROBE_DATA(sys_enter_lseek);
	
						sprintf(str + strlen(str), " FD: %d Off: %d Orig: %d\n",
												data->fd,
												data->offset,
												data->origin); 
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
