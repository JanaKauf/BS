#ifndef _TRANSLATE_H
#define _TRANSLATE_H

#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */

#ifndef init_MUTEX
#define init_MUTEX(mutex) sema_init(mutex, 1)
#endif	/* init_MUTEX */

/*
 * Macros to help debugging
 */
#define PRINT_DEBUG \
       printk (KERN_DEBUG "[% s]: FUNC:% s: LINE:% d \ n", __FILE__,
               __FUNCTION__, __LINE__)

#undef PDEBUG             /* undef it, just in case */
#ifdef SCULL_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "trans: " fmt, ## args)
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#ifndef TRANS_MAJOR
#define TRANS_MAJOR 0   /* dynamic major by default */
#endif

#ifndef TRANS_NR_DEVS
#define TRANS_NR_DEVS 2    /* trans0 through scull1 */
#endif

#ifndef SCULL_P_NR_DEVS
#define SCULL_P_NR_DEVS 4  /* scullpipe0 through scullpipe3 */
#endif

#define DEVICE_NAME "trans"

#define BUFF_SIZE 40
#define SHIFT     3


#endif
