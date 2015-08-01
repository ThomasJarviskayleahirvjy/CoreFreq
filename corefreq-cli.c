/*
 * Copyright (C) 2015 CYRIL INGENIERIE
 * Licenses: GPL2
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>

#include "corefreq.h"

unsigned int Shutdown=0x0;

void Emergency(int caught)
{
	switch(caught)
	{
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
			Shutdown=0x1;
		break;
	}
}

int main(int argc, char *argv[])
{
	struct stat shmStat={0};
	SHM_STRUCT *Shm;
	unsigned int cpu=0;
	int fd=-1, rc=0;
	char option=(argc == 2) && (argv[1][0] == '-') ? argv[1][1] : 'c';

	if(((fd=shm_open(SHM_FILENAME, O_RDWR,
			S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) !=-1)
	&& ((fstat(fd, &shmStat) != -1)
	&& ((Shm=mmap(0, shmStat.st_size,
		      PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) != MAP_FAILED)))
	{
	    double Clock=Shm->Proc.Clock.Q				\
			+ ((double) Shm->Proc.Clock.R			\
			/ (Shm->Proc.Boost[1] * 100000L));

	    printf(	"CoreFreq-Cli [%s] , Clock @ %.2f MHz\n\n",
			Shm->Proc.Brand, Clock);

	    signal(SIGINT, Emergency);
	    signal(SIGQUIT, Emergency);
	    signal(SIGTERM, Emergency);

	    while(!Shutdown)
	    {
		while(!BITWISEAND(Shm->Proc.Sync, 0x1) && !Shutdown)
			usleep(Shm->Proc.msleep * 100);
		BITCLR(Shm->Proc.Sync, 0);

		switch(option)
		{
		case 'i':
		    {
		    printf("CPU     IPS            IPC            CPI\n");
		    for(cpu=0; (cpu < Shm->Proc.CPU.Count) && !Shutdown; cpu++)
		        if(!Shm->Cpu[cpu].OffLine)
		        {
			struct FLIP_FLOP *Flop=				      \
				&Shm->Cpu[cpu].FlipFlop[!Shm->Cpu[cpu].Toggle];

			printf("#%02u %12.6f/s %12.6f/c %12.6f/i\n",
				cpu,
				Flop->State.IPS,
				Flop->State.IPC,
				Flop->State.CPI);
			}
		    printf("\n");
		    }
		break;
		case 'c':
		default:
		    {
		    printf("CPU  Frequency  Ratio   Turbo"		\
			   "    C0      C1      C3      C6      C7"	\
			   "    Temps\n");
		    for(cpu=0; (cpu < Shm->Proc.CPU.Count) && !Shutdown; cpu++)
		        if(!Shm->Cpu[cpu].OffLine)
		        {
			struct FLIP_FLOP *Flop=				      \
				&Shm->Cpu[cpu].FlipFlop[!Shm->Cpu[cpu].Toggle];

			printf("#%02u %7.2fMHz (%5.2f)"			\
			    " %6.2f%% %6.2f%% %6.2f%% %6.2f%% %6.2f%% %6.2f%%"\
			    " @ %llu°C\n",
				cpu,
				Flop->Relative.Freq,
				Flop->Relative.Ratio,
				100.f * Flop->State.Turbo,
				100.f * Flop->State.C0,
				100.f * Flop->State.C1,
				100.f * Flop->State.C3,
				100.f * Flop->State.C6,
				100.f * Flop->State.C7,
				Flop->Temperature);
			}
		    printf("\nAverage C-states\n"			\
		       "Turbo\t  C0\t  C1\t  C3\t  C6\t  C7\n"		\
		       "%6.2f%%\t%6.2f%%\t%6.2f%%\t%6.2f\t%6.2f%%\t%6.2f%%\n\n",
				100.f * Shm->Proc.Avg.Turbo,
				100.f * Shm->Proc.Avg.C0,
				100.f * Shm->Proc.Avg.C1,
				100.f * Shm->Proc.Avg.C3,
				100.f * Shm->Proc.Avg.C6,
				100.f * Shm->Proc.Avg.C7);
		    }
		break;
		}
	    }
	    munmap(Shm, shmStat.st_size);
	    close(fd);
	}
	else rc=1;
	return(rc);
}
