
#include <linux/file.h>

#include "hi_module.h"
#include "hi_drv_module.h"
#include "hifb_drv.h"

#include "hifb.h"
#include "hifb_p.h"

#include "hifb_comm.h"
#include <sw_sync.h>























HI_S32 hifb_create_fence(struct sw_sync_timeline * pstTimeline, char* name, HI_U32 u32FenceValue)
{
	HI_S32 s32Ret;
	struct sync_pt *p;
	struct sync_fence* fence;

	if (pstTimeline == NULL)
	{
		return -22;
	}

	s32Ret = get_unused_fd_flags(0);
	if (s32Ret < 0)
	{
		HIFB_ERROR("get_unused_fd failed!\n"); //50
		return s32Ret;
	}
	p = sw_sync_pt_create(pstTimeline, u32FenceValue);
	if (p == NULL)
	{
		return -12;
	}
	fence = sync_fence_create(name, p);
	if (fence == NULL)
	{
		s32Ret = -12;
		sync_pt_free(p);
		return s32Ret;
	}
	sync_fence_install(fence, s32Ret);
	return s32Ret;
}

HI_S32 hifb_fence_wait(HI_S32 fd, HI_U32 b)
{
	struct sync_fence *fence;
	int ip;

	fence = sync_fence_fdget(fd);
	if (fence == 0)
	{
		HIFB_ERROR("sync_fence_fdget failed!\n"); //77
		return -22;
	}
	ip = sync_fence_wait(fence, b);
	if (ip == -ETIME)
	{
		ip = sync_fence_wait(fence, 10000);
	}
	if (ip < 0)
	{
		HIFB_WARNING("error waiting on fence: 0x%x\n", ip); //86
	}

	sync_fence_put(fence);

	return HI_SUCCESS;
}


