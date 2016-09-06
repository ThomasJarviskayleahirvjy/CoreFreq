/*
 * CoreFreq
 * Copyright (C) 2015-2016 CYRIL INGENIERIE
 * Licenses: GPL2
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>

#include "intelasm.h"
#include "coretypes.h"
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

void Cycles(SHM_STRUCT *Shm)
{
	unsigned int cpu=0;
	while(!Shutdown)
	{
		while(!BITWISEAND(Shm->Proc.Sync, 0x1) && !Shutdown)
			usleep(Shm->Proc.msleep * 100);
		BITCLR(Shm->Proc.Sync, 0);

		printf("CPU  Frequency  Ratio   Turbo"			\
			"    C0      C1      C3      C6      C7"	\
			"    Temps\n");
		for(cpu=0; (cpu < Shm->Proc.CPU.Count) && !Shutdown; cpu++)
		if(!Shm->Cpu[cpu].OffLine)
		{
		    struct FLIP_FLOP *Flop=				\
			&Shm->Cpu[cpu].FlipFlop[!Shm->Cpu[cpu].Toggle];

		printf("#%02u %7.2fMHz (%5.2f)"				\
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
		printf("\nAverage C-states\n"				\
		"Turbo\t  C0\t  C1\t  C3\t  C6\t  C7\n"			\
		"%6.2f%%\t%6.2f%%\t%6.2f%%\t%6.2f\t%6.2f%%\t%6.2f%%\n\n",
			100.f * Shm->Proc.Avg.Turbo,
			100.f * Shm->Proc.Avg.C0,
			100.f * Shm->Proc.Avg.C1,
			100.f * Shm->Proc.Avg.C3,
			100.f * Shm->Proc.Avg.C6,
			100.f * Shm->Proc.Avg.C7);
	}
}

void Instructions(SHM_STRUCT *Shm)
{
	unsigned int cpu=0;
	while(!Shutdown)
	{
		while(!BITWISEAND(Shm->Proc.Sync, 0x1) && !Shutdown)
			usleep(Shm->Proc.msleep * 100);
		BITCLR(Shm->Proc.Sync, 0);

		printf("CPU     IPS            IPC            CPI\n");
		for(cpu=0; (cpu < Shm->Proc.CPU.Count) && !Shutdown; cpu++)
		    if(!Shm->Cpu[cpu].OffLine)
		    {
		    struct FLIP_FLOP *Flop=				\
			&Shm->Cpu[cpu].FlipFlop[!Shm->Cpu[cpu].Toggle];

		    printf("#%02u %12.6f/s %12.6f/c %12.6f/i\n",
			cpu,
			Flop->State.IPS,
			Flop->State.IPC,
			Flop->State.CPI);
		    }
		printf("\n");
	}
}

void Topology(SHM_STRUCT *Shm)
{
	unsigned int cpu=0, level=0x0;

	while(!BITWISEAND(Shm->Proc.Sync, 0x1) && !Shutdown)
		usleep(Shm->Proc.msleep * 100);
	BITCLR(Shm->Proc.Sync, 0);

	printf(	"CPU       ApicID CoreID ThreadID"		\
		" x2APIC Enable Caches Inst Data Unified\n");
	for(cpu=0; cpu < Shm->Proc.CPU.Count; cpu++)
	{
	    printf(	"#%02u%-5s  %6d %6d   %6d"		\
			"    %3s    %c     |  ",
		cpu,
		(Shm->Cpu[cpu].Topology.BSP) ? "(BSP)" : "(AP)",
		Shm->Cpu[cpu].Topology.ApicID,
		Shm->Cpu[cpu].Topology.CoreID,
		Shm->Cpu[cpu].Topology.ThreadID,
		(Shm->Cpu[cpu].Topology.x2APIC) ? "ON" : "OFF",
		(Shm->Cpu[cpu].Topology.Enable) ? 'Y' : 'N');
	    for(level=0; level < CACHE_MAX_LEVEL; level++)
		if(Shm->Cpu[cpu].Topology.Enable)
			printf(	" %-u",
				Shm->Cpu[cpu].Topology.Cache[level].Size);
	    printf("\n");
	}
}

void SysInfo(SHM_STRUCT *Shm)
{
	int i=0;
	printf(	"  Processor [%s]\n"					\
		"  Architecture [%s]\n"					\
		"  %u/%u CPU Online.\n"					\
		"  Ratio Boost:     Min Max  8C  7C  6C  5C  4C  3C  2C  1C\n"\
		"                   ",
		Shm->Proc.Brand,
		Shm->Proc.Architecture,
		Shm->Proc.CPU.OnLine,
		Shm->Proc.CPU.Count	);
	for(i=0; i < 1+1+8; i++)
		if(Shm->Proc.Boost[i] != 0)
			printf("%3d ", Shm->Proc.Boost[i]);
		else
			printf("  - ");
	printf(	"\n"							\
		"  Technologies:\n"					\
		"  |- Time Stamp Counter                    TSC [%9s]\n"\
		"  |- Hyper-Threading                       HTT       [%3s]\n"\
		"  |- Turbo Boost                           IDA       [%3s]\n"\
		"  |- SpeedStep                            EIST       [%3s]\n"\
		"  |- Performance Monitoring                 PM       [%3d]\n",
		Shm->Proc.InvariantTSC ? "Invariant" : "Variant",
		enabled(Shm->Proc.HyperThreading),
		enabled(Shm->Proc.TurboBoost),
		enabled(Shm->Proc.SpeedStep),
		Shm->Proc.PM_version	);
}

int main(int argc, char *argv[])
{
	struct stat shmStat={0};
	SHM_STRUCT *Shm;
	int fd=-1, rc=0;
	char option=(argc == 2) ? ((argv[1][0] == '-') ? argv[1][1] : 'h'):'\0';
	if(option == 'h')
		printf(	"usage:\t%s [-option]\n"		\
			"\t-c\tMonitor Cycles\n"		\
			"\t-i\tMonitor Instructions\n"		\
			"\t-s\tPrint System Information\n"	\
			"\t-t\tPrint Topology\n"		\
			"\t-h\tPrint out this message\n"	\
			"\nExit status:\n"			\
				"0\tif OK,\n"			\
				"1\tif problems,\n"		\
				">1\tif serious trouble.\n"	\
			"\nReport bugs to labs[at]cyring.fr\n", argv[0]);

	else if(((fd=shm_open(SHM_FILENAME, O_RDWR,
			S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)) !=-1)
		&& ((fstat(fd, &shmStat) != -1)
		&& ((Shm=mmap(0, shmStat.st_size,
			PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) != MAP_FAILED)))
	   {
		printf(	"CoreFreq Client."	\
			"  Copyright (C) 2015-2016 CYRIL INGENIERIE\n\n");

		signal(SIGINT, Emergency);
		signal(SIGQUIT, Emergency);
		signal(SIGTERM, Emergency);

		switch(option)
		{
			case 's': SysInfo(Shm);
			break;
			case 't': Topology(Shm);
			break;
			case 'i': Instructions(Shm);
			break;
			case 'c':
			default:  Cycles(Shm);
			break;
		}
		munmap(Shm, shmStat.st_size);
		close(fd);
	    }
		else rc=2;
	return(rc);
}
