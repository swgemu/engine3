#include "multimmap.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/vmalloc.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/hugetlb.h>
#include <linux/mman.h>
#include <linux/swap.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/version.h>

#include <asm/tlb.h>
#include <asm/uaccess.h>

#define INITIAL_PGDS_SIZE 16
#define INITIAL_UNMAP_ENTRIES_SIZE 4
#define INVALID_PGD_INDEX ((unsigned)0xffffffff)
#define STATICS_PGOFF (MULTIMMAP_HEAP_SIZE/PAGE_SIZE)
#define KERNEL_HEAP_PGOFF (MULTIMMAP_KERNEL_HEAP_OFFSET/PAGE_SIZE)
#define KERNEL_STATICS_PGOFF (MULTIMMAP_KERNEL_STATICS_OFFSET/PAGE_SIZE)
#define SHARED_MEMORY_PGOFF (MULTIMMAP_SHARED_MEMORY_OFFSET/PAGE_SIZE)

typedef void (*multimmap_pmd_fn_t)(void* arg);

#define IS_LICENSE_GPL 1

#if defined(IS_LICENSE_GPL) 
MODULE_LICENSE("GPL");
#else
MODULE_LICENSE("Proprietary");
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)
 
#define device_create(_cls, _parent, _devt, _device, _fmt, _num)        \
    class_device_create(_cls, _parent, _devt, _device, _fmt, _num)
 
#define device_destroy(_cls, _device) \
    class_device_destroy(_cls, _device)
 
#else

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)

#define device_create(_cls, _parent, _devt, _device, _fmt, _num)             \
    device_create_drvdata(_cls, _parent, _devt, _device, _fmt, _num)

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27) */

#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26) */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
pgprot_t protection_map[16] = {
    __P000, __P001, __P010, __P011, __P100, __P101, __P110, __P111,
    __S000, __S001, __S010, __S011, __S100, __S101, __S110, __S111
};

pgprot_t vm_get_page_prot(unsigned long vm_flags)
{
    return protection_map[vm_flags &
                          (VM_READ|VM_WRITE|VM_EXEC|VM_SHARED)];
}
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19) */

static unsigned numdevs = 4;
module_param(numdevs, uint, S_IRUGO);

static unsigned major = 0;
module_param(major, uint, S_IRUGO);

static unsigned minor = 0;

#define DEVICE_NAME "multimmap"

#if IS_LICENSE_GPL
static struct class* multimmap_class;
#endif

/**
 * Structure to describe statics for the current process.
 */
struct multimmap_statics {
    /**
     * The VMA describing the statics area
     */
    struct vm_area_struct* vma;

    /**
     * The number of pages in the statics area.
     */
    unsigned long num_pages;

    /**
     * Our copy of the statics area in pages
     */
    struct page** shadow;

    /**
     * The PUD for the shadow area
     */
    unsigned long shadow_pud;

    /**
     * Indicate if the shadow area is mapped.
     */
    int shadow_mapped;
};

/**
 * Info with PGDs
 */
struct multimmap_pgd_info {
    /**
     * The heap PGD. If INVALID_PGD_INDEX, the union contains the index of the
     * next free PGD.
     */
    unsigned long heap;

    /**
     * The number of pages allocated for this heap either in the heap area or
     * the statics area
     */
    unsigned long num_pages;

    union {
        /**
         * The statics PMD
         */
        unsigned long statics;

        /**
         * The index of the next free PGD
         */
        unsigned next_free_pgd;
    } u;
};

/**
 * The type of the page directory entry to unmap
 */
typedef enum {
    UNMAP_PGD,

    UNMAP_PUD,

    UNMAP_PMD
} unmap_type;

/**
 * Info about some page directory entry that should be unmapped.
 */
struct multimmap_unmap_info {
    /**
     * The type of the entry
     */
    unmap_type type;

    /**
     * The actual value.
     */
    unsigned long entry;
};

struct multimmap_dev {
    struct cdev cdev;

    /**
     * Spinlock to protect common stuff that may be accessed from several
     * processes. If used in combination with mmap_sem, mmap_sem should be 
     * locked first!
     */
    struct semaphore sema;

    /**
     * The number of users of the device.
     */
    unsigned users;

    /**
     * Our VMA
     */
    struct vm_area_struct* vma;

    /**
     * The statics area used by this device, if any.
     */
    struct multimmap_statics* statics;

    /**
     * The index of the current PGD or INVALID_PGD_INDEX
     */
    unsigned current_pgd;
    
    /**
     * The total size of the pgds array.
     */
    unsigned pgds_size;

    /**
     * The array of PGD pointers. It also contains the free list of available
     * places. If an entry has bit 0 set, it is a PGD entry. Otherwise it is
     * the index of the next free entry multiplied by 2.
     */
    struct multimmap_pgd_info* pgds;

    /**
     * The index of the first free PGD. If INVALID_PGD_INDEX, there are not free entries.
     */
    unsigned first_free_pgd;

    /**
     * Indicate if we had a request into the area that could not be fulfilled
     * because no area is mapped in. If such a thing occurs  before an area has
     * ever been selected, we need to do an extra decrement on nr_ptes when
     * closing the VMA, otherwise we crash.
     */
    int had_failed_statics_request;

    /**
     * Indicate if we have ever selected a process memory area.
     */
    int selected_process_memory;

    /**
     * The number of kernel heap mappings.
     */
    unsigned kernel_heap_maps_count;

    /* /\** */
    /*  * The virtual address of the kernel heap. It should be mapped to the same */
    /*  * address in all processes. */
    /*  *\/ */
    /* unsigned long kernel_heap_address; */

    /**
     * The size of the kernel heap. It is nonzero if the kernel has been mapped
     * at least once. All other mappings must be of the same size.
     */
    unsigned long kernel_heap_size;

    /**
     * The addresses of kernel heap pages.
     */
    struct page** kernel_heap_pages;

    /**
     * The number of kernel statics mappings
     */
    unsigned kernel_statics_maps_count;

    /**
     * The virtual address of the kernel statics. It should be mapped to the
     * same address in all processes.
     */
    unsigned long kernel_statics_address;

    /**
     * The size of the kernel statics. It is nonzero if the kernel statics have
     * been mapped at least once. All other mappings must be of the same size.
     */
    unsigned long kernel_statics_size;

    /**
     * The addresses of kernel statics pages.
     */
    struct page** kernel_statics_pages;

    /**
     * The size of the array containing entries to unmap.
     */
    unsigned unmap_entries_size;

    /**
     * The number of entries to unmap.
     */
    unsigned unmap_entries_count;

    /**
     * The array of entries to unmap.
     */
    struct multimmap_unmap_info* unmap_entries;
};

/**
 * Base structure for the private data of VMAs.
 */
struct multimmap_vma_private {
    /**
     * The device the mapping belongs to.
     */
    struct multimmap_dev* dev;
};

/**
 * Information about a kernel heap or statics mapping in a process.
 */
struct multimmap_kernel_mapping_info {
    /**
     * The base.
     */
    struct multimmap_vma_private base;

    /**
     * Semaphore for the kernel mapping info
     */
    struct semaphore sema;

    /**
     * Indicate if the kernel mapping is R/W
     */
    int is_rw;

    /**
     * The R/W page directory entry for the mapping. If it is the kernel heap,
     * this is a PGD entry, otherwise it is a PMD entry
     */
    unsigned long rw_entry;

    /**
     * The R/O page directory entry for the mapping.
     */
    unsigned long ro_entry;
};

/**
 * Information about the memory shared between all processes running on the
 * same CPU (even if on different cores) and using multimmap.
 */
struct multimmap_shared_memory_info {
    /**
     * Spinlock to protect common stuff that may be accessed from several
     * processes. If used in combination with mmap_sem, mmap_sem should be 
     * locked first!
     */
    struct semaphore sema;

    /**
     * The number of the mappings of the shared memory.
     */
    unsigned maps_count;

    /**
     * The virtual address of the shared memory. It should be mapped to the same
     * address in all processes.
     */
    unsigned long address;

    /**
     * The size of the shared memory. It is nonzero if the shared memory been mapped
     * at least once. All other mappings must be of the same size.
     */
    unsigned long size;

    /**
     * The addresses of shared memory pages.
     */
    struct page** pages;    
};

/**
 * Information about a shared memory mapping in a process.
 */
struct multimmap_shared_memory_mapping_info {
    /**
     * The base.
     */
    struct multimmap_vma_private base;

    /**
     * Semaphore for the shared memory mapping info
     */
    struct semaphore sema;

    /**
     * Indicate if the shared memory mapping is R/W
     */
    int is_rw;

    /**
     * The R/W page directory entry for the mapping. It is a PUD entry.
     */
    unsigned long rw_entry;

    /**
     * The R/O page directory entry for the mapping.
     */
    unsigned long ro_entry;
};

static struct multimmap_dev* devs = 0;
static struct file_operations fops;
static struct vm_operations_struct multimmap_vm_ops;
static struct multimmap_shared_memory_info shared_memory;

static inline int multimmap_is_pgd_valid(struct multimmap_pgd_info* pgd_info)
{
    return pgd_info->heap!=INVALID_PGD_INDEX;
}

static inline int multimmap_is_pgd_mapped(struct multimmap_pgd_info* pgd_info)
{
    return multimmap_is_pgd_valid(pgd_info) && 
        (pgd_info->heap!=0 || pgd_info->u.statics!=0);
}

static unsigned long multimmap_find_area(unsigned long address)
{
    address += PGDIR_SIZE - 1;
    address &= ~(PGDIR_SIZE-1);
    for(; address<16*PGDIR_SIZE; address += PGDIR_SIZE) {
        unsigned long x = get_unmapped_area(0, address, PGDIR_SIZE, 0, 0);
        if (x==address) return address;
    }

    return (unsigned long)-1;
}

static void multimmap_dec_nr_ptes(void* arg)
{
    struct mm_struct* mm = (struct mm_struct*)arg;
    --mm->nr_ptes;
}

static void multimmap_inc_nr_ptes(void* arg)
{
    struct mm_struct* mm = (struct mm_struct*)arg;
    ++mm->nr_ptes;
}

static void multimmap_walk_pmd(pmd_t* pmd, multimmap_pmd_fn_t pmd_fn, void* arg)
{
/*     printk("multimmap_walk_pmd: %lx\n", pmd_val(*pmd)); */

    if (pmd_val(*pmd)!=0) {
        pmd_fn(arg);
    }
}

static void multimmap_walk_pud(pud_t* pud, multimmap_pmd_fn_t pmd_fn, void* arg)
{
    unsigned i;
    unsigned long address = 0;

/*     printk("multimmap_walk_pud: %lx\n", pud_val(*pud));  */

    if (pud_val(*pud)==0) return;

    for(i=0; i<PTRS_PER_PMD; address += PMD_SIZE, ++i) {
        multimmap_walk_pmd(pmd_offset(pud, address), pmd_fn, arg);
    }
}

static void multimmap_walk_pgd(pgd_t* pgd, multimmap_pmd_fn_t pmd_fn, void* arg)
{
    unsigned i;
    unsigned long address = 0;

/*     printk("multimmap_walk_pgd: %lx\n", pgd_val(*pgd)); */

    for(i=0; i<PTRS_PER_PUD; address += PUD_SIZE, ++i) {
        multimmap_walk_pud(pud_offset(pgd, address), pmd_fn, arg);
    }
}

static inline void multimmap_free_pgd(struct multimmap_dev* dev, unsigned index)
{
    dev->pgds[index].heap = INVALID_PGD_INDEX;
    dev->pgds[index].u.next_free_pgd = dev->first_free_pgd;
    dev->first_free_pgd = index;

    if (index==dev->current_pgd) dev->current_pgd = INVALID_PGD_INDEX;
}

static void print_vmas(struct mm_struct* mm)
{
    struct vm_area_struct* vma = mm->mmap;
    printk("VMAs from %p:\n", mm);
    while(vma!=0) {
        printk("  %016lx - %016lx\n", vma->vm_start, vma->vm_end);
        vma = vma->vm_next;
    }
}

static unsigned long multimmap_alloc_paging_table(void)
{
    struct page* page = alloc_page(GFP_KERNEL|__GFP_ZERO);
    if (page==0) return (unsigned long)-1;
    else return page_to_pfn(page) << PAGE_SHIFT;
}

static pgd_t* multimmap_ensure_pgd(struct mm_struct* mm,
                                   unsigned long address)
{
    pgd_t* pgd = pgd_offset(mm, address);
    if (pgd_val(*pgd)==0) {
        unsigned long address = multimmap_alloc_paging_table();
        *pgd = __pgd(address | _PAGE_TABLE);
    }

    return pgd;
}

static pud_t* multimmap_ensure_pud(struct mm_struct* mm,
                                   unsigned long address)
{
    pgd_t* pgd = multimmap_ensure_pgd(mm, address);

    pud_t* pud = pud_offset(pgd, address);
    if (pud_val(*pud)==0) {
        unsigned long address = multimmap_alloc_paging_table();
        *pud = __pud(address | _PAGE_TABLE);
    }    

    return pud;
}

static void multimmap_unmap_pud(struct multimmap_dev* dev,
                                struct mm_struct* mm, pud_t pud,
                                unsigned long map_address, int is_current)
{
    long address;
    pud_t* entry = 0;

    if (!is_current) {
        multimmap_walk_pud(&pud, multimmap_inc_nr_ptes, mm);
    }

    down_write(&mm->mmap_sem);
        
    /* We map read-only to avoid trouble with internal checks */
    address = do_mmap_pgoff(0, map_address, PUD_SIZE, PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, 0);
    if (address!=map_address) {
        printk("multimmap_unmap_pmd: area mapped at %lx (%ld) instead of %lx - %lx, map_count: %d\n", 
               address, address, map_address, map_address + PUD_SIZE, mm->map_count);
        print_vmas(mm);
        multimmap_walk_pud(&pud, multimmap_dec_nr_ptes, mm);
    } else {
        pgd_t* pgd = multimmap_ensure_pgd(mm, map_address);
        entry = pud_offset(pgd, map_address);
/*             printk("multimmap_unmap_area: index=%u, mapping done, nr_ptes: %lu\n", index, mm->nr_ptes); */
        *entry = pud;
/*             printk("multimmap_unmap_area: index=%u, entry set up, nr_ptes: %lu\n", index, mm->nr_ptes); */
    }
    
    if (address>0) {
        do_munmap(mm, address, PUD_SIZE);        
/*             printk("multimmap_unmap_area: index=%u, unmapping done, nr_ptes: %lu\n", index, mm->nr_ptes); */
    }

    if (entry!=0) {
        *entry = __pud(0);
    }

    up_write(&mm->mmap_sem);
    
}

static void multimmap_unmap_pmd(struct multimmap_dev* dev,
                                struct mm_struct* mm, pmd_t pmd,
                                unsigned long map_address, int is_current)
{
    long address;
    pmd_t* entry = 0;

    if (!is_current) {
        multimmap_walk_pmd(&pmd, multimmap_inc_nr_ptes, mm);
    }

    down_write(&mm->mmap_sem);
        
    /* We map read-only to avoid trouble with internal checks */
    address = do_mmap_pgoff(0, map_address, PMD_SIZE, PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, 0);
    if (address!=map_address) {
        printk("multimmap_unmap_pmd: area mapped at %lx (%ld) instead of %lx - %lx, map_count: %d\n", 
               address, address, map_address, map_address + PMD_SIZE, mm->map_count);
        print_vmas(mm);
        multimmap_walk_pmd(&pmd, multimmap_dec_nr_ptes, mm);
    } else {
        pud_t* pud = multimmap_ensure_pud(mm, map_address);
        entry = pmd_offset(pud, map_address);
/*             printk("multimmap_unmap_area: index=%u, mapping done, nr_ptes: %lu\n", index, mm->nr_ptes); */
        *entry = pmd;
/*             printk("multimmap_unmap_area: index=%u, entry set up, nr_ptes: %lu\n", index, mm->nr_ptes); */
    }
    
    if (address>0) {
        do_munmap(mm, address, PMD_SIZE);        
/*             printk("multimmap_unmap_area: index=%u, unmapping done, nr_ptes: %lu\n", index, mm->nr_ptes); */
    }

    if (entry!=0) {
        *entry = __pmd(0);
    }

    up_write(&mm->mmap_sem);
    
}

static struct page* multimmap_find_page(struct mm_struct* mm, unsigned long address)
{
    pte_t* pte = 0;
    pmd_t* pmd = 0;
    pud_t* pud = 0;
    pgd_t* pgd = pgd_offset(mm, address);
    if (pgd_val(*pgd)==0) {
        return 0;
    }

    pud = pud_offset(pgd, address);
    if (pud_val(*pud)==0) {
        return 0;
    }    
    
    pmd = pmd_offset(pud, address);
    if (pmd_val(*pmd)==0) {
        return 0;
    }    

    pte = pte_offset_kernel(pmd, address);
    if (pte_val(*pte)==0) {
        return 0;
    } else {
        return pte_page(*pte);
    }
}

static void multimmap_unmap_pgd(struct mm_struct* mm, pgd_t pgd,
                                unsigned long map_address, int is_current)
{
    long address;
    pgd_t* entry = pgd_offset(mm, map_address);

    if (!is_current) {
        multimmap_walk_pgd(&pgd, multimmap_inc_nr_ptes, mm);
    }

/*     printk("multimmap_unmap_pgd: pgd=%lx\n", pgd_val(pgd)); */

    down_write(&mm->mmap_sem);

/*     printk("multimmap_unmap_pgd: initially nr_ptes: %lu\n", mm->nr_ptes); */
        
    /* We map read-only to avoid trouble with internal checks */
    address = do_mmap_pgoff(0, map_address, PGDIR_SIZE, PROT_READ, MAP_PRIVATE|MAP_ANONYMOUS, 0);
    if (address!=map_address) {
        printk("multimmap_unmap_pgd: area mapped at %lx (%ld) instead of %lx - %lx, map_count: %d\n", 
               address, address, map_address, map_address + PGDIR_SIZE, mm->map_count);
        print_vmas(mm);
        multimmap_walk_pgd(&pgd, multimmap_dec_nr_ptes, mm);
    } else {
/*             printk("multimmap_unmap_area: index=%u, mapping done, nr_ptes: %lu\n", index, mm->nr_ptes); */
        *entry = pgd;
/*             printk("multimmap_unmap_area: index=%u, entry set up, nr_ptes: %lu\n", index, mm->nr_ptes); */
    }
    
    if (address>0) {
/*         printk("multimmap_unmap_pgd: unmapping is about to be done, nr_ptes: %lu\n", mm->nr_ptes); */
        do_munmap(mm, address, PGDIR_SIZE);        
/*         printk("multimmap_unmap_pgd: unmapping done, nr_ptes: %lu\n", mm->nr_ptes); */
    }

    if (address==map_address) {
        *entry = __pgd(0);
    }

    up_write(&mm->mmap_sem);        
}

static void multimmap_add_unmap_entry(struct multimmap_dev* dev, 
                                      unmap_type type, unsigned long entry)
{
    down(&dev->sema);
    if (dev->unmap_entries==0) {
        dev->unmap_entries = 
            (struct multimmap_unmap_info*)vmalloc(sizeof(struct multimmap_unmap_info)*
                                                  INITIAL_UNMAP_ENTRIES_SIZE);
        dev->unmap_entries_size = INITIAL_UNMAP_ENTRIES_SIZE;
        dev->unmap_entries_count = 0;
    } else if (dev->unmap_entries_count>=dev->unmap_entries_size) {
        struct multimmap_unmap_info* new_unmap_entries = 
            (struct multimmap_unmap_info*)vmalloc(sizeof(struct multimmap_unmap_info)*
                                                  dev->unmap_entries_size*2);
        memcpy(new_unmap_entries, dev->unmap_entries, 
               sizeof(struct multimmap_unmap_info)*dev->unmap_entries_size);
        vfree(dev->unmap_entries);
        dev->unmap_entries = new_unmap_entries;
        dev->unmap_entries_size *= 2;
    }

    dev->unmap_entries[dev->unmap_entries_count].type = type;
    dev->unmap_entries[dev->unmap_entries_count].entry = entry;
    ++dev->unmap_entries_count;

    up(&dev->sema);
}

static void multimmap_add_unmap_pgd(struct multimmap_dev* dev, 
                                    unsigned long pgd)
{
    multimmap_add_unmap_entry(dev, UNMAP_PGD, pgd);
}

static void multimmap_add_unmap_pud(struct multimmap_dev* dev, 
                                    unsigned long pud)
{
    multimmap_add_unmap_entry(dev, UNMAP_PUD, pud);
}

static void multimmap_add_unmap_pmd(struct multimmap_dev* dev, 
                                    unsigned long pmd)
{
    multimmap_add_unmap_entry(dev, UNMAP_PMD, pmd);
}

// Should be called with dev->sema held (unless loading or unloading module).
static void multimmap_unmap_entries(struct multimmap_dev* dev,
                                    struct mm_struct* mm,
                                    unsigned long map_address)
{
    unsigned i = 0;
 
    for(; i<dev->unmap_entries_count; ++i) {
        switch (dev->unmap_entries[i].type) {
          case UNMAP_PGD:
            multimmap_unmap_pgd(mm, __pgd(dev->unmap_entries[i].entry), map_address, 0);
            break;
          case UNMAP_PUD:
            multimmap_unmap_pud(dev, mm, __pud(dev->unmap_entries[i].entry), map_address, 0);
            break;
          case UNMAP_PMD:
            multimmap_unmap_pmd(dev, mm, __pmd(dev->unmap_entries[i].entry), map_address, 0);
            break;
        }
    }
    dev->unmap_entries_count = 0;
}

static struct mm_struct* multimmap_get_mm(struct multimmap_dev* dev)
{
    struct mm_struct* mm = current->mm;

    if (mm==0) {
        BUG_ON(dev->vma==0);
        mm = dev->vma->vm_mm;
    }

    return mm;
}

static void multimmap_unmap_area(struct multimmap_dev* dev, unsigned index, unsigned long map_address,
                                 int is_current)
{
    struct mm_struct* mm = multimmap_get_mm(dev);

    if (index>=dev->pgds_size) return;

/*     printk("multimmap_unmap_area0: index=%u, current_pgd=%u, nr_ptes: %lu\n", */
/*            index, dev->current_pgd, mm->nr_ptes); */
    
    if (multimmap_is_pgd_mapped(dev->pgds + index)) {
/*         printk("multimmap_unmap_area: index=%u\n", index); */
        if (dev->pgds[index].heap!=0) {
/*             printk("multimmap_unmap_pgd: index=%u\n", index); */
            multimmap_unmap_pgd(mm, __pgd(dev->pgds[index].heap), map_address, 
                                is_current);
        }
        if (dev->pgds[index].u.statics!=0) {
/*             printk("multimmap_unmap_pmd: index=%u\n", index); */
            multimmap_unmap_pud(dev, mm, __pud(dev->pgds[index].u.statics), 
                                map_address, is_current);
        }
    }
    
    multimmap_free_pgd(dev, index);

/*     printk("multimmap_unmap_area1: nr_ptes: %lu\n", mm->nr_ptes); */
}

static int multimmap_unmap_areas(struct multimmap_dev* dev)
{
    struct mm_struct* mm = multimmap_get_mm(dev);

    unsigned i;

    unsigned long map_address = multimmap_find_area(2*MULTIMMAP_HEAP_SIZE);    

/*     printk("multimmap_unmap_areas, map_address: %lx\n", map_address); */

    if (map_address==(unsigned long)-1) return 0;
    
    for(i=0; i<dev->pgds_size; ++i) {
/*         printk("multimmap_unmap_areas: i=%u (current=%u), pgds[i]=%lx\n",  */
/*                i, dev->current_pgd, dev->pgds[i]); */
    
        if (multimmap_is_pgd_valid(dev->pgds+i)) {
            multimmap_unmap_area(dev, i, map_address, 0);
        }
    }

    vfree(dev->pgds); dev->pgds = 0;    
    dev->pgds_size = 0;
    dev->first_free_pgd = INVALID_PGD_INDEX;
    dev->current_pgd = INVALID_PGD_INDEX;

    multimmap_unmap_entries(dev, mm, map_address);

/*     printk("multimmap_unmap_areas done\n"); */

    return 1;
}

static __init int multimmap_setup_dev(struct multimmap_dev* dev, unsigned index)
{
    cdev_init(&dev->cdev, &fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &fops;

    sema_init(&dev->sema, 1);

    dev->users = 0;
    dev->vma = 0;
    dev->statics = 0;
    dev->pgds = 0;
    dev->pgds_size = 0;
    dev->first_free_pgd = INVALID_PGD_INDEX;
    dev->current_pgd = INVALID_PGD_INDEX;
    dev->had_failed_statics_request = 0;
    dev->selected_process_memory = 0;
    dev->kernel_heap_maps_count = 0;
    //dev->kernel_heap_address = 0;
    dev->kernel_heap_size = 0;
    dev->kernel_heap_pages = 0;
    dev->kernel_statics_maps_count = 0;
    dev->kernel_statics_address = 0;
    dev->kernel_statics_size = 0;
    dev->kernel_statics_pages = 0;
    dev->unmap_entries_size = 0;
    dev->unmap_entries_count = 0;
    dev->unmap_entries = 0;

    return cdev_add(&dev->cdev, MKDEV(major, minor+index), 1);
}

static __init int multimmap_setup_devs(void)
{
    unsigned i;
    int err = 0;

    devs = (struct multimmap_dev*)vmalloc(sizeof(struct multimmap_dev)*numdevs);
    if (devs==0) {
        return -ENOMEM;
    }

    memset(devs, 0, sizeof(struct multimmap_dev)*numdevs);

    for(i=0; i<numdevs && !err; ++i) {
        err = multimmap_setup_dev(devs+i, i);
    }

    return err;
}

static void multimmap_clear_devs(void)
{
    unsigned i;
    if (devs==0) return;

    for(i=0; i<numdevs; ++i) {
        struct multimmap_dev* dev = devs + i;
        if (dev->cdev.owner==THIS_MODULE) {
            multimmap_unmap_areas(dev);
            cdev_del(&dev->cdev);
            if (dev->unmap_entries!=0) vfree(dev->unmap_entries);
        }
    }

    vfree(devs);
}

static __init int multimmap_init(void)
{
    int err = 0;
#if IS_LICENSE_GPL
    unsigned i;
#endif

    if (major>0) {
        err = register_chrdev_region(MKDEV(major, minor), numdevs, DEVICE_NAME);
    } else {
        dev_t dev;
        err = alloc_chrdev_region(&dev, minor, numdevs, DEVICE_NAME);
        major = MAJOR(dev);
    }

    if (err) goto fail;

    err = multimmap_setup_devs();
    if (err) goto setup_devs_fail;

    sema_init(&shared_memory.sema, 1);
    shared_memory.maps_count = 0;
    shared_memory.address = 0;
    shared_memory.size = 0;
    shared_memory.pages = 0;

#if IS_LICENSE_GPL
    multimmap_class = class_create(THIS_MODULE, DEVICE_NAME);
    for(i=0; i<numdevs; ++i) {
        device_create(multimmap_class, NULL, MKDEV(major, i), NULL, DEVICE_NAME"%u", i);
    }
#endif

    return 0;
    
setup_devs_fail:
    multimmap_clear_devs();
    unregister_chrdev_region(MKDEV(major, minor), numdevs);

fail:
    return err;
}

static int multimmap_open(struct inode* inode, struct file* filp)
{
    struct multimmap_dev* dev = container_of(inode->i_cdev, struct multimmap_dev, cdev);

/*     printk("multimmap_open: users=%u\n", dev->users); */

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->users==0) {
        if (!multimmap_unmap_areas(dev)) {
            up(&dev->sema);
            return -ENOMEM;
        }
        dev->had_failed_statics_request = 0;
        dev->selected_process_memory = 0;
    }

    ++dev->users;
    
    up(&dev->sema);
    
    filp->private_data = dev;
    
    return 0;
}

static void multimmap_free_statics(struct multimmap_dev* dev)
{
/*     printk("multimmap_free_statics\n"); */
    unsigned long i = 0;
    for(; i<dev->statics->num_pages; ++i) {
        if (dev->statics->shadow[i]!=0) {
            put_page(dev->statics->shadow[i]);
        }
    }
    vfree(dev->statics->shadow);
    vfree(dev->statics);
    dev->statics = 0;
}

static void multimmap_free_kernel_statics(struct multimmap_dev* dev)
{
    unsigned long i, num_pages = dev->kernel_statics_size/PAGE_SIZE;
    
    for(i=0; i<num_pages; ++i) {
        struct page* page = dev->kernel_statics_pages[i];
        put_page(page);
    }

    vfree(dev->kernel_statics_pages);
    dev->kernel_statics_pages = 0;

    dev->kernel_statics_address = 0;
    dev->kernel_statics_size = 0;
}

static void multimmap_release_kernel_statics(struct multimmap_dev* dev)
{
    if (dev->users==0 && dev->kernel_statics_pages!=0 && dev->kernel_statics_maps_count==0) {        
        multimmap_free_kernel_statics(dev);
    }
}

static int multimmap_init_statics(struct multimmap_dev* dev,
                                  struct file* filp,
                                  unsigned long arg)
{
    int err = 0;
    unsigned long address, size, i = 0;
    struct multimmap_statics_info statics_info;

/*         printk("MULTIMMAP_INIT_STATICS\n"); */

    err = copy_from_user(&statics_info, (void __user*)arg,
                         sizeof(struct multimmap_statics_info));
    if (err!=0) return err;
    
    if ((statics_info.start&PUD_MASK)!=statics_info.start) {
        return -EINVAL;
    }

    statics_info.size = (statics_info.size + PAGE_SIZE - 1) & (~(PAGE_SIZE-1UL));
    
    if (statics_info.size>MULTIMMAP_STATICS_SIZE) {
        return -EINVAL;
    }
    
    if (!access_ok(VERIFY_READ, (void*)statics_info.start, statics_info.size)) {
        return -EFAULT;
    }

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->statics!=0) {
        up(&dev->sema);
        return -ENODEV;
    }
    
    dev->statics = (struct multimmap_statics*)vmalloc(sizeof(struct multimmap_statics));
    if (dev->statics==0) {
        up(&dev->sema);
        return -ENOMEM;
    }
    
    dev->statics->vma = 0;
    dev->statics->num_pages = statics_info.size / PAGE_SIZE;
    dev->statics->shadow = (struct page**)vmalloc(sizeof(struct page*) * dev->statics->num_pages);
    if (dev->statics->shadow==0) {
        vfree(dev->statics);
        dev->statics = 0;
        up(&dev->sema);
        return -ENOMEM;
    }
    dev->statics->shadow_pud = 0;
    dev->statics->shadow_mapped = 0;

    up(&dev->sema);


    address = statics_info.start;
    for(i=0; i<dev->statics->num_pages; ++i, address+=PAGE_SIZE) {
        struct page* page = multimmap_find_page(current->mm, address);
        if (page!=0) {
            get_page(page);
        }
        dev->statics->shadow[i] = page;
    }

    down_write(&current->mm->mmap_sem);

    err = do_munmap(current->mm, statics_info.start, MULTIMMAP_STATICS_SIZE);
    if (err!=0) {
        printk("multimmap_ioctl[MULTIMMAP_INIT_STATICS]: do_munmap failed with %d\n",
               err);
        down(&dev->sema);
        multimmap_free_statics(dev);
        up(&dev->sema);
        up_write(&current->mm->mmap_sem);
        return err;
    }

    size = MULTIMMAP_STATICS_SIZE;
    do {
        // FIXME: MAP_SHARED is needed here so that the shadow pages are not
        // copied when the shadow area is mapped and someone wants to write
        // into it. However, a better solution may be possible, because this
        // really is a private mapping.
        address = do_mmap_pgoff(filp, statics_info.start, 
                                size,
                                PROT_READ|PROT_WRITE,
                                MAP_SHARED, STATICS_PGOFF);
        size /= 2;
    } while(address!=statics_info.start && size>=statics_info.size);

    if (size!=(MULTIMMAP_STATICS_SIZE/2)) {
        printk("multimmap_ioctl[MULTIMMAP_INIT_STATICS]: mapped %lx bytes instead of %lx\n",
               size*2, MULTIMMAP_STATICS_SIZE);
    }

    if (address!=statics_info.start) {
        printk("multimmap_ioctl[MULTIMMAP_INIT_STATICS]: do_mmap_pgoff used address %ld (%lx) instead of %lx\n",
               address, address, statics_info.start);
        down(&dev->sema);
        multimmap_free_statics(dev);
        up(&dev->sema);
        up_write(&current->mm->mmap_sem);
        return err;
    }

    up_write(&current->mm->mmap_sem);

    return 0;
}

static int multimmap_create(struct multimmap_dev* dev) 
{
    unsigned index;

/*         printk("MULTIMMAP_CREATE: dev: %p, vma: %p\n", dev, dev->vma);  */

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->vma==0) {
        up(&dev->sema);
        return -ENOTTY;
    }

    if (dev->first_free_pgd==INVALID_PGD_INDEX) {            
        unsigned i;
        if (dev->pgds==0) {
            dev->pgds = 
                (struct multimmap_pgd_info*)vmalloc(sizeof(struct multimmap_pgd_info) * 
                                                    INITIAL_PGDS_SIZE);
            if (dev->pgds==0) {
                up(&dev->sema);
                return -ENOMEM;
            }
            for(i=0; i<(INITIAL_PGDS_SIZE-1); ++i) {
                dev->pgds[i].heap = INVALID_PGD_INDEX;
                dev->pgds[i].u.next_free_pgd = i+1;
            }
            dev->pgds_size = INITIAL_PGDS_SIZE;
            dev->first_free_pgd = 0;
        } else {
            struct multimmap_pgd_info* new_pgds = 
                (struct multimmap_pgd_info*)vmalloc(sizeof(struct multimmap_pgd_info) * 
                                                    dev->pgds_size * 2);
            if (new_pgds==0) {
                up(&dev->sema);
                return -ENOMEM;
            }
            memcpy(new_pgds, dev->pgds, sizeof(struct multimmap_pgd_info) * dev->pgds_size);
            for(i=dev->pgds_size; i<(dev->pgds_size * 2 - 1); ++i) {
                new_pgds[i].heap = INVALID_PGD_INDEX;
                new_pgds[i].u.next_free_pgd = i+1;
            }
            vfree(dev->pgds); dev->pgds = new_pgds; 
            dev->first_free_pgd = dev->pgds_size;
            dev->pgds_size *= 2;
        }

        dev->pgds[dev->pgds_size-1].heap = INVALID_PGD_INDEX;
        dev->pgds[dev->pgds_size-1].u.next_free_pgd = INVALID_PGD_INDEX;
    }

    index = dev->first_free_pgd;

    dev->first_free_pgd = dev->pgds[index].u.next_free_pgd;

    dev->pgds[index].heap = 0;
    dev->pgds[index].num_pages = 0;
    dev->pgds[index].u.statics = 0;

    up(&dev->sema);    

    return index;
}

static pud_t* multimmap_get_statics_pud(struct multimmap_dev* dev)
{
    unsigned long address = dev->statics->vma->vm_start;
    pgd_t* pgd = multimmap_ensure_pgd(dev->statics->vma->vm_mm, address);
    return pud_offset(pgd, address);
}

static int multimmap_select(struct multimmap_dev* dev, unsigned long arg) 
{
    pgd_t* entry = 0;
    pud_t* statics_entry = 0;

/*     printk("MULTIMMAP_SELECT, statics=%p\n", dev->statics); */

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->vma==0 || (dev->statics!=0 && dev->statics->shadow_mapped)) {
        up(&dev->sema);
        return -ENOTTY;
    }

    entry = pgd_offset(dev->vma->vm_mm, dev->vma->vm_start);

    if (arg>=dev->pgds_size || !multimmap_is_pgd_valid(dev->pgds + arg)) {
        up(&dev->sema);
        return -EINVAL;
    }

    if (dev->statics!=0 && dev->statics->vma!=0) {
        statics_entry = multimmap_get_statics_pud(dev);
    }
    if (dev->current_pgd!=INVALID_PGD_INDEX) {
        unsigned long val = pgd_val(*entry);
        dev->pgds[dev->current_pgd].heap = val;
        if (statics_entry!=0) {
            unsigned long val = pud_val(*statics_entry);
            dev->pgds[dev->current_pgd].u.statics = val;
        }
    }

    dev->current_pgd = (unsigned)arg;
    *entry = __pgd(dev->pgds[arg].heap);
    if (statics_entry!=0) {
        *statics_entry = __pud(dev->pgds[arg].u.statics);
    }

    dev->selected_process_memory = 1;

    up(&dev->sema);

    __flush_tlb();

    return 0;
}

static int multimmap_map_static_shadow(struct multimmap_dev* dev,
                                       unsigned long arg)
{
    pud_t* statics_entry = 0;

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->vma==0 || dev->statics==0 || dev->statics->vma==0) {
        up(&dev->sema);
        return -ENOTTY;
    }

    if (dev->statics->shadow_mapped==arg) {
        up(&dev->sema);
        return 0;
    }

    statics_entry = multimmap_get_statics_pud(dev);

    if (arg==0) {
        dev->statics->shadow_pud = pud_val(*statics_entry);
        if (dev->current_pgd!=INVALID_PGD_INDEX) {
            *statics_entry = __pud(dev->pgds[dev->current_pgd].u.statics);
        } else {
            *statics_entry = __pud(0);
        }
    } else {
        if (dev->current_pgd!=INVALID_PGD_INDEX) {
            unsigned long val = pud_val(*statics_entry);
            dev->pgds[dev->current_pgd].u.statics = val;
        }
        *statics_entry = __pud(dev->statics->shadow_pud);
    }

    dev->statics->shadow_mapped = !!arg;

    up(&dev->sema);

    __flush_tlb();

    return 0;
}

static int multimmap_get_heap_pages(struct multimmap_dev* dev,
                                    unsigned long arg)
{
    int num_pages = 0;

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->vma==0 || dev->statics==0 || dev->statics->vma==0) {
        up(&dev->sema);
        return -ENOTTY;
    }

    if (arg>=dev->pgds_size || !multimmap_is_pgd_valid(dev->pgds + arg)) {
        up(&dev->sema);
        return -EINVAL;
    }

    num_pages = (int)dev->pgds[arg].num_pages;

    up(&dev->sema);

    return num_pages;
}

static int multimmap_destroy(struct multimmap_dev* dev, unsigned long arg) 
{
    pgd_t* entry = 0;
    pud_t* statics_entry = 0;

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }

    if (dev->vma==0 || (dev->statics!=0 && dev->statics->shadow_mapped)) {
        up(&dev->sema);
        return -ENOTTY;
    }

    if (arg>=dev->pgds_size || !multimmap_is_pgd_valid(dev->pgds + arg)) {
        up(&dev->sema);
        return -EINVAL;
    }

    if (arg==dev->current_pgd) {        
        entry = pgd_offset(dev->vma->vm_mm, dev->vma->vm_start);
        dev->pgds[arg].heap = pgd_val(*entry);
        *entry = __pgd(0);
        if (dev->statics!=0 && dev->statics->vma!=0) {
            unsigned long address = dev->statics->vma->vm_start;
            pgd_t* pgd = multimmap_ensure_pgd(dev->statics->vma->vm_mm, address);
            
            statics_entry = pud_offset(pgd, address);
            dev->pgds[arg].u.statics = pud_val(*statics_entry);
            *statics_entry = __pud(0);
        }
    }
    
    multimmap_unmap_area(dev, arg, multimmap_find_area(dev->vma->vm_end), 1);

    up(&dev->sema);

    return 0;
}

static int multimmap_kernel_heap_rw(struct vm_area_struct* vma,
                                    unsigned long arg, int interruptible,
                                    int* was_rw)
    
{
    pgd_t* entry = 0;
    struct multimmap_kernel_mapping_info* kernel_mapping_info = 0;

    kernel_mapping_info = (struct multimmap_kernel_mapping_info*)vma->vm_private_data;

    if (interruptible) {
        if (down_killable(&kernel_mapping_info->sema)) {
            return -ERESTARTSYS;
        }
    } else {
        down(&kernel_mapping_info->sema);
    }

    if ((kernel_mapping_info->is_rw && arg) || (!kernel_mapping_info->is_rw && !arg)) {
        up(&kernel_mapping_info->sema);
        return 0;
    }

    entry = pgd_offset(vma->vm_mm, vma->vm_start);
    if (arg) {
        kernel_mapping_info->ro_entry = pgd_val(*entry);
        *entry = __pgd(kernel_mapping_info->rw_entry);
        vma->vm_flags |= VM_WRITE;
    } else {
        kernel_mapping_info->rw_entry = pgd_val(*entry);
        *entry = __pgd(kernel_mapping_info->ro_entry);
        vma->vm_flags &= ~VM_WRITE;
    }
    vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);

    *was_rw = kernel_mapping_info->is_rw;

    kernel_mapping_info->is_rw = !!arg;

    up(&kernel_mapping_info->sema);

    return 0;
}

static int multimmap_kernel_statics_rw(struct vm_area_struct* vma,
                                       unsigned long arg, int interruptible)
    
{
    pud_t* pud = 0;
    pmd_t* entry = 0;
    struct multimmap_kernel_mapping_info* kernel_mapping_info = 0;

    kernel_mapping_info = (struct multimmap_kernel_mapping_info*)vma->vm_private_data;

    if (interruptible) {
        if (down_killable(&kernel_mapping_info->sema)) {
            return -ERESTARTSYS;
        }
    } else {
        down(&kernel_mapping_info->sema);
    }

    if ((kernel_mapping_info->is_rw && arg) || (!kernel_mapping_info->is_rw && !arg)) {
        up(&kernel_mapping_info->sema);
        return 0;
    }

    pud = multimmap_ensure_pud(current->mm, vma->vm_start);

    entry = pmd_offset(pud, vma->vm_start);
    if (arg) {
        kernel_mapping_info->ro_entry = pmd_val(*entry);
        *entry = __pmd(kernel_mapping_info->rw_entry);
        vma->vm_flags |= VM_WRITE;
    } else {
        kernel_mapping_info->rw_entry = pmd_val(*entry);
        *entry = __pmd(kernel_mapping_info->ro_entry);
        vma->vm_flags &= ~VM_WRITE;
    }
    vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);

    kernel_mapping_info->is_rw = !!arg;

    up(&kernel_mapping_info->sema);

    return 0;
}

static int multimmap_kernel_memory_rw(struct multimmap_dev* dev,
                                      unsigned long arg1)
{
    struct vm_area_struct* heap_vma = 0;
    struct vm_area_struct* statics_vma = 0;
    struct multimmap_kernel_memory_info memory_info;
    int is_rw = 0;
    int error = 0;
    int kernel_was_rw = 0;
    unsigned orig_kel = 0;

    if (dev->kernel_heap_maps_count==0 && dev->kernel_statics_maps_count==0) {
        return -ENODEV;
    }

    error = copy_from_user(&memory_info, (void __user*)arg1,
                           sizeof(struct multimmap_kernel_memory_info));
    if (error!=0) return -error;
    
    heap_vma = find_vma(current->mm, memory_info.heap_address);
    if (heap_vma!=0 && 
        (heap_vma->vm_pgoff!=KERNEL_HEAP_PGOFF ||
         ((struct multimmap_vma_private*)heap_vma->vm_private_data)->dev!=dev))
    {
        heap_vma = 0;
    }
    
    statics_vma = find_vma(current->mm, memory_info.statics_address);
    if (statics_vma!=0 && 
        (statics_vma->vm_pgoff!=KERNEL_STATICS_PGOFF ||
         ((struct multimmap_vma_private*)statics_vma->vm_private_data)->dev!=dev))
    {
        statics_vma = 0;
    }
    
    if (heap_vma==0 && statics_vma==0) {
        return -ENODEV;
    }

/*         printk("MULTIMMAP_INIT_STATICS\n"); */

    is_rw = memory_info.desired_kel>0;
    orig_kel = *memory_info.kel_address;

    if (!is_rw) {
        *memory_info.kel_address = memory_info.desired_kel;
    }

    if (heap_vma!=0 && dev->kernel_heap_maps_count>0) {
        error = multimmap_kernel_heap_rw(heap_vma, is_rw, 1, &kernel_was_rw);
    }

    if (error==0 && statics_vma!=0 && dev->kernel_statics_maps_count>0) {
        error = multimmap_kernel_statics_rw(statics_vma, is_rw, 1);
        if (error!=0 && heap_vma!=0) {
            multimmap_kernel_heap_rw(heap_vma, kernel_was_rw, 0, &kernel_was_rw);
        }
    }

    if (error==0 && is_rw) {
        *memory_info.kel_address = memory_info.desired_kel;
    }

    __flush_tlb();

    return error;
}

static int multimmap_init_kernel_statics(struct multimmap_dev* dev,
                                         struct file* filp,
                                         unsigned long arg)
{
    int err = 0;
    unsigned long address, num_pages, i;
    struct multimmap_statics_info statics_info;

/*         printk("MULTIMMAP_INIT_KERNEL_STATICS\n"); */

    err = copy_from_user(&statics_info, (void __user*)arg,
                         sizeof(struct multimmap_statics_info));
    if (err!=0) return err;
    
    if ((statics_info.start&PMD_MASK)!=statics_info.start) {
        return -EINVAL;
    }

    statics_info.size = (statics_info.size + PAGE_SIZE - 1) & (~(PAGE_SIZE-1UL));
    
    if (statics_info.size>MULTIMMAP_KERNEL_STATICS_SIZE) {
        return -EINVAL;
    }
    
    if (!access_ok(VERIFY_READ, (void*)statics_info.start, statics_info.size)) {
        return -EFAULT;
    }

    if (down_killable(&dev->sema)!=0) {
        return -ERESTARTSYS;
    }
    
    if (dev->kernel_statics_pages!=0) {
        up(&dev->sema);
        return -ENODEV;
    }

    num_pages = statics_info.size/PAGE_SIZE;
    
    dev->kernel_statics_pages = (struct page**)vmalloc(sizeof(struct page*)*num_pages);
    if (dev->kernel_statics_pages==0) {
        up(&dev->sema);
        return -ENOMEM;
    }
    
    dev->kernel_statics_address = statics_info.start;
    dev->kernel_statics_size = statics_info.size;    

    for(i=0; i<num_pages; ++i) {
        struct page* page = alloc_page(GFP_USER);
        memcpy(page_address(page),
               (void*)(dev->kernel_statics_address + i*PAGE_SIZE),
               PAGE_SIZE);
        dev->kernel_statics_pages[i] = page;
    }

    up(&dev->sema);

    down_write(&current->mm->mmap_sem);

    err = do_munmap(current->mm, statics_info.start, statics_info.size);
    if (err!=0) {
        printk("multimmap_ioctl[MULTIMMAP_INIT_KERNEL_STATICS]: do_munmap failed with %d\n",
               err);
        down(&dev->sema);
        multimmap_free_kernel_statics(dev);
        up(&dev->sema);
        up_write(&current->mm->mmap_sem);
        return err;
    }

    address = do_mmap_pgoff(filp, statics_info.start, 
                            statics_info.size,
                            PROT_READ|PROT_WRITE,
                            MAP_SHARED, KERNEL_STATICS_PGOFF);

    if (address!=statics_info.start) {
        printk("multimmap_ioctl[MULTIMMAP_INIT_KERNEL_STATICS]: do_mmap_pgoff used address %ld (%lx) instead of %lx (statics are now lost!!!)\n",
               address, address, statics_info.start);
        down(&dev->sema);
        multimmap_free_kernel_statics(dev);
        up(&dev->sema);
        up_write(&current->mm->mmap_sem);
        return err;
    }
    
    up_write(&current->mm->mmap_sem);

    return 0;
}

static int multimmap_shared_memory_rw(unsigned long arg)    
{
    struct vm_area_struct* shared_memory_vma = 0;
    pgd_t* pgd = 0;
    pud_t* entry = 0;
    struct multimmap_shared_memory_mapping_info* shared_memory_mapping_info = 0;

    if (shared_memory.maps_count==0) {
        return -ENODEV;
    }

    shared_memory_vma = find_vma(current->mm, shared_memory.address);
    if (shared_memory_vma==0) {
        return -ENODEV;
    }
/*         printk("MULTIMMAP_INIT_STATICS\n"); */

    shared_memory_mapping_info = 
        (struct multimmap_shared_memory_mapping_info*)shared_memory_vma->vm_private_data;

    if (down_killable(&shared_memory_mapping_info->sema)) {
        return -ERESTARTSYS;
    }
    if ((shared_memory_mapping_info->is_rw && arg) || (!shared_memory_mapping_info->is_rw && !arg)) {
        up(&shared_memory_mapping_info->sema);
        return 0;
    }

    pgd = multimmap_ensure_pgd(current->mm, shared_memory_vma->vm_start);

#if 0    
    printk("vma: %p, arg=%lu, ro_entry: 0x%lx, rw_entry: 0x%lx\n",
           shared_memory_vma, arg, shared_memory_mapping_info->ro_entry,
           shared_memory_mapping_info->rw_entry);
#endif
    entry = pud_offset(pgd, shared_memory_vma->vm_start);
    if (arg) {
        shared_memory_mapping_info->ro_entry = pud_val(*entry);
        *entry = __pud(shared_memory_mapping_info->rw_entry);
        shared_memory_vma->vm_flags |= VM_WRITE;
    } else {
        shared_memory_mapping_info->rw_entry = pud_val(*entry);
        *entry = __pud(shared_memory_mapping_info->ro_entry);
        shared_memory_vma->vm_flags &= ~VM_WRITE;
    }
#if 0
    printk("vma1: %p, arg=%lu, ro_entry: 0x%lx, rw_entry: 0x%lx, entry: 0x%lx @%p (next: 0x%lx @ %p)\n",
           shared_memory_vma, arg, shared_memory_mapping_info->ro_entry,
           shared_memory_mapping_info->rw_entry,
           *pud_offset(pgd, shared_memory_vma->vm_start),
           pud_offset(pgd, shared_memory_vma->vm_start),
           *pud_offset(pgd, shared_memory_vma->vm_start + 2*1024*1024),
           pud_offset(pgd, shared_memory_vma->vm_start + 2*1024*1024));
#endif
    shared_memory_vma->vm_page_prot = vm_get_page_prot(shared_memory_vma->vm_flags);

    shared_memory_mapping_info->is_rw = !!arg;

    up(&shared_memory_mapping_info->sema);

    __flush_tlb();

    return 0;
}

static int multimmap_ioctl(struct inode* inode, struct file* filp,
                           unsigned int cmd, unsigned long arg)
{
    struct multimmap_dev* dev = (struct multimmap_dev*)filp->private_data;

/*     printk("multimmap_ioctl: cmd=%08x\n", cmd); */

    if (_IOC_TYPE(cmd) != MULTIMMAP_IOC_MAGIC) return -ENOTTY;
    if (_IOC_NR(cmd) > MULTIMMAP_IOC_MAXNR) return -ENOTTY;

    switch(cmd) {
      case MULTIMMAP_INIT_STATICS:
        return multimmap_init_statics(dev, filp, arg);
      case MULTIMMAP_CREATE:
        return multimmap_create(dev);
      case MULTIMMAP_SELECT:
        return multimmap_select(dev, arg);
      case MULTIMMAP_MAP_STATIC_SHADOW:
        return multimmap_map_static_shadow(dev, arg);
      case MULTIMMAP_GET_HEAP_PAGES:
        return multimmap_get_heap_pages(dev, arg);
      case MULTIMMAP_DESTROY:
        return multimmap_destroy(dev, arg);
      case MULTIMMAP_KERNEL_MEMORY_RW:
        return multimmap_kernel_memory_rw(dev, arg);
      case MULTIMMAP_INIT_KERNEL_STATICS:
        return multimmap_init_kernel_statics(dev, filp, arg);
      case MULTIMMAP_SHARED_MEMORY_RW:
        return multimmap_shared_memory_rw(arg);
      default:
        return -ENOTTY;
    }
}                       

static int multimmap_mmap(struct file* filp, struct vm_area_struct* vma)
{
    static const unsigned long KERNEL_FLAGS = 
        VM_READ|VM_WRITE|VM_SHARED|VM_MAYREAD|VM_MAYWRITE|VM_MAYEXEC|VM_MAYSHARE;

    size_t size = 0;
    struct multimmap_dev* dev = (struct multimmap_dev*)filp->private_data;

/*     printk("multimmap_mmap: vma=%p, dev=%p, pgoff=%lx, vm_start=%lx, pid=%d\n",  */
/*            vma, dev, vma->vm_pgoff, vma->vm_start, current->pid); */

    if (vma->vm_pgoff!=0 && vma->vm_pgoff!=STATICS_PGOFF && 
        vma->vm_pgoff!=KERNEL_HEAP_PGOFF && vma->vm_pgoff!=KERNEL_STATICS_PGOFF &&
        vma->vm_pgoff!=SHARED_MEMORY_PGOFF) return -EINVAL;
    if (vma->vm_pgoff==0 && 
        (dev->vma!=0 || dev->pgds_size!=0)) return -ENODEV;
    if (vma->vm_pgoff==STATICS_PGOFF && 
        (dev->statics==0 || dev->statics->vma!=0 ||
         dev->pgds_size!=0)) return -ENODEV;
    
    vma->vm_ops = &multimmap_vm_ops;
    //vma->vm_flags |= VM_RESERVED;
    vma->vm_flags |= VM_DONTCOPY;

    if (vma->vm_pgoff==0) {
        struct multimmap_vma_private* private = 0;

        if ( (vma->vm_start&(MULTIMMAP_HEAP_SIZE-1)) != 0) {
            return -EINVAL;
        }
        
        size = vma->vm_end - vma->vm_start;
        if (size>MULTIMMAP_HEAP_SIZE) {
            return -EINVAL;
        }
/*         printk("multimmap_mmap: vm_start=%lx, vm_end=%lx\n", */
/*                vma->vm_start, vma->vm_end); */

        private = 
            (struct multimmap_vma_private*)vmalloc(sizeof(struct multimmap_vma_private));
        private->dev = dev;
        vma->vm_private_data = private;

        dev->vma = vma;
    } else if (vma->vm_pgoff==STATICS_PGOFF) {
        struct multimmap_vma_private* private = 
            (struct multimmap_vma_private*)vmalloc(sizeof(struct multimmap_vma_private));
        private->dev = dev;
        vma->vm_private_data = private;

        dev->statics->vma = vma;
    } else if (vma->vm_pgoff==KERNEL_HEAP_PGOFF) {
        struct multimmap_kernel_mapping_info* kernel_mapping_info = 0;

        if ((vma->vm_flags&KERNEL_FLAGS)!=KERNEL_FLAGS) {
            printk("multimmap_mmap: invalid flags for the kernel heap mapping: %lx\n",
                   vma->vm_flags);
            return -EINVAL;
        }

        if (down_killable(&dev->sema)!=0) {
            return -ERESTARTSYS;
        }
        if (dev->kernel_heap_maps_count==0) {
            if ( (vma->vm_start&(MULTIMMAP_HEAP_SIZE-1)) != 0) {
                up(&dev->sema);
                return -EINVAL;
            }

            size = vma->vm_end - vma->vm_start;
/*         printk("mmap size: %lx\n", size); */
            if (size>MULTIMMAP_HEAP_SIZE) {
                up(&dev->sema);
                return -EINVAL;
            }

            //dev->kernel_heap_address = vma->vm_start;
            dev->kernel_heap_size = size;
            
            dev->kernel_heap_pages = 
                vmalloc(sizeof(unsigned long)*dev->kernel_heap_size/PAGE_SIZE);
            memset(dev->kernel_heap_pages, 0, 
                   sizeof(unsigned long)*dev->kernel_heap_size/PAGE_SIZE);
        } else {
            /* if (vma->vm_start!=dev->kernel_heap_address) { */
            /*     up(&dev->sema); */
            /*     printk("multimmap_mmap: trying to map kernel heap at 0x%lx instead of 0x%lx\n", */
            /*            vma->vm_start, dev->kernel_heap_address); */
            /*     return -EINVAL; */
            /* } */

            size = vma->vm_end - vma->vm_start;
            if (size!=dev->kernel_heap_size) {
                up(&dev->sema);
                printk("multimmap_mmap: trying to map kernel heap of size %lu instead of %lu\n",
                       size, dev->kernel_heap_size);
                return -EINVAL;
            }
        }
/*         printk("multimmap_mmap: kernel: vm_start=%lx, vm_end=%lx\n", */
/*                vma->vm_start, vma->vm_end); */

        ++dev->kernel_heap_maps_count;
        up(&dev->sema);

        kernel_mapping_info =
            (struct multimmap_kernel_mapping_info*)vmalloc(sizeof(struct multimmap_kernel_mapping_info));
        
        sema_init(&kernel_mapping_info->sema, 1);
        kernel_mapping_info->base.dev = dev;
        kernel_mapping_info->is_rw = 1;
        kernel_mapping_info->rw_entry = 0;
        kernel_mapping_info->ro_entry = 0;
        
        vma->vm_private_data = kernel_mapping_info;
    } else if (vma->vm_pgoff==KERNEL_STATICS_PGOFF) {
        struct multimmap_kernel_mapping_info* kernel_mapping_info = 0;

        if ((vma->vm_flags&KERNEL_FLAGS)!=KERNEL_FLAGS) {
            printk("multimmap_mmap: invalid flags for the kernel statics mapping: %lx\n",
                   vma->vm_flags);
            return -EINVAL;
        }

        if (down_killable(&dev->sema)!=0) {
            return -ERESTARTSYS;
        }
        if (dev->kernel_statics_maps_count>0) {
            if (vma->vm_start!=dev->kernel_statics_address) {
                up(&dev->sema);
                printk("multimmap_mmap: trying to map kernel statics at 0x%lx instead of 0x%lx\n",
                       vma->vm_start, dev->kernel_statics_address);
                return -EINVAL;
            }

            size = vma->vm_end - vma->vm_start;
            if (size!=dev->kernel_statics_size) {
                up(&dev->sema);
                printk("multimmap_mmap: trying to map kernel statics of size %lu instead of %lu\n",
                       size, dev->kernel_statics_size);
                return -EINVAL;
            }
        }
/*         printk("multimmap_mmap: kernel: vm_start=%lx, vm_end=%lx\n", */
/*                vma->vm_start, vma->vm_end); */

        ++dev->kernel_statics_maps_count;
        up(&dev->sema);

        kernel_mapping_info =
            (struct multimmap_kernel_mapping_info*)vmalloc(sizeof(struct multimmap_kernel_mapping_info));
        
        sema_init(&kernel_mapping_info->sema, 1);
        kernel_mapping_info->base.dev = dev;
        kernel_mapping_info->is_rw = 1;
        kernel_mapping_info->rw_entry = 0;
        kernel_mapping_info->ro_entry = 0;
        
        vma->vm_private_data = kernel_mapping_info;
    } else if (vma->vm_pgoff==SHARED_MEMORY_PGOFF) {
        struct multimmap_shared_memory_mapping_info* shared_memory_mapping_info = 0;

        if ((vma->vm_flags&KERNEL_FLAGS)!=KERNEL_FLAGS) {
            printk("multimmap_mmap: invalid flags for the shared memory mapping: %lx\n",
                   vma->vm_flags);
            return -EINVAL;
        }

        if (down_killable(&shared_memory.sema)!=0) {
            return -ERESTARTSYS;
        }
        if (shared_memory.maps_count==0) {
            if ( (vma->vm_start&(MULTIMMAP_SHARED_MEMORY_SIZE-1)) != 0) {
                up(&shared_memory.sema);
                return -EINVAL;
            }

            size = vma->vm_end - vma->vm_start;
/*         printk("mmap size: %lx\n", size); */
            if (size>MULTIMMAP_SHARED_MEMORY_SIZE) {
                up(&dev->sema);
                return -EINVAL;
            }

            shared_memory.address = vma->vm_start;
            shared_memory.size = size;
            
            shared_memory.pages = 
                vmalloc(sizeof(unsigned long)*shared_memory.size/PAGE_SIZE);
            memset(shared_memory.pages, 0, 
                   sizeof(unsigned long)*shared_memory.size/PAGE_SIZE);
        } else {
            if (vma->vm_start!=shared_memory.address) {
                up(&shared_memory.sema);
                printk("multimmap_mmap: trying to map shared memory at 0x%lx instead of 0x%lx\n",
                       vma->vm_start, shared_memory.address);
                return -EINVAL;
            }

            size = vma->vm_end - vma->vm_start;
            if (size!=shared_memory.size) {
                up(&shared_memory.sema);
                printk("multimmap_mmap: trying to map shared memory of size %lu instead of %lu\n",
                       size, shared_memory.size);
                return -EINVAL;
            }
        }
/*         printk("multimmap_mmap: kernel: vm_start=%lx, vm_end=%lx\n", */
/*                vma->vm_start, vma->vm_end); */

        ++shared_memory.maps_count;
        up(&shared_memory.sema);

        shared_memory_mapping_info =
            (struct multimmap_shared_memory_mapping_info*)vmalloc(sizeof(struct multimmap_shared_memory_mapping_info));
        
        sema_init(&shared_memory_mapping_info->sema, 1);
        shared_memory_mapping_info->base.dev = dev;
        shared_memory_mapping_info->is_rw = 1;
        shared_memory_mapping_info->rw_entry = 0;
        shared_memory_mapping_info->ro_entry = 0;
        
        vma->vm_private_data = shared_memory_mapping_info;
    } else {
        BUG();
    }

    return 0;
}

static void multimmap_vma_open(struct vm_area_struct* vma)
{
	printk("WARNING: multimmap_vma_open: vma=%p, pgoff=%lx, pid=%d called\n",
           vma, vma->vm_pgoff, current->pid);
    //BUG();

    //send_signal(SIGSEGV, &info, p, 1);
    kill_proc(current->pid, SIGSEGV, 1);
}

static struct page* multimmap_vma_nopage(struct vm_area_struct* vma, 
                                         unsigned long fault_address, int* type)
{
    struct page* page = 0;
    
    struct multimmap_vma_private* private = 
        (struct multimmap_vma_private*)vma->vm_private_data;
    struct multimmap_dev* dev = private->dev;
    
/*     printk("multimmap_vma_fault: vma=%p, vm_start=%lx, vm_end=%lx, pgoff=%lx (%lx), virtual_address: %016lx\n", */
/*            vma, vma->vm_start, vma->vm_end, vma->vm_pgoff, KERNEL_HEAP_PGOFF, fault_address);  */

    
    if (vma->vm_pgoff==KERNEL_HEAP_PGOFF) {
        unsigned long pageno = (fault_address - vma->vm_start) / PAGE_SIZE;
        
        if (down_killable(&dev->sema)!=0) {
            printk("multimmap_vma_nopage: fatal signal received while waiting for dev->sema\n");
            return 0;
        }
        if (dev->kernel_heap_pages[pageno]==0) {
            dev->kernel_heap_pages[pageno] = alloc_page(GFP_USER);
        }
        page = dev->kernel_heap_pages[pageno];
        up(&dev->sema);

        // We call it even if this page has just been allocated, so that we
        // have an extra reference. It is possible that one process maps the
        // page, than that process exits and then the other process wants to
        // use this page, but it has already been released. This extra
        // reference is removed in multimmap_vma_close when the last VMA is
        // closed. 
        get_page(page);
    } else if (vma->vm_pgoff==KERNEL_STATICS_PGOFF) {
        unsigned long pageno = 
            (fault_address - dev->kernel_statics_address) / PAGE_SIZE;
        if (dev->kernel_statics_pages!=0) {
            page = dev->kernel_statics_pages[pageno];
            get_page(page);
        }
    } else if (vma->vm_pgoff==SHARED_MEMORY_PGOFF) {
        unsigned long pageno = 
            (fault_address - shared_memory.address) / PAGE_SIZE;
        
        if (down_killable(&shared_memory.sema)!=0) {
            printk("multimmap_vma_nopage: fatal signal received while waiting for shared_memory.sema\n");
            return 0;
        }
        if (shared_memory.pages[pageno]==0) {
            shared_memory.pages[pageno] = alloc_page(GFP_USER|__GFP_ZERO);
        }
        page = shared_memory.pages[pageno];
        up(&shared_memory.sema);

        // We call it even if this page has just been allocated, so that we
        // have an extra reference. It is possible that one process maps the
        // page, than that process exits and then the other process wants to
        // use this page, but it has already been released. This extra
        // reference is removed in multimmap_vma_close when the last VMA is
        // closed. 
        get_page(page);
    } else {
        unsigned long pgoffset = (fault_address - vma->vm_start) / PAGE_SIZE;
        if (vma->vm_pgoff==STATICS_PGOFF && dev->statics->shadow_mapped) {
            if (pgoffset>=dev->statics->num_pages) {
                if (!dev->selected_process_memory) {
                    dev->had_failed_statics_request = 1;
                }
                page = 0;
            } else {
                if (dev->statics->shadow[pgoffset]==0) {
                    dev->statics->shadow[pgoffset] = alloc_page(GFP_USER);
                }
                page = dev->statics->shadow[pgoffset];
                if (page!=0) {
                    get_page(page);
                }
            }
        } else {
            if (dev->current_pgd==INVALID_PGD_INDEX) {
                if (vma->vm_pgoff==STATICS_PGOFF && !dev->selected_process_memory) 
                {
                    dev->had_failed_statics_request = 1;
                }
                page = 0;
            } else {
                if (vma->vm_pgoff==0) {
                    page = alloc_page(GFP_USER);
                } else if (vma->vm_pgoff==STATICS_PGOFF) {
                    if (pgoffset>=dev->statics->num_pages) {
                        page = 0;
                    } else if (dev->statics->shadow[pgoffset]!=0) {
                        page = alloc_page(GFP_USER);
                        if (page!=0) {
                            memcpy(page_address(page), 
                                   page_address(dev->statics->shadow[pgoffset]), 
                                   PAGE_SIZE);
                        }
                    } else {
                        page= 0;
                    }
                }
                if (page!=0) {
                    ++dev->pgds[dev->current_pgd].num_pages;
                }
            }
        }
    }
    
    if (page==0 && type!=0) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22)
        *type = VM_FAULT_ERROR;
#else
        *type = VM_FAULT_OOM;
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22) */
    }

    if (page==0) {
        printk("multimmap_vma_fault: unhandled fault: vma=%p, vm_start=%lx, vm_end=%lx, pgoff=%lx (%lx), virtual_address: %016lx\n",
               vma, vma->vm_start, vma->vm_end, vma->vm_pgoff, KERNEL_HEAP_PGOFF, fault_address);
    }
    return page;
}

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22)
static int multimmap_vma_fault(struct vm_area_struct* vma,
                               struct vm_fault* fault)
{
    fault->page = 
        multimmap_vma_nopage(vma, 
                             (unsigned long)fault->virtual_address,
                             0);
    
    return (fault->page==0) ? VM_FAULT_SIGBUS : 0;
}
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22) */
 

static void multimmap_release_statics(struct multimmap_dev* dev)
{
    if (dev->users==0 && dev->statics!=0 && dev->statics->vma==0) {        
        multimmap_free_statics(dev);
    }
}

static void multimmap_vma_close(struct vm_area_struct* vma)
{
    unsigned i;
    struct multimmap_vma_private* private = 
        (struct multimmap_vma_private*)vma->vm_private_data;
    struct multimmap_dev* dev = private->dev;

/*     printk("multimmap_vma_close: vma=%p, vm_file=%p, pgoff=%lx, pid=%d\n", */
/*            vma, vma->vm_file, vma->vm_pgoff, current->pid);  */

/*     printk("multimmap_vma_close, current->mm: %p, dev: %p, current->mm: %p, dev->vma: %p, vma: %p, nr_ptes: %lu, current_pgd: %u, vm_start: %lx, vm_end: %lx\n", */
/*            current->mm, dev, current->mm, dev->vma, vma, vma->vm_mm->nr_ptes, dev->current_pgd, vma->vm_start, vma->vm_end); */
/*     print_vmas(current->mm); */
    
    down(&dev->sema);
    if (dev->kernel_heap_maps_count>0 && vma->vm_pgoff==KERNEL_HEAP_PGOFF) {
        struct multimmap_kernel_mapping_info* kernel_mapping_info =
            (struct multimmap_kernel_mapping_info*)private;        

/*         printk("multimmap_vma_close: kernel VMA\n"); */
/*         pgd_t* entry = pgd_offset(dev->kernel_vma->vm_mm, dev->kernel_vma->vm_start); */
/*         printk("multimmap_vma_close: ro_entry=%lx, rw_entry=%lx, entry=%p (%lx)\n", */
/*                dev->kernel_ro_entry, dev->kernel_rw_entry, entry, pgd_val(*entry)); */
        pgd_t pgd = __pgd(kernel_mapping_info->is_rw ? 
                          kernel_mapping_info->ro_entry : kernel_mapping_info->rw_entry);
        up(&dev->sema);

/*         printk("multimmap_vma_close: ro_entry=%lx, rw_entry=%lx\n", */
/*                kernel_mapping_info->ro_entry, kernel_mapping_info->rw_entry); */

/*        printk("multimmap_vma_close: pgd=%lx\n", pgd_val(pgd)); */
        if (pgd_val(pgd)!=0) {
            multimmap_walk_pgd(&pgd, &multimmap_dec_nr_ptes, vma->vm_mm);
            multimmap_add_unmap_pgd(dev, pgd_val(pgd));
        }

        vfree(kernel_mapping_info);
        
        down(&dev->sema);
        --dev->kernel_heap_maps_count;
        if (dev->kernel_heap_maps_count==0) {
            unsigned long num_kernel_heap_pages = dev->kernel_heap_size/PAGE_SIZE;
            unsigned long i = 0;
            for(; i<num_kernel_heap_pages; ++i) {
                struct page* page = dev->kernel_heap_pages[i];
                if (page!=0) put_page(page);
            }

            //dev->kernel_heap_address = 0;
            dev->kernel_heap_size = 0;
            vfree(dev->kernel_heap_pages);
            dev->kernel_heap_pages = 0;
        }
        up(&dev->sema);
    } else if (dev->kernel_statics_maps_count>0 && vma->vm_start==dev->kernel_statics_address) {
        struct multimmap_kernel_mapping_info* kernel_mapping_info =
            (struct multimmap_kernel_mapping_info*)private;

/*         pgd_t* entry = pgd_offset(dev->kernel_vma->vm_mm, dev->kernel_vma->vm_start); */
/*         printk("multimmap_vma_close: ro_entry=%lx, rw_entry=%lx, entry=%p (%lx)\n", */
/*                dev->kernel_ro_entry, dev->kernel_rw_entry, entry, pgd_val(*entry)); */
        pmd_t pmd = __pmd(kernel_mapping_info->is_rw ? 
                          kernel_mapping_info->ro_entry : kernel_mapping_info->rw_entry);
        up(&dev->sema);

/*         printk("multimmap_vma_close: ro_entry=%lx, rw_entry=%lx\n", */
/*                kernel_mapping_info->ro_entry, kernel_mapping_info->rw_entry); */

/*        printk("multimmap_vma_close: pgd=%lx\n", pgd_val(pgd)); */
        if (pmd_val(pmd)!=0) {
            multimmap_walk_pmd(&pmd, &multimmap_dec_nr_ptes, vma->vm_mm);
            multimmap_add_unmap_pmd(dev, pmd_val(pmd));
        }

        vfree(kernel_mapping_info);
        
        down(&dev->sema);
        --dev->kernel_statics_maps_count;
        if (dev->kernel_statics_maps_count==0) {
            multimmap_release_kernel_statics(dev);
        }
        up(&dev->sema);
    } else if (shared_memory.maps_count>0 && vma->vm_start==shared_memory.address) {
        struct multimmap_shared_memory_mapping_info* shared_memory_mapping_info =
            (struct multimmap_shared_memory_mapping_info*)private;
        

/*         printk("multimmap_vma_close: kernel VMA\n"); */
/*         pgd_t* entry = pgd_offset(dev->kernel_vma->vm_mm, dev->kernel_vma->vm_start); */
/*         printk("multimmap_vma_close: ro_entry=%lx, rw_entry=%lx, entry=%p (%lx)\n", */
/*                dev->kernel_ro_entry, dev->kernel_rw_entry, entry, pgd_val(*entry)); */
        pud_t pud = __pud(shared_memory_mapping_info->is_rw ? 
                          shared_memory_mapping_info->ro_entry : shared_memory_mapping_info->rw_entry);
        up(&dev->sema);

/*         printk("multimmap_vma_close: ro_entry=%lx, rw_entry=%lx\n", */
/*                kernel_mapping_info->ro_entry, kernel_mapping_info->rw_entry); */

/*        printk("multimmap_vma_close: pgd=%lx\n", pgd_val(pgd)); */
        if (pud_val(pud)!=0) {
            multimmap_walk_pud(&pud, &multimmap_dec_nr_ptes, vma->vm_mm);
            multimmap_add_unmap_pud(dev, pud_val(pud));
        }

        vfree(shared_memory_mapping_info);
        
        down(&shared_memory.sema);
        --shared_memory.maps_count;
        if (shared_memory.maps_count==0) {
            unsigned long num_shared_memory_pages = shared_memory.size/PAGE_SIZE;
            unsigned long i = 0;
            for(; i<num_shared_memory_pages; ++i) {
                struct page* page = shared_memory.pages[i];
                if (page!=0) put_page(page);
            }

            shared_memory.address = 0;
            shared_memory.size = 0;
            vfree(shared_memory.pages);
            shared_memory.pages = 0;
        }
        up(&shared_memory.sema);
    } else {
        if (vma==dev->vma) {
/*         printk("multimmap_vma_close: heap VMA\n"); */
            for(i=0; i<dev->pgds_size; ++i) {
                if (i!=dev->current_pgd && multimmap_is_pgd_mapped(dev->pgds+i)) {
                    if (dev->pgds[i].heap!=0) {
                        pgd_t pgd = __pgd(dev->pgds[i].heap);
/*                     printk("walk pgd of heap of %u\n", i); */
                        multimmap_walk_pgd(&pgd, &multimmap_dec_nr_ptes, vma->vm_mm);
                    }
                }
            }
            if (dev->current_pgd!=INVALID_PGD_INDEX && (dev->statics==0 || dev->statics->vma==0)) {
                multimmap_free_pgd(dev, dev->current_pgd);
            }        
            dev->vma = 0;
        } else if (dev->statics!=0 && vma==dev->statics->vma) {
/*         printk("multimmap_vma_close: statics VMA\n"); */
            for(i=0; i<dev->pgds_size; ++i) {
                if ((i!=dev->current_pgd || dev->statics->shadow_mapped) && multimmap_is_pgd_mapped(dev->pgds+i)) {
                    if (dev->pgds[i].u.statics!=0) {
                        pud_t pud = __pud(dev->pgds[i].u.statics);
/*                     printk("walk pmd of statics of %u\n", i); */
                        multimmap_walk_pud(&pud, &multimmap_dec_nr_ptes, vma->vm_mm);
                    }
                }
            }

            if (!dev->statics->shadow_mapped && dev->statics->shadow_pud!=0) {
                pud_t pud = __pud(dev->statics->shadow_pud);
                multimmap_walk_pud(&pud, &multimmap_dec_nr_ptes, vma->vm_mm);
            }

            if (dev->had_failed_statics_request && vma->vm_mm->nr_ptes>0) {
                --vma->vm_mm->nr_ptes;
            }
            
            dev->statics->vma = 0;
            multimmap_release_statics(dev);
            if (dev->current_pgd!=INVALID_PGD_INDEX && dev->vma==0) {
                multimmap_free_pgd(dev, dev->current_pgd);
            }
        }
        vfree(private);
        up(&dev->sema);
    }

/*     printk("multimmap_vma_close1, nr_ptes: %lu\n", vma->vm_mm->nr_ptes); */
}

static int multimmap_release(struct inode* inode, struct file* filp)
{
    struct multimmap_dev* dev = (struct multimmap_dev*)filp->private_data;
   
    down(&dev->sema);

/*     printk("multimmap_release: users=%u\n", dev->users); */

    --dev->users;
    multimmap_release_statics(dev);
    multimmap_release_kernel_statics(dev);

    up(&dev->sema);

    return 0;
}

static __exit void multimmap_exit(void)
{
#if IS_LICENSE_GPL
    unsigned i;
    for(i=0; i<numdevs; ++i) {
        device_destroy(multimmap_class, MKDEV(major, i));
    }
    class_unregister(multimmap_class);
    class_destroy(multimmap_class);
#endif

    multimmap_clear_devs();
    unregister_chrdev_region(MKDEV(major, minor), numdevs);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = &multimmap_open,
    .ioctl = &multimmap_ioctl,
    .mmap = &multimmap_mmap,
    .release = &multimmap_release,
};

static struct vm_operations_struct multimmap_vm_ops = {
    .open = &multimmap_vma_open,
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22)
    .fault = &multimmap_vma_fault,
#else
    .nopage = &multimmap_vma_nopage,
#endif /* LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22) */
    .close = &multimmap_vma_close,
};

module_init(multimmap_init);
module_exit(multimmap_exit);
