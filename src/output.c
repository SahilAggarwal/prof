#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "output.h"
#include "event_list.h"
#include "probe_buff.h"

__u64 write_output(void *buf, __u64 size, void *out_buff)
{
	
	int nread = 0;
	struct output_records  *out	   = (struct output_records *)out_buff;
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
                                        sprintf(str,"TID: %d, PID: %d",    		\
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
			
				if(attr.sample_type & PERF_SAMPLE_RAW) {
					struct perf_record_sample_raw *raw = sample;
					if(out->e_type & SCHED_SWITCH) {
						struct sched_switch *swtch = (struct sched_switch *)raw->data;
						sprintf(str + strlen(str), 	
						" PrevPID: %d PrevComm: %s NextComm: %s NextPID:%d\n",
											swtch->prev_pid,
											swtch->prev_comm,
											swtch->next_comm,
											swtch->next_pid);

					}
					if(out->e_type & SYS_ENTER_OPEN) {
						struct sys_enter_open *open = (struct sys_enter_open *)raw->data;
						sprintf(str + strlen(str)," SyscallNo: %lx Mode: %d\n",open->nr,  \
										 	 open->mode);
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
