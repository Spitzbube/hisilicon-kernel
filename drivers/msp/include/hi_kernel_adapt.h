#include <linux/version.h>
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36))
#include <linux/smp_lock.h>
#define HI_DECLARE_MUTEX(x) DECLARE_MUTEX(x)
#else
#define HI_DECLARE_MUTEX(x) DEFINE_SEMAPHORE(x)
#endif

#define HI_INIT_MUTEX(x)  sema_init(x, 1)