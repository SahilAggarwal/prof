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

				out->print(&raw, str + strlen(str)   );
					
				sample += sizeof(*raw);
			}
			probe_buff->write(probe_buff,str,strlen(str));
		}
		nread += header->size;
		buf   += header->size;
	}
	return nread;
}		
