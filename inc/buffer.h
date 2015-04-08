#pragma once
#ifndef _H_BUFFER
#define _H_BUFFER

#include <sys/types.h>

struct buf_reader {
	__u64 (*read)(void *buf, __u64 size,void *out_buff);
};

//inline int buf_read(void *buf, int size, struct buf_reader *buf_reader) {
//	return buf_reader->read(buf, size, buf_reader);

#endif
