#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include "probe_buff.h"

#define PROBE_BUFF_SIZE 4096*2
#define OUTPUT_FILE "perf.text"

struct probe_buff *probe_buff__init()
{
	struct probe_buff *probe_buff = malloc(sizeof(*probe_buff));

	int fd = probe_buff__get_file();
	if(fd < 0) {
		fprintf(stderr,"Failed to get output file FD");
		return NULL;
	}
	probe_buff->fd   = fd;
	probe_buff->size = PROBE_BUFF_SIZE;
	probe_buff->base = mmap(NULL, PROBE_BUFF_SIZE, PROT_READ | PROT_WRITE, \
				MAP_SHARED | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
	if(probe_buff->base == MAP_FAILED) {
		fprintf(stderr,"Failed to mmap probe buffer\n");
		return NULL;
	}	
	probe_buff->nwrite = 0;

	probe_buff->write  = probe_buff__write; 
	
	if(sem_init(&probe_buff->sem,0,0)) {
		fprintf(stderr,"Failed to init sem\n");
		return NULL;
	}
	sem_post(&probe_buff->sem);
	return probe_buff;
}

int probe_buff__write(struct probe_buff *probe_buff,char *buff,int len)
{
	sem_wait(&probe_buff->sem);
	int nleft = probe_buff->size - probe_buff->nwrite;

	if(len > nleft) {
		fprintf(stderr,"Buffer overflow, flushing out...\n");
		if(probe_buff__flush(probe_buff) <= 0) {
			fprintf(stderr,"Failed to flush out\n ");
			return 0;
		}
		probe_buff->nwrite = 0;
	}
	
	memcpy(probe_buff->base + probe_buff->nwrite,buff,len);	
	probe_buff->nwrite += len;

	sem_post(&probe_buff->sem);
}

int probe_buff__flush(struct probe_buff *probe_buff)
{
	int nwrite = write(probe_buff->fd,probe_buff->base,probe_buff->nwrite);
	return nwrite;
}

int probe_buff__get_file()
{
	FILE *fp = NULL;
	fp = fopen(OUTPUT_FILE,"w");
	if(fp == NULL) {
		fprintf(stderr,"Failed to open output file\n");
		return -1;
	}
	return fileno(fp);
}
