	.arch armv7-a
	.fpu softvfp
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 2
	.eabi_attribute 18, 4
	.file	"drv_disp_alg_rgb2yuv.c"
@ GNU C (Hisilicon_v200(gcc4.4-290+glibc-2.11+eabi+nptl)) version 4.4.1 (arm-hisiv200-linux-gnueabi)
@	compiled by GNU C version 4.1.1 20061011 (Red Hat 4.1.1-30), GMP version 4.3.1, MPFR version 2.4.2.
@ GGC heuristics: --param ggc-min-expand=100 --param ggc-min-heapsize=131072
@ options passed:  -nostdinc
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/arch/arm/include
@ -Iarch/arm/include/generated -Iinclude
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/drivers/msp/vo
@ -Idrivers/msp/vo
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/arch/arm/mach-s40/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/common/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/common/api/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/common/drv/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/api/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/api/higo/include
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/api/vo
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0/hal/common
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0/hal/cv200
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0/alg
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0/com
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0/drv
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0
@ -I/home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/../../../source/msp/drv/vo/vdp_v1_0/include
@ -imultilib armv7a_soft -iprefix
@ /opt/hisi-linux/x86-arm/arm-hisiv200-linux/bin/../lib/gcc/arm-hisiv200-linux-gnueabi/4.4.1/
@ -isysroot /opt/hisi-linux/x86-arm/arm-hisiv200-linux/bin/../target
@ -D__KERNEL__ -D__LINUX_ARM_ARCH__=7 -Uarm -DHI_DISP_BUILD_FULL
@ -D__DISP_PLATFORM_SDK__ -D__DISP_D0_FOLLOW_D1__ -D__DISP_SELF_START__
@ -DHI_VO_SHARPNESS_SUPPORT -DHI_VO_HD_VIDEO_DO_DEI -DCHIP_TYPE_hi3716cv200
@ -DSDK_VERSION=HiSTBLinuxV100R002C00SPC020 -DHI_GPIOI2C_SUPPORT
@ -DHI_LOG_SUPPORT=1 -DHI_LOG_LEVEL=4 -DHI_PROC_SUPPORT=1
@ -DHI_KEYLED_SUPPORT -DHI_HDCP_SUPPORT -DHI_SCI_SUPPORT -DHI_VI_SUPPORT
@ -DHI_VENC_SUPPORT -DHI_AENC_SUPPORT -DKBUILD_STR(s)=#s
@ -DKBUILD_BASENAME=KBUILD_STR(drv_disp_alg_rgb2yuv)
@ -DKBUILD_MODNAME=KBUILD_STR(hi_vou) -isystem
@ /opt/hisi-linux/x86-arm/arm-hisiv200-linux/bin/../lib/gcc/arm-hisiv200-linux-gnueabi/4.4.1/include
@ -include
@ /home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/include/linux/kconfig.h
@ -MD drivers/msp/vo/./vdp_v1_0/alg/.drv_disp_alg_rgb2yuv.o.d
@ /home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/drivers/msp/vo/./vdp_v1_0/alg/drv_disp_alg_rgb2yuv.c
@ -mlittle-endian -marm -mapcs -mno-sched-prolog -mabi=aapcs-linux
@ -mno-thumb-interwork -march=armv7-a -msoft-float -auxbase-strip
@ drivers/msp/vo/./vdp_v1_0/alg/drv_disp_alg_rgb2yuv.o -O2 -Wall -Wundef
@ -Wstrict-prototypes -Wno-trigraphs -Werror-implicit-function-declaration
@ -Wno-format-security -Wframe-larger-than=1024
@ -Wdeclaration-after-statement -Wno-pointer-sign -p -fno-strict-aliasing
@ -fno-common -fno-delete-null-pointer-checks -fno-dwarf2-cfi-asm
@ -funwind-tables -fno-stack-protector -fno-omit-frame-pointer
@ -fno-optimize-sibling-calls -fno-strict-overflow -fconserve-stack
@ -fverbose-asm
@ options enabled:  -falign-loops -fargument-alias -fauto-inc-dec
@ -fbranch-count-reg -fcaller-saves -fcprop-registers -fcrossjumping
@ -fcse-follow-jumps -fdefer-pop -fearly-inlining
@ -feliminate-unused-debug-types -fexpensive-optimizations
@ -fforward-propagate -ffunction-cse -fgcse -fgcse-lm
@ -fguess-branch-probability -fident -fif-conversion -fif-conversion2
@ -findirect-inlining -finline -finline-functions-called-once
@ -finline-small-functions -fipa-cp -fipa-pure-const -fipa-reference
@ -fira-share-save-slots -fira-share-spill-slots -fivopts
@ -fkeep-static-consts -fleading-underscore -fmath-errno -fmerge-constants
@ -fmerge-debug-strings -fmove-loop-invariants -foptimize-register-move
@ -fpeephole -fpeephole2 -fprofile -fpromote-loop-indices
@ -freg-struct-return -fregmove -frename-registers -freorder-blocks
@ -freorder-functions -frerun-cse-after-loop -fsched-interblock
@ -fsched-spec -fsched-stalled-insns-dep -fschedule-insns -fschedule-insns2
@ -fsection-anchors -fsigned-zeros -fsplit-ivs-in-unroller
@ -fsplit-wide-types -fthread-jumps -ftoplevel-reorder -ftrapping-math
@ -ftree-builtin-call-dce -ftree-ccp -ftree-ch -ftree-copy-prop
@ -ftree-copyrename -ftree-cselim -ftree-dce -ftree-dominator-opts
@ -ftree-dse -ftree-fre -ftree-loop-im -ftree-loop-ivcanon
@ -ftree-loop-optimize -ftree-parallelize-loops= -ftree-pre -ftree-reassoc
@ -ftree-scev-cprop -ftree-sink -ftree-sra -ftree-switch-conversion
@ -ftree-ter -ftree-vect-loop-version -ftree-vrp -funit-at-a-time
@ -funroll-loops -funwind-tables -fverbose-asm -fweb
@ -fzero-initialized-in-bss -mapcs-frame -mglibc -mlittle-endian

@ Compiler executable checksum: fa59e0bd179ef45b3a3c3e6f3383fdac

	.text
	.align	2
	.global	DISP_ALG_CscRgb2Yuv
	.type	DISP_ALG_CscRgb2Yuv, %function
DISP_ALG_CscRgb2Yuv:
	.fnstart
.LFB0:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #28
	sub	sp, sp, #28	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	ldrb	r2, [r0, #2]	@ zero_extendqisi2	@ D.1742, <variable>.u8Blue
	ldrb	ip, [r0, #0]	@ zero_extendqisi2	@ D.1733, <variable>.u8Red
	movw	r6, #439	@ tmp181,
	ldrb	r0, [r0, #1]	@ zero_extendqisi2	@ D.1737, <variable>.u8Green
	mov	r7, #368	@ tmp211,
	mul	r8, r6, r2	@ tmp180, tmp181, D.1742
	mvn	r5, #147	@ tmp174,
	mul	sl, r7, r0	@, tmp211, D.1737
	movw	r3, #:lower16:274877907	@ tmp145,
	mul	r7, r6, ip	@, tmp181, D.1733
	movt	r3, #:upper16:274877907	@ tmp145,
	mul	r5, r5, ip	@ tmp173, tmp174, D.1733
	movw	r9, #291	@ tmp193,
	mul	r9, r9, r0	@ tmp192, tmp193, D.1737
	rsb	r4, r0, r0, asl #6	@, tmp150, D.1737, D.1737,
	str	r7, [fp, #-48]	@, %sfp
	add	ip, ip, ip, asl #8	@, tmp142, D.1733, D.1733,
	smull	r7, r6, r3, r8	@,, tmp145, tmp180
	mov	r4, r4, asl #3	@ tmp151, tmp150,
	str	sl, [fp, #-64]	@, %sfp
	mov	r7, #98	@ tmp163,
	smull	r0, sl, r3, r5	@, tmp176, tmp145, tmp173
	add	r0, r2, r2, asl #3	@, tmp222, D.1742, D.1742,
	mul	r8, r7, r2	@ tmp162, tmp163, D.1742
	rsb	r0, r2, r0, asl #3	@, tmp224, D.1742, tmp222,
	str	r6, [fp, #-56]	@, %sfp
	smull	r2, r9, r3, r9	@, tmp195, tmp145, tmp192
	mov	r2, r5, asr #31	@ tmp179, tmp173,
	rsb	r5, r2, sl, asr #6	@, tmp175, tmp179, tmp176,
	ldr	r6, [fp, #-64]	@, %sfp
	smull	sl, r2, r3, r4	@, tmp153, tmp145, tmp151
	mov	r4, r4, asr #31	@ tmp156, tmp151,
	ldr	sl, [fp, #-56]	@, %sfp
	smull	r6, r7, r3, r6	@, tmp213, tmp145,
	add	r6, r5, sl, lsr #6	@, tmp189, tmp175,,
	smull	r5, ip, r3, ip	@, tmp144, tmp145, tmp142
	sub	r6, r6, #128	@ tmp191, tmp189,
	ldr	r5, [fp, #-48]	@, %sfp
	rsb	r4, r4, r2, asr #6	@, tmp152, tmp156, tmp153,
	sub	r9, r6, r9, lsr #6	@ tmp201, tmp191, tmp195,
	mov	r7, r7, lsr #6	@ tmp212, tmp213,
	smull	r6, r8, r3, r8	@, tmp165, tmp145, tmp162
	uxtb	r9, r9	@ D.1759, tmp201
	smull	r5, sl, r3, r5	@, tmp206, tmp145,
	cmp	r9, #15	@ D.1759,
	smull	r6, r5, r3, r0	@, tmp226, tmp145, tmp224
	mov	r0, r0, asr #31	@ tmp229, tmp224,
	add	r2, r4, ip, lsr #6	@, tmp159, tmp152, tmp144,
	strb	r9, [r1, #4]	@ D.1759, <variable>.u8Cb
	rsb	r7, r7, sl, lsr #6	@, tmp219, tmp212, tmp206,
	add	ip, r2, #16	@ tmp161, tmp159,
	add	r8, ip, r8, lsr #6	@, tmp171, tmp161, tmp165,
	movls	r3, #16	@ tmp236,
	strb	r8, [r1, #3]	@ tmp171, <variable>.u8Y
	rsb	r5, r0, r5, asr #6	@, tmp225, tmp229, tmp226,
	strlsb	r3, [r1, #4]	@ tmp236, <variable>.u8Cb
	rsb	r5, r5, r7	@ tmp232, tmp225, tmp219
	sub	r5, r5, #128	@ tmp234, tmp232,
	strb	r5, [r1, #5]	@ tmp234, <variable>.u8Cr
	bls	.L3	@,
	cmp	r9, #240	@ D.1759,
	mvnhi	r3, #15	@ tmp238,
	strhib	r3, [r1, #4]	@ tmp238, <variable>.u8Cb
.L3:
	ldrb	r3, [r1, #5]	@ zero_extendqisi2	@ D.1782, <variable>.u8Cr
	cmp	r3, #15	@ D.1782,
	movls	r3, #16	@ tmp240,
	strlsb	r3, [r1, #5]	@ tmp240, <variable>.u8Cr
	bls	.L6	@,
	cmp	r3, #240	@ D.1782,
	mvnhi	r3, #15	@ tmp242,
	strhib	r3, [r1, #5]	@ tmp242, <variable>.u8Cr
.L6:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
	.fnend
	.size	DISP_ALG_CscRgb2Yuv, .-DISP_ALG_CscRgb2Yuv
	.ident	"GCC: (Hisilicon_v200(gcc4.4-290+glibc-2.11+eabi+nptl)) 4.4.1"
	.section	.note.GNU-stack,"",%progbits
