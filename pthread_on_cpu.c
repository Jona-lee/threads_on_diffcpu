/*
 *
 *  Copyright (c) 2013 javenly@gmail.com
 * 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>

#define MAX_THREADS 8

struct thread_msg {
	char name[20];
	char num_of_cpu;
	char run_cpu;
	char start;
};

int get_cpu_num(void)
{
	long nprocs       = -1;
	long nprocs_max   = -1;

#ifdef _SC_NPROCESSORS_ONLN
	nprocs = sysconf(_SC_NPROCESSORS_ONLN);
	if (nprocs < 1) {
		printf("Could not determine number of CPUs on line, %s\n",
			strerror(errno));
		return 0;
	}
	nprocs_max = sysconf(_SC_NPROCESSORS_CONF);
	if (nprocs_max < 1) {
		printf("Could not determine number of CPUs in host, %s\n",
			strerror(errno));
		return 0;
	}
	printf("%d of %d CPUs online\n", nprocs, nprocs_max);
	return nprocs;
#else
	printf("Could not determine number of CPUs\n");
	return 0;
#endif
}

void *pthread_handle( void *ptr )
{
	struct thread_msg *msg = (struct thread_msg *)ptr;
	cpu_set_t cpuset;
	int maxcpus;
	int i;

	while(!msg->start)
		usleep(10000);

	CPU_ZERO(&cpuset);
	
	if (pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0)
		for (i = 0; i < msg->num_of_cpu; i++)
			if (CPU_ISSET(i, &cpuset))
				printf("%s(ID:%u) : CPU %d\n", msg->name, pthread_self(), i);
	while(1) {
		//do something here.
		sleep(1);
	}
}

int main()
{
	pthread_t thread[MAX_THREADS];
	struct thread_msg tmsg[MAX_THREADS];
	cpu_set_t cpuset[MAX_THREADS];
	int num, ret, i;

	num = get_cpu_num();
	if (num > MAX_THREADS)
		num = MAX_THREADS;

	for (i = 0; i < num; i++) {
		snprintf(tmsg[i].name, sizeof(tmsg[i].name), "thread %d", i);
		tmsg[i].num_of_cpu = num;
		tmsg[i].run_cpu = i;
		tmsg[i].start = 0;
		ret = pthread_create(&thread[i], NULL, &pthread_handle, (void*)&tmsg[i]);
		if (ret != 0) {
			printf("can not creat thread %d, %s\n", i, strerror(errno));
			break;
		}

		CPU_ZERO(&cpuset[i]);
		CPU_SET(i, &cpuset[i]);
    		ret = pthread_setaffinity_np(thread[i], sizeof(cpu_set_t), &cpuset[i]);
		if (ret != 0) {
			printf("can not set thread %d affinity, %s\n", i, strerror(errno));
			break;
		}
		tmsg[i].start = 1;	
		usleep(100000);
	}

	for (i = 0; i < num; i++) {
		pthread_join(thread[i], NULL);
	}

	return 0;
}

