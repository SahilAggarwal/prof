#ifndef _H_PROBE
#define _H_PROBE

#include "event_list.h"

struct perf_probe_thread {
        struct event_list_node  *e_list;
        pthread_t               thread;
};

int probe_start(char *,char *,pid_t);

static int probe_start_exec(char *,char *);

static int probe_start_pid(pid_t);

static int probe_initiate(pid_t);

#endif
