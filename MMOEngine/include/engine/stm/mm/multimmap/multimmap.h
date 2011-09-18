#ifndef MULTIMMAP_H
#define MULTIMMAP_H

#include <linux/ioctl.h>

#define MULTIMMAP_IOC_MAGIC 'M'

struct multimmap_statics_info {
	unsigned long start;
	unsigned long size;
};

struct multimmap_kernel_memory_info {
	unsigned long heap_address;
	unsigned long statics_address;
	volatile unsigned* kel_address;
	unsigned desired_kel;
};

#define MULTIMMAP_INIT_STATICS  _IOW(MULTIMMAP_IOC_MAGIC, 1, struct multimmap_statics_info*)

#define MULTIMMAP_CREATE  _IOR(MULTIMMAP_IOC_MAGIC, 10, unsigned)
#define MULTIMMAP_DESTROY _IOW(MULTIMMAP_IOC_MAGIC, 11, unsigned)
#define MULTIMMAP_SELECT  _IOW(MULTIMMAP_IOC_MAGIC, 12, unsigned)
#define MULTIMMAP_MAP_STATIC_SHADOW  _IOW(MULTIMMAP_IOC_MAGIC, 13, int)
#define MULTIMMAP_GET_HEAP_PAGES  _IOR(MULTIMMAP_IOC_MAGIC, 14, unsigned)

#define MULTIMMAP_KERNEL_MEMORY_RW  _IOW(MULTIMMAP_IOC_MAGIC, 21, struct multimmap_kernel_memory_info*)
#define MULTIMMAP_INIT_KERNEL_STATICS  _IOW(MULTIMMAP_IOC_MAGIC, 22, struct multimmap_statics_info*)

#define MULTIMMAP_SHARED_MEMORY_RW  _IOW(MULTIMMAP_IOC_MAGIC, 31, int)

#define MULTIMMAP_IOC_MAXNR 31

#ifdef __KERNEL__
#define MULTIMMAP_HEAP_SIZE PGDIR_SIZE
#define MULTIMMAP_STATICS_SIZE PUD_SIZE
#else
#define MULTIMMAP_HEAP_SIZE (512*512*512*4096UL)
#define MULTIMMAP_STATICS_SIZE (512*512*4096UL)
#endif

#define MULTIMMAP_KERNEL_HEAP_OFFSET (2*MULTIMMAP_HEAP_SIZE)

#define MULTIMMAP_KERNEL_STATICS_OFFSET (MULTIMMAP_KERNEL_HEAP_OFFSET + MULTIMMAP_HEAP_SIZE)

#ifdef __KERNEL__
#define MULTIMMAP_KERNEL_STATICS_SIZE PMD_SIZE
#else
#define MULTIMMAP_KERNEL_STATICS_SIZE (512*4096UL)
#endif

#define MULTIMMAP_SHARED_MEMORY_OFFSET (MULTIMMAP_KERNEL_STATICS_OFFSET + MULTIMMAP_KERNEL_STATICS_SIZE)

#ifdef __KERNEL__
#define MULTIMMAP_SHARED_MEMORY_SIZE PUD_SIZE
#else
#define MULTIMMAP_SHARED_MEMORY_SIZE (512*512*4096UL)
#endif

#endif
