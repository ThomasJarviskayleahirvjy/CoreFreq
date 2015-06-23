/*
 * Copyright (C) 2015 CYRIL INGENIERIE
 * Licenses: GPL2
 */

#define	_MAX_CPU_ 	64
#define	TASK_COMM_LEN	16

#define	SHM_DEVNAME "intelfreq"
#define	SHM_FILENAME "/dev/"SHM_DEVNAME

#define	LOOP_MIN_MS	100
#define LOOP_MAX_MS	5000
#define	LOOP_DEF_MS	1000
/*
#define RDMSR(_lo, _hi,  _reg)	\
	__asm__ volatile	\
	(			\
		"rdmsr ;"	\
                : "=a" (_lo),	\
		  "=d" (_hi)	\
		: "c" (_reg)	\
	);

#define WRMSR(_lo, _hi,  _reg)	\
	__asm__ volatile	\
	(			\
		"wrmsr ;"	\
		:		\
		: "c" (_reg),	\
		  "a" (_lo),	\
		  "d" (_hi)	\
	);
*/
#define RDMSR(_val, _reg)	\
({				\
	__u32 _lo, _hi;	\
				\
	__asm__ volatile	\
	(			\
		"rdmsr ;"	\
                : "=a" (_lo),	\
		  "=d" (_hi)	\
		: "c" (_reg)	\
	);			\
	_val.qword=_lo | ((__u64) _hi << 32);	\
})
#define WRMSR(_val,  _reg)	\
	__asm__ volatile	\
	(			\
		"wrmsr ;"	\
		:		\
		: "c" (_reg),	\
		  "a" ((__u32) _val.qword & 0xFFFFFFFF),	\
		  "d" ((__u32) (_val.qword >> 32))	\
	);

typedef struct
{
	struct
	{
		__u8 Chr[4];
	} AX, BX, CX, DX;
} BRAND;

/*
typedef	struct
{
	__u64
		ReservedBits1	:  8-0,
		MaxBusRatio	: 13-8,
		ReservedBits2	: 50-13,
		PlatformId	: 53-50,
		ReservedBits3	: 64-53;
} PLATFORM_ID;

typedef	struct
{
	__u64
		Bus_Speed	:  3-0,
		ReservedBits	: 64-3;
} FSB_FREQ;

typedef	struct
{
	__u64
		CurrentRatio	: 16-0,
		ReservedBits1	: 31-16,
		XE		: 32-31,
		ReservedBits2	: 40-32,
		MaxBusRatio	: 45-40,
		ReservedBits3	: 46-45,
		NonInt_BusRatio	: 47-46,
		ReservedBits4	: 64-47;
} PERF_STATUS;

typedef	struct
{
	__u64
		EIST_Target	: 16-0,
		ReservedBits1	: 32-16,
		Turbo_IDA	: 33-32,
		ReservedBits2	: 64-33;
} PERF_CONTROL;
*/
typedef union
{
	__u64	qword;
	struct
	{
		__u64
		ReservedBits1	:  8-0,
		MaxNonTurboRatio: 16-8,
		ReservedBits2	: 28-16,
		Ratio_Limited	: 29-28,
		TDC_TDP_Limited	: 30-29,
		ReservedBits3	: 32-30,
		LowPowerMode	: 33-32,
		ConfigTDPlevels	: 35-33,
		ReservedBits4	: 40-35,
		MinimumRatio	: 48-40,
		MinOpeRatio	: 56-48,
		ReservedBits5	: 64-56;
	};
} PLATFORM_INFO;
/*
typedef struct
{
	__u64
		Pkg_CST_Limit	:  3-0,
		ReservedBits1	: 10-3,
		IO_MWAIT_Redir	: 11-10,
		ReservedBits2	: 15-11,
		CFG_Lock	: 16-15,
		ReservedBits3	: 24-16,
		Int_Filtering	: 25-24,	// Nehalem
		C3autoDemotion	: 26-25,
		C1autoDemotion	: 27-26,
		C3undemotion	: 28-27,	// Sandy Bridge
		C1undemotion	: 29-28,	// Sandy Bridge
		ReservedBits4	: 64-29;
} CSTATE_CONFIG;
*/
typedef union
{
	__u64	qword;
	struct
	{
		__u64
		MaxRatio_1C	:  8-0,
		MaxRatio_2C	: 16-8,
		MaxRatio_3C	: 24-16,
		MaxRatio_4C	: 32-24,
		MaxRatio_5C	: 40-32,
		MaxRatio_6C	: 48-40,
		MaxRatio_7C	: 56-48,
		MaxRatio_8C	: 64-56;
	};
} TURBO_RATIO;

/*
typedef	struct
{
	__u64
		FastStrings	:  1-0,
		ReservedBits1	:  3-1,
		TCC		:  4-3,
		ReservedBits2	:  7-4,
		PerfMonitoring	:  8-7,
		ReservedBits3	: 11-8,
		BTS		: 12-11,
		PEBS		: 13-12,
		TM2_Enable	: 14-13,
		ReservedBits4	: 16-14,
		EIST		: 17-16,
		ReservedBits5	: 18-17,
		FSM		: 19-18,
		ReservedBits6	: 22-19,
		CPUID_MaxVal	: 23-22,
		xTPR		: 24-23,
		ReservedBits7	: 34-24,
		XD_Bit		: 35-34,
		ReservedBits8	: 37-35,
		DCU_Prefetcher	: 38-37,
		Turbo_IDA	: 39-38,
		IP_Prefetcher	: 40-39,
		ReservedBits9	: 64-40;
} MISC_PROC_FEATURES;

typedef struct
{
	__u64
		Type		:  8-0,
		ReservedBits1	: 10-8,
		FixeRange	: 11-10,
		Enable		: 12-11,
		ReservedBits2	: 64-12;
} MTRR_DEF_TYPE;
*/
typedef union
{
	__u64	qword;
	struct
	{
		__u64
		EN_PMC0		:  1-0,
		EN_PMC1		:  2-1,
		EN_PMC2		:  3-2,
		EN_PMC3		:  4-3,
		EN_PMCn		: 32-4,
		EN_FIXED_CTR0	: 33-32,
		EN_FIXED_CTR1	: 34-33,
		EN_FIXED_CTR2	: 35-34,
		ReservedBits2	: 64-35;
	};
} GLOBAL_PERF_COUNTER;

typedef union
{
	__u64	qword;
	struct
	{
		__u64
		EN0_OS		:  1-0,
		EN0_Usr		:  2-1,
		AnyThread_EN0	:  3-2,
		EN0_PMI		:  4-3,
		EN1_OS		:  5-4,
		EN1_Usr		:  6-5,
		AnyThread_EN1	:  7-6,
		EN1_PMI		:  8-7,
		EN2_OS		:  9-8,
		EN2_Usr		: 10-9,
		AnyThread_EN2	: 11-10,
		EN2_PMI		: 12-11,
		ReservedBits	: 64-12;
	};
} FIXED_PERF_COUNTER;

typedef union
{
	__u64	qword;
	struct
	{
		__u64
		Overflow_PMC0	:  1-0,
		Overflow_PMC1	:  2-1,
		Overflow_PMC2	:  3-2,
		Overflow_PMC3	:  4-3,
		Overflow_PMCn	: 32-4,
		Overflow_CTR0	: 33-32,
		Overflow_CTR1	: 34-33,
		Overflow_CTR2	: 35-34,
		ReservedBits2	: 61-35,
		Overflow_UNC	: 62-61,
		Overflow_Buf	: 63-62,
		Ovf_CondChg	: 64-63;
	};
} GLOBAL_PERF_STATUS;

typedef union
{
	__u64	qword;
	struct
	{
		__u64
		Clear_Ovf_PMC0	:  1-0,
		Clear_Ovf_PMC1	:  2-1,
		Clear_Ovf_PMC2	:  3-2,
		Clear_Ovf_PMC3	:  4-3,
		Clear_Ovf_PMCn	: 32-2,
		Clear_Ovf_CTR0 	: 33-32,
		Clear_Ovf_CTR1	: 34-33,
		Clear_Ovf_CTR2	: 35-34,
		ReservedBits2	: 61-35,
		Clear_Ovf_UNC	: 62-61,
		Clear_Ovf_Buf	: 63-62,
		Clear_CondChg	: 64-63;
	};
} GLOBAL_PERF_OVF_CTRL;

typedef union
{
	__u64	qword;
	struct
	{
		__u64
		StatusBit       :  1-0,
		StatusLog       :  2-1,
		PROCHOT         :  3-2,
		PROCHOTLog      :  4-3,
		CriticalTemp    :  5-4,
		CriticalTempLog :  6-5,
		Threshold1      :  7-6,
		Threshold1Log   :  8-7,
		Threshold2      :  9-8,
		Threshold2Log   : 10-9,
		PowerLimit      : 11-10,
		PowerLimitLog   : 12-11,
		ReservedBits1   : 16-12,
		DTS             : 23-16,
		ReservedBits2   : 27-23,
		Resolution      : 31-27,
		ReadingValid    : 32-31,
		ReservedBits3   : 64-32;
	};
} THERM_STATUS;


typedef union
{
	__u64	qword;
	struct
	{
		__u64
		ReservedBits1   : 16-0,
		Target          : 24-16,
		ReservedBits2   : 64-24;
	};
} TJMAX;

enum { INIT=0, END=1, CYCLE=2, START=2, STOP=3 };

typedef struct
{
	atomic_ullong		Sync;

	__u32			Bind;
	struct task_struct	*TID[STOP];

	struct SAVEAREA
	{
		GLOBAL_PERF_COUNTER	GlobalPerfCounter;
		FIXED_PERF_COUNTER	FixedPerfCounter;
	} SaveArea;

	struct
	{
		union
		{
			__u64 qword;
		}
				INST[2];
		struct
		{
			union
			{
				__u64 qword;
			}
				UCC,
				URC;
		}		C0[2];
		union
		{
			__u64 qword;
		}
				C3[2],
				C6[2],
				C7[2],
				TSC[2];

		__u64		C1[2];
	} Cycles;

	struct
	{
		__u64
				INST;
		struct
		{
		__u64
				UCC,
				URC;
		}		C0;
		__u64
				C3,
				C6,
				C7,
				TSC,
				C1;
	} Delta;

	int			Temp;
	TJMAX			TjMax;
	THERM_STATUS		ThermStat;
} CORE;


typedef struct
{
	__u32			msleep;

	struct {
		__u32		Count,
				OnLine;
	} CPU;

	char			Brand[48+1];
	__u32			Boost[1+1+8],
				PerCore,
				Clock;

	CORE			Core[_MAX_CPU_];
} PROC;
