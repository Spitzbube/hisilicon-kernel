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
	.file	"drv_disp_alg_csc.c"
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
@ -DKBUILD_BASENAME=KBUILD_STR(drv_disp_alg_csc)
@ -DKBUILD_MODNAME=KBUILD_STR(hi_vou) -isystem
@ /opt/hisi-linux/x86-arm/arm-hisiv200-linux/bin/../lib/gcc/arm-hisiv200-linux-gnueabi/4.4.1/include
@ -include
@ /home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/include/linux/kconfig.h
@ -MD drivers/msp/vo/./vdp_v1_0/alg/.drv_disp_alg_csc.o.d
@ /home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/drivers/msp/vo/./vdp_v1_0/alg/drv_disp_alg_csc.c
@ -mlittle-endian -marm -mapcs -mno-sched-prolog -mabi=aapcs-linux
@ -mno-thumb-interwork -march=armv7-a -msoft-float -auxbase-strip
@ drivers/msp/vo/./vdp_v1_0/alg/drv_disp_alg_csc.o -O2 -Wall -Wundef
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
	.type	CalcCscCoef_YCbCrtoYCbCr, %function
CalcCscCoef_YCbCrtoYCbCr:
	.fnstart
.LFB3:
	@ args = 0, pretend = 0, frame = 88
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #92
	sub	sp, sp, #92	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r9, [r0, #20]	@ <variable>.u32Hue, <variable>.u32Hue
	ldr	r7, [r1, #24]	@, <variable>.s32C20
	movw	r3, #:lower16:1374389535	@ tmp544,
	movt	r3, #:upper16:1374389535	@ tmp544,
	movw	r5, #:lower16:.LANCHOR0	@ tmp188,
	rsb	sl, r9, r9, asl #4	@, tmp166, <variable>.u32Hue, <variable>.u32Hue,
	ldr	r9, [r1, #12]	@ <variable>.s32C10, <variable>.s32C10
	str	r7, [fp, #-48]	@, %sfp
	movt	r5, #:upper16:.LANCHOR0	@ tmp188,
	ldr	r8, [r1, #28]	@, <variable>.s32C21
	mov	r6, sl, asl #2	@ tmp167, tmp166,
	movw	ip, #:lower16:274877907	@ tmp545,
	movt	ip, #:upper16:274877907	@ tmp545,
	umull	sl, r6, r3, r6	@, tmp168, tmp544, tmp167
	str	r8, [fp, #-52]	@, %sfp
	ldr	r4, [r1, #32]	@, <variable>.s32C22
	str	r4, [fp, #-76]	@, %sfp
	movw	r4, #:lower16:1759218605	@ tmp546,
	ldr	sl, [r0, #16]	@ <variable>.u32Contrst, <variable>.u32Contrst
	mov	r6, r6, lsr #5	@ u32Hue, tmp168,
	ldr	r7, [r1, #0]	@, <variable>.s32C00
	movt	r4, #:upper16:1759218605	@ tmp546,
	ldr	r8, [r0, #24]	@ <variable>.u32Satur, <variable>.u32Satur
	mov	sl, sl, asl #1	@, <variable>.u32Contrst,
	str	sl, [fp, #-72]	@, %sfp
	str	r7, [fp, #-92]	@, %sfp
	ldr	r7, [r1, #4]	@, <variable>.s32C01
	mov	sl, r8, asl #1	@, <variable>.u32Satur,
	str	sl, [fp, #-80]	@, %sfp
	str	r7, [fp, #-84]	@, %sfp
	add	r7, r5, r6, asl #2	@, tmp200, tmp188, u32Hue,
	ldr	r6, [r5, r6, asl #2]	@ tmp190, VDPCOS_TABLE
	ldr	r5, [r1, #8]	@, <variable>.s32C02
	mul	r8, r9, r6	@, <variable>.s32C10, tmp190
	str	r5, [fp, #-68]	@, %sfp
	ldr	r5, [r7, #244]	@ tmp203, VDPSIN_TABLE
	str	r8, [fp, #-88]	@, %sfp
	ldr	sl, [r1, #16]	@, <variable>.s32C11
	mul	r7, sl, r6	@,, tmp190
	ldr	sl, [fp, #-48]	@, %sfp
	str	r7, [fp, #-108]	@, %sfp
	ldr	r9, [r1, #20]	@, <variable>.s32C12
	ldr	r7, [fp, #-88]	@, %sfp
	mul	r8, r9, r6	@,, tmp190
	mov	r9, r7, asr #31	@,,
	ldr	r7, [fp, #-108]	@, %sfp
	str	r9, [fp, #-64]	@, %sfp
	str	r8, [fp, #-56]	@, %sfp
	mul	r8, sl, r5	@ tmp202,, tmp203
	ldr	sl, [fp, #-52]	@, %sfp
	mul	r9, sl, r5	@ tmp230,, tmp203
	mov	sl, r7, asr #31	@,,
	ldr	r7, [fp, #-76]	@, %sfp
	str	sl, [fp, #-96]	@, %sfp
	mul	sl, r7, r5	@,, tmp203
	ldr	r7, [fp, #-56]	@, %sfp
	str	sl, [fp, #-60]	@, %sfp
	mov	sl, r7, asr #31	@,,
	str	sl, [fp, #-104]	@, %sfp
	ldr	r7, [fp, #-88]	@, %sfp
	smull	sl, r7, ip, r7	@,, tmp545,
	str	r7, [fp, #-128]	@, %sfp
	mov	r7, r8, asr #31	@, tmp202,
	str	r7, [fp, #-88]	@, %sfp
	smull	r7, sl, ip, r8	@,, tmp545, tmp202
	mov	r7, r9, asr #31	@, tmp230,
	str	r7, [fp, #-100]	@, %sfp
	str	sl, [fp, #-120]	@, %sfp
	ldr	sl, [fp, #-108]	@, %sfp
	smull	r7, r8, ip, sl	@,, tmp545,
	ldr	sl, [fp, #-60]	@, %sfp
	mov	r7, sl, asr #31	@,,
	str	r7, [fp, #-108]	@, %sfp
	str	r8, [fp, #-112]	@, %sfp
	smull	r7, r8, ip, r9	@,, tmp545, tmp230
	ldr	r9, [fp, #-128]	@, %sfp
	ldr	sl, [fp, #-64]	@, %sfp
	rsb	r7, sl, r9, asr #6	@,,,,
	str	r7, [fp, #-64]	@, %sfp
	str	r8, [fp, #-116]	@, %sfp
	ldr	r8, [fp, #-56]	@, %sfp
	ldr	r9, [fp, #-120]	@, %sfp
	smull	r7, sl, ip, r8	@,, tmp545,
	ldr	r8, [fp, #-88]	@, %sfp
	rsb	r7, r8, r9, asr #6	@,,,,
	str	r7, [fp, #-88]	@, %sfp
	ldr	r9, [fp, #-112]	@, %sfp
	str	sl, [fp, #-56]	@, %sfp
	ldr	sl, [fp, #-60]	@, %sfp
	smull	r7, r8, ip, sl	@,, tmp545,
	ldr	sl, [fp, #-96]	@, %sfp
	ldr	r7, [fp, #-72]	@, %sfp
	str	r8, [fp, #-60]	@, %sfp
	rsb	r8, sl, r9, asr #6	@,,,,
	ldr	r9, [fp, #-80]	@, %sfp
	str	r8, [fp, #-96]	@, %sfp
	ldr	sl, [fp, #-100]	@, %sfp
	mul	r8, r7, r9	@ D.2185,,
	ldr	r7, [fp, #-116]	@, %sfp
	rsb	r9, sl, r7, asr #6	@, tmp233,,,
	ldr	r7, [fp, #-56]	@, %sfp
	ldr	sl, [fp, #-104]	@, %sfp
	rsb	sl, sl, r7, asr #6	@,,,,
	ldr	r7, [fp, #-88]	@, %sfp
	str	sl, [fp, #-56]	@, %sfp
	ldr	sl, [fp, #-64]	@, %sfp
	add	sl, sl, r7	@,,
	ldr	r7, [fp, #-96]	@, %sfp
	str	sl, [fp, #-64]	@, %sfp
	add	r9, r7, r9	@,, tmp233
	str	r9, [fp, #-80]	@, %sfp
	ldr	sl, [fp, #-60]	@, %sfp
	ldr	r7, [fp, #-108]	@, %sfp
	rsb	r9, r7, sl, asr #6	@, tmp261,,,
	ldr	r7, [fp, #-64]	@, %sfp
	ldr	sl, [fp, #-56]	@, %sfp
	add	r9, sl, r9	@ tmp266,, tmp261
	mul	sl, r8, r7	@ tmp211, D.2185,
	ldr	r7, [fp, #-80]	@, %sfp
	mul	r7, r8, r7	@, D.2185,
	str	r7, [fp, #-64]	@, %sfp
	mul	r7, r8, r9	@, D.2185, tmp266
	str	r7, [fp, #-80]	@, %sfp
	smull	r7, r9, r4, sl	@,, tmp546, tmp211
	mov	sl, sl, asr #31	@ tmp215, tmp211,
	str	r9, [fp, #-56]	@, %sfp
	ldr	r9, [fp, #-64]	@, %sfp
	smull	r7, r9, r4, r9	@,, tmp546,
	ldr	r7, [fp, #-64]	@, %sfp
	str	r9, [fp, #-60]	@, %sfp
	mov	r9, r7, asr #31	@,,
	ldr	r7, [fp, #-80]	@, %sfp
	str	r9, [fp, #-88]	@, %sfp
	smull	r9, r7, r4, r7	@,, tmp546,
	str	r7, [fp, #-64]	@, %sfp
	ldr	r7, [fp, #-80]	@, %sfp
	mov	r9, r7, asr #31	@ tmp271,,
	ldr	r7, [fp, #-56]	@, %sfp
	rsb	sl, sl, r7, asr #12	@,, tmp215,,
	ldr	r7, [fp, #-60]	@, %sfp
	str	sl, [fp, #-80]	@, %sfp
	str	sl, [r2, #12]	@, <variable>.s32C10
	ldr	sl, [fp, #-88]	@, %sfp
	rsb	sl, sl, r7, asr #12	@,,,,
	str	sl, [fp, #-60]	@, %sfp
	str	sl, [r2, #16]	@, <variable>.s32C11
	ldr	sl, [r1, #12]	@, <variable>.s32C10
	str	sl, [fp, #-96]	@, %sfp
	ldr	sl, [fp, #-64]	@, %sfp
	ldr	r7, [r1, #16]	@, <variable>.s32C11
	str	r7, [fp, #-88]	@, %sfp
	rsb	r7, r9, sl, asr #12	@,, tmp271,,
	str	r7, [fp, #-56]	@, %sfp
	str	r7, [r2, #20]	@, <variable>.s32C12
	ldr	r9, [r1, #20]	@ <variable>.s32C12, <variable>.s32C12
	ldr	r1, [fp, #-76]	@, %sfp
	ldr	r7, [fp, #-48]	@, %sfp
	mul	sl, r9, r5	@, <variable>.s32C12, tmp203
	ldr	r9, [fp, #-96]	@, %sfp
	mul	r1, r1, r6	@,, tmp190
	str	sl, [fp, #-104]	@, %sfp
	mul	sl, r7, r6	@ tmp273,, tmp190
	str	r1, [fp, #-116]	@, %sfp
	mul	r7, r9, r5	@ tmp286,, tmp203
	mov	r9, r1, asr #31	@,,
	str	r9, [fp, #-96]	@, %sfp
	ldr	r1, [fp, #-52]	@, %sfp
	ldr	r9, [fp, #-104]	@, %sfp
	mul	r6, r1, r6	@ tmp301,, tmp190
	mov	r1, r9, asr #31	@,,
	ldr	r9, [fp, #-88]	@, %sfp
	str	r1, [fp, #-100]	@, %sfp
	mov	r1, sl, asr #31	@, tmp273,
	str	r1, [fp, #-48]	@, %sfp
	mul	r5, r9, r5	@ tmp314,, tmp203
	smull	r1, r9, ip, sl	@,, tmp545, tmp273
	mov	sl, r7, asr #31	@, tmp286,
	str	sl, [fp, #-52]	@, %sfp
	str	r9, [fp, #-112]	@, %sfp
	smull	r1, r9, ip, r7	@,, tmp545, tmp286
	mov	r1, r6, asr #31	@, tmp301,
	smull	sl, r7, ip, r6	@,, tmp545, tmp301
	mov	sl, r5, asr #31	@, tmp314,
	str	r1, [fp, #-76]	@, %sfp
	str	r9, [fp, #-108]	@, %sfp
	ldr	r1, [fp, #-116]	@, %sfp
	ldr	r9, [fp, #-112]	@, %sfp
	smull	r6, r5, ip, r5	@, tmp318, tmp545, tmp314
	ldr	r6, [fp, #-48]	@, %sfp
	str	r7, [fp, #-88]	@, %sfp
	str	sl, [fp, #-64]	@, %sfp
	rsb	sl, r6, r9, asr #6	@, tmp276,,,
	smull	r6, r7, ip, r1	@,, tmp545,
	ldr	r1, [fp, #-108]	@, %sfp
	ldr	r6, [fp, #-104]	@, %sfp
	ldr	r9, [fp, #-52]	@, %sfp
	str	r7, [fp, #-112]	@, %sfp
	rsb	r7, r9, r1, asr #6	@, tmp289,,,
	smull	r9, r1, ip, r6	@,, tmp545,
	ldr	r6, [fp, #-88]	@, %sfp
	str	r1, [fp, #-48]	@, %sfp
	ldr	r1, [fp, #-76]	@, %sfp
	rsb	r9, r1, r6, asr #6	@, tmp304,,,
	ldr	r6, [fp, #-64]	@, %sfp
	rsb	r1, r7, sl	@, tmp289, tmp276
	ldr	sl, [fp, #-112]	@, %sfp
	rsb	r5, r6, r5, asr #6	@, tmp317,, tmp318,
	ldr	r6, [fp, #-96]	@, %sfp
	str	r1, [fp, #-52]	@, %sfp
	ldr	r7, [fp, #-92]	@, %sfp
	rsb	r1, r6, sl, asr #6	@, tmp332,,,
	ldr	sl, [fp, #-72]	@, %sfp
	rsb	r6, r5, r9	@ tmp322, tmp317, tmp304
	ldr	r9, [fp, #-48]	@, %sfp
	mul	r5, r7, sl	@,,
	ldr	r7, [fp, #-100]	@, %sfp
	str	r5, [fp, #-76]	@, %sfp
	rsb	r5, r7, r9, asr #6	@, tmp345,,,
	ldr	r9, [fp, #-84]	@, %sfp
	rsb	r1, r5, r1	@ tmp350, tmp345, tmp332
	ldr	r7, [fp, #-76]	@, %sfp
	mul	r1, r8, r1	@ tmp351, D.2185, tmp350
	mul	r5, r9, sl	@ tmp176,,
	mov	r9, r7, asr #31	@,,
	ldr	r7, [fp, #-68]	@, %sfp
	str	r9, [fp, #-48]	@, %sfp
	ldr	r9, [fp, #-52]	@, %sfp
	mul	sl, r7, sl	@ tmp182,,
	mul	r7, r8, r9	@ tmp295, D.2185,
	mov	r9, r5, asr #31	@, tmp176,
	mul	r8, r8, r6	@ tmp323, D.2185, tmp322
	str	r9, [fp, #-72]	@, %sfp
	mov	r6, r1, asr #31	@, tmp351,
	str	r6, [fp, #-64]	@, %sfp
	smull	r6, r9, r4, r1	@,, tmp546, tmp351
	mov	r6, sl, asr #31	@, tmp182,
	str	r6, [fp, #-52]	@, %sfp
	smull	r1, r5, r3, r5	@, tmp178, tmp544, tmp176
	str	r9, [fp, #-68]	@, %sfp
	ldr	r9, [fp, #-76]	@, %sfp
	smull	r1, r6, r3, r9	@, tmp172, tmp544,
	mov	r1, r8, asr #31	@, tmp323,
	str	r1, [fp, #-88]	@, %sfp
	mov	r9, r7, asr #31	@ tmp299, tmp295,
	smull	r1, sl, r3, sl	@, tmp184, tmp544, tmp182
	ldr	r1, [fp, #-48]	@, %sfp
	rsb	r6, r1, r6, asr #5	@,,, tmp172,
	ldr	r1, [fp, #-72]	@, %sfp
	str	r6, [fp, #-84]	@, %sfp
	smull	r6, r7, r4, r7	@, tmp296, tmp546, tmp295
	rsb	r5, r1, r5, asr #5	@,,, tmp178,
	str	r5, [fp, #-92]	@, %sfp
	smull	r5, r8, r4, r8	@, tmp324, tmp546, tmp323
	ldr	r6, [fp, #-52]	@, %sfp
	rsb	r9, r9, r7, asr #12	@,, tmp299, tmp296,
	ldr	r1, [fp, #-68]	@, %sfp
	rsb	r5, r6, sl, asr #5	@,,, tmp184,
	str	r5, [fp, #-76]	@, %sfp
	ldr	r4, [fp, #-84]	@, %sfp
	ldr	r6, [fp, #-92]	@, %sfp
	ldr	r5, [fp, #-76]	@, %sfp
	ldr	r7, [fp, #-88]	@, %sfp
	ldr	sl, [fp, #-64]	@, %sfp
	str	r6, [r2, #4]	@, <variable>.s32C01
	rsb	r8, r7, r8, asr #12	@,,, tmp324,
	str	r5, [r2, #8]	@, <variable>.s32C02
	rsb	sl, sl, r1, asr #12	@,,,,
	str	r8, [fp, #-48]	@, %sfp
	movw	r7, #587	@ tmp360,
	str	sl, [fp, #-72]	@, %sfp
	str	sl, [r2, #32]	@, <variable>.s32C22
	mov	sl, #114	@,
	str	r8, [r2, #28]	@, <variable>.s32C21
	str	r4, [r2, #0]	@, <variable>.s32C00
	str	r9, [fp, #-52]	@, %sfp
	str	r9, [r2, #24]	@, <variable>.s32C20
	ldr	r9, [r0, #32]	@ <variable>.u32Kg, <variable>.u32Kg
	ldr	r1, [r0, #36]	@ <variable>.u32Kb, <variable>.u32Kb
	ldr	r4, [r0, #28]	@ <variable>.u32Kr, <variable>.u32Kr
	mov	r0, r1, asl #1	@ s32Bgain, <variable>.u32Kb,
	mov	r1, r9, asl #1	@ s32Ggain, <variable>.u32Kg,
	rsb	r6, r1, r9, asl #24	@, tmp421, s32Ggain, <variable>.u32Kg,
	mov	r4, r4, asl #1	@ s32Rgain, <variable>.u32Kr,
	mul	r5, sl, r0	@ tmp361,, s32Bgain
	rsb	r8, r1, r6, asl #7	@, tmp423, s32Ggain, tmp421,
	mla	r7, r7, r1, r5	@ tmp363, tmp360, s32Ggain, tmp361
	mvn	sl, #146	@ tmp392,
	add	r9, r1, r8, asl #2	@,, s32Ggain, tmp423,
	mov	r6, #436	@ tmp390,
	mul	r8, sl, r4	@ tmp391, tmp392, s32Rgain
	mov	r5, #232	@ tmp374,
	mla	r8, r6, r0, r8	@ tmp393, tmp390, s32Bgain, tmp391
	str	r9, [fp, #-100]	@, %sfp
	mul	r6, r5, r0	@ tmp373, tmp374, s32Bgain
	movw	r9, #341	@ tmp383,
	mvn	sl, #231	@ tmp372,
	mul	r5, r9, r4	@ tmp382, tmp383, s32Rgain
	mla	r9, sl, r1, r6	@, tmp372, s32Ggain, tmp373
	mvn	sl, #340	@ tmp381,
	mla	r5, sl, r1, r5	@ tmp384, tmp381, s32Ggain, tmp382
	movw	sl, #299	@,
	mla	r7, sl, r4, r7	@ tmp366,, s32Rgain, tmp363
	movw	sl, #886	@ tmp404,
	mul	sl, sl, r0	@, tmp404, s32Bgain
	mov	r6, r9, asr #31	@,,
	str	r6, [fp, #-88]	@, %sfp
	str	r9, [fp, #-120]	@, %sfp
	mov	r9, r5, asr #31	@, tmp384,
	str	sl, [fp, #-68]	@, %sfp
	mov	sl, #168	@ tmp413,
	mul	r6, sl, r1	@ tmp412, tmp413, s32Ggain
	mvn	sl, #288	@ tmp395,
	str	r9, [fp, #-96]	@, %sfp
	mla	r9, sl, r1, r8	@, tmp395, s32Ggain, tmp393
	mov	r8, #114	@,
	mov	sl, r7, asr #31	@, tmp366,
	str	sl, [fp, #-64]	@, %sfp
	str	r9, [fp, #-128]	@, %sfp
	ldr	r9, [fp, #-68]	@, %sfp
	mla	sl, r8, r1, r9	@,, s32Ggain,
	mvn	r9, #167	@ tmp411,
	mla	r9, r9, r4, r6	@ tmp414, tmp411, s32Rgain, tmp412
	ldr	r6, [fp, #-100]	@, %sfp
	movw	r8, #615	@ tmp427,
	str	sl, [fp, #-68]	@, %sfp
	mla	sl, r8, r4, r6	@, tmp427, s32Rgain,
	ldr	r8, [fp, #-128]	@, %sfp
	mov	r6, r8, asr #31	@,,
	str	r6, [fp, #-100]	@, %sfp
	ldr	r6, [fp, #-68]	@, %sfp
	str	sl, [fp, #-112]	@, %sfp
	movw	sl, #299	@,
	mul	r8, sl, r1	@,, s32Ggain
	mov	sl, r6, asr #31	@,,
	str	sl, [fp, #-104]	@, %sfp
	mov	r6, r9, asr #31	@, tmp414,
	str	r6, [fp, #-108]	@, %sfp
	str	r8, [fp, #-116]	@, %sfp
	mov	r8, #203	@ tmp439,
	mul	sl, r8, r1	@ tmp438, tmp439, s32Ggain
	ldr	r8, [fp, #-120]	@, %sfp
	smull	r1, r7, r3, r7	@, tmp367, tmp544, tmp366
	smull	r8, r6, r3, r8	@, tmp376, tmp544,
	smull	r8, r1, r3, r5	@,, tmp544, tmp384
	ldr	r5, [fp, #-64]	@, %sfp
	rsb	r7, r5, r7, asr #5	@, D.2643,, tmp367,
	ldr	r5, [fp, #-88]	@, %sfp
	str	r1, [fp, #-120]	@, %sfp
	mvn	r1, #202	@ tmp437,
	mla	r8, r1, r0, sl	@, tmp437, s32Bgain, tmp438
	ldr	sl, [fp, #-116]	@, %sfp
	rsb	r6, r5, r6, asr #5	@, D.2639,, tmp376,
	movw	r1, #701	@ tmp446,
	mvn	r5, #99	@ tmp430,
	mla	r4, r1, r4, sl	@ tmp449, tmp446, s32Rgain,
	ldr	sl, [fp, #-120]	@, %sfp
	str	r8, [fp, #-132]	@, %sfp
	ldr	r8, [fp, #-112]	@, %sfp
	ldr	r1, [fp, #-96]	@, %sfp
	mla	r0, r5, r0, r8	@ tmp431, tmp430, s32Bgain,
	ldr	r8, [fp, #-128]	@, %sfp
	rsb	r5, r1, sl, asr #5	@, D.2635,,,
	smull	sl, r1, r3, r8	@, tmp397, tmp544,
	ldr	sl, [fp, #-132]	@, %sfp
	mov	r8, sl, asr #31	@,,
	ldr	sl, [fp, #-68]	@, %sfp
	str	r8, [fp, #-64]	@, %sfp
	smull	r8, sl, r3, sl	@,, tmp544,
	str	sl, [fp, #-96]	@, %sfp
	mov	sl, r0, asr #31	@, tmp431,
	str	sl, [fp, #-68]	@, %sfp
	smull	sl, r8, r3, r9	@,, tmp544, tmp414
	ldr	r9, [fp, #-100]	@, %sfp
	rsb	sl, r9, r1, asr #5	@, D.2629,, tmp397,
	ldr	r9, [fp, #-104]	@, %sfp
	str	r8, [fp, #-112]	@, %sfp
	smull	r1, r8, r3, r4	@,, tmp544, tmp449
	mov	r4, r4, asr #31	@ tmp453, tmp449,
	ldr	r1, [fp, #-96]	@, %sfp
	str	r8, [fp, #-88]	@, %sfp
	rsb	r8, r9, r1, asr #5	@, D.2625,,,
	smull	r1, r9, r3, r0	@,, tmp544, tmp431
	ldr	r1, [fp, #-112]	@, %sfp
	ldr	r0, [fp, #-108]	@, %sfp
	str	r9, [fp, #-100]	@, %sfp
	rsb	r9, r0, r1, asr #5	@, D.2621,,,
	ldr	r0, [fp, #-132]	@, %sfp
	smull	r1, r0, r3, r0	@,, tmp544,
	str	r0, [fp, #-104]	@, %sfp
	ldr	r1, [fp, #-52]	@, %sfp
	ldr	r3, [fp, #-100]	@, %sfp
	mul	r0, r5, r1	@, D.2635,
	ldr	r1, [fp, #-68]	@, %sfp
	str	r0, [fp, #-96]	@, %sfp
	rsb	r0, r1, r3, asr #5	@, D.2615,,,
	ldr	r3, [fp, #-84]	@, %sfp
	mul	r1, r3, r5	@,, D.2635
	ldr	r3, [fp, #-88]	@, %sfp
	rsb	r4, r4, r3, asr #5	@, D.2607, tmp453,,
	str	r1, [fp, #-68]	@, %sfp
	ldr	r1, [fp, #-80]	@, %sfp
	mul	r3, r5, r1	@, D.2635,
	ldr	r1, [fp, #-104]	@, %sfp
	ldr	r5, [fp, #-64]	@, %sfp
	str	r3, [fp, #-88]	@, %sfp
	rsb	r3, r5, r1, asr #5	@, D.2611,,,
	ldr	r1, [fp, #-52]	@, %sfp
	mul	r5, r7, r1	@, D.2643,
	ldr	r1, [fp, #-52]	@, %sfp
	str	r5, [fp, #-100]	@, %sfp
	mul	r5, r6, r1	@, D.2639,
	ldr	r1, [fp, #-84]	@, %sfp
	str	r5, [fp, #-104]	@, %sfp
	mul	r5, r1, r7	@,, D.2643
	ldr	r1, [fp, #-84]	@, %sfp
	str	r5, [fp, #-52]	@, %sfp
	mul	r5, r1, r6	@,, D.2639
	ldr	r1, [fp, #-80]	@, %sfp
	mul	r6, r6, r1	@ tmp495, D.2639,
	str	r5, [fp, #-84]	@, %sfp
	mul	r7, r7, r1	@ tmp485, D.2643,
	ldr	r5, [fp, #-96]	@, %sfp
	ldr	r1, [fp, #-48]	@, %sfp
	mla	r1, r9, r1, r5	@, D.2621,,
	ldr	r5, [fp, #-68]	@, %sfp
	str	r1, [fp, #-64]	@, %sfp
	ldr	r1, [fp, #-92]	@, %sfp
	mla	r1, r1, r9, r5	@,, D.2621,
	ldr	r5, [fp, #-88]	@, %sfp
	str	r1, [fp, #-80]	@, %sfp
	ldr	r1, [fp, #-60]	@, %sfp
	mla	r9, r9, r1, r5	@ tmp506, D.2621,,
	ldr	r5, [fp, #-100]	@, %sfp
	ldr	r1, [fp, #-48]	@, %sfp
	mla	r1, sl, r1, r5	@, D.2629,,
	ldr	r5, [fp, #-104]	@, %sfp
	str	r1, [fp, #-68]	@, %sfp
	ldr	r1, [fp, #-48]	@, %sfp
	mla	r1, r8, r1, r5	@, D.2625,,
	str	r1, [fp, #-96]	@, %sfp
	ldr	r5, [fp, #-52]	@, %sfp
	ldr	r1, [fp, #-92]	@, %sfp
	mla	r1, r1, sl, r5	@,, D.2629,
	ldr	r5, [fp, #-84]	@, %sfp
	str	r1, [fp, #-48]	@, %sfp
	ldr	r1, [fp, #-92]	@, %sfp
	mla	r1, r1, r8, r5	@,, D.2625,
	str	r1, [fp, #-52]	@, %sfp
	ldr	r1, [fp, #-60]	@, %sfp
	mla	r7, sl, r1, r7	@, D.2629,, tmp485
	ldr	sl, [fp, #-64]	@, %sfp
	mla	r5, r8, r1, r6	@, D.2625,, tmp495
	ldr	r6, [fp, #-76]	@, %sfp
	ldr	r1, [fp, #-80]	@, %sfp
	str	r7, [fp, #-92]	@, %sfp
	ldr	r7, [fp, #-72]	@, %sfp
	str	r5, [fp, #-84]	@, %sfp
	ldr	r5, [fp, #-56]	@, %sfp
	mla	r8, r4, r7, sl	@ tmp538, D.2607,,
	ldr	sl, [fp, #-68]	@, %sfp
	mla	r7, r6, r4, r1	@ tmp478,, D.2607,
	ldr	r6, [fp, #-72]	@, %sfp
	mla	r4, r4, r5, r9	@ tmp508, D.2607,, tmp506
	ldr	r9, [fp, #-96]	@, %sfp
	mla	r1, r0, r6, sl	@, D.2615,,
	ldr	sl, [fp, #-72]	@, %sfp
	mov	r5, r8, asr #31	@, tmp538,
	str	r5, [fp, #-68]	@, %sfp
	mov	r6, r7, asr #31	@, tmp478,
	str	r6, [fp, #-60]	@, %sfp
	mla	r5, r3, sl, r9	@ tmp528, D.2611,,
	ldr	sl, [fp, #-76]	@, %sfp
	str	r1, [fp, #-88]	@, %sfp
	mov	r9, r4, asr #31	@, tmp508,
	ldr	r1, [fp, #-48]	@, %sfp
	str	r9, [fp, #-80]	@, %sfp
	ldr	r9, [fp, #-88]	@, %sfp
	mla	r6, sl, r0, r1	@ tmp458,, D.2615,
	ldr	r1, [fp, #-52]	@, %sfp
	mla	sl, sl, r3, r1	@ tmp468,, D.2611,
	mov	r1, r9, asr #31	@,,
	ldr	r9, [fp, #-92]	@, %sfp
	str	r1, [fp, #-52]	@, %sfp
	ldr	r1, [fp, #-56]	@, %sfp
	mla	r0, r0, r1, r9	@ tmp488, D.2615,,
	mov	r1, r5, asr #31	@, tmp528,
	ldr	r9, [fp, #-56]	@, %sfp
	str	r1, [fp, #-76]	@, %sfp
	ldr	r1, [fp, #-84]	@, %sfp
	mla	r3, r3, r9, r1	@ tmp498, D.2611,,
	mov	r9, r6, asr #31	@, tmp458,
	str	r9, [fp, #-64]	@, %sfp
	smull	r9, r1, ip, r8	@,, tmp545, tmp538
	mov	r9, sl, asr #31	@ tmp472, tmp468,
	mov	r8, r0, asr #31	@, tmp488,
	str	r1, [fp, #-56]	@, %sfp
	smull	r1, r6, ip, r6	@, tmp459, tmp545, tmp458
	smull	r1, sl, ip, sl	@, tmp469, tmp545, tmp468
	smull	r1, r7, ip, r7	@, tmp479, tmp545, tmp478
	ldr	r1, [fp, #-64]	@, %sfp
	rsb	sl, r9, sl, asr #6	@, tmp473, tmp472, tmp469,
	str	r8, [fp, #-48]	@, %sfp
	rsb	r6, r1, r6, asr #6	@, tmp463,, tmp459,
	mov	r8, r3, asr #31	@ tmp502, tmp498,
	smull	r1, r0, ip, r0	@, tmp489, tmp545, tmp488
	smull	r9, r3, ip, r3	@, tmp499, tmp545, tmp498
	ldr	r9, [fp, #-60]	@, %sfp
	ldr	r1, [fp, #-48]	@, %sfp
	rsb	r7, r9, r7, asr #6	@, tmp483,, tmp479,
	str	r6, [r2, #0]	@ tmp463, <variable>.s32C00
	smull	r9, r4, ip, r4	@, tmp509, tmp545, tmp508
	rsb	r3, r8, r3, asr #6	@, tmp503, tmp502, tmp499,
	ldr	r9, [fp, #-88]	@, %sfp
	rsb	r0, r1, r0, asr #6	@, tmp493,, tmp489,
	ldr	r6, [fp, #-56]	@, %sfp
	str	sl, [r2, #4]	@ tmp473, <variable>.s32C01
	smull	r9, r1, ip, r9	@, tmp519, tmp545,
	smull	r8, ip, ip, r5	@, tmp529, tmp545, tmp528
	ldr	r9, [fp, #-80]	@, %sfp
	ldr	r5, [fp, #-68]	@, %sfp
	rsb	r4, r9, r4, asr #6	@, tmp513,, tmp509,
	ldr	sl, [fp, #-76]	@, %sfp
	ldr	r9, [fp, #-52]	@, %sfp
	rsb	r8, r5, r6, asr #6	@, tmp543,,,
	rsb	ip, sl, ip, asr #6	@, tmp533,, tmp529,
	str	r7, [r2, #8]	@ tmp483, <variable>.s32C02
	rsb	r1, r9, r1, asr #6	@, tmp523,, tmp519,
	str	r0, [r2, #12]	@ tmp493, <variable>.s32C10
	str	r3, [r2, #16]	@ tmp503, <variable>.s32C11
	str	r8, [r2, #32]	@ tmp543, <variable>.s32C22
	str	r4, [r2, #20]	@ tmp513, <variable>.s32C12
	str	r1, [r2, #24]	@ tmp523, <variable>.s32C20
	str	ip, [r2, #28]	@ tmp533, <variable>.s32C21
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
	.fnend
	.size	CalcCscCoef_YCbCrtoYCbCr, .-CalcCscCoef_YCbCrtoYCbCr
	.align	2
	.global	VDPGetCscType
	.type	VDPGetCscType, %function
VDPGetCscType:
	.fnstart
.LFB5:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {fp, ip, lr, pc}	@,
	.pad #4
	.save {fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	sub	r0, r0, #2	@ tmp135, eCSDrv,
	cmp	r0, #10	@ tmp135,
	ldrls	pc, [pc, r0, asl #2]	@ tmp135
	b	.L4	@
.L10:
	.word	.L5
	.word	.L6
	.word	.L4
	.word	.L4
	.word	.L4
	.word	.L4
	.word	.L4
	.word	.L7
	.word	.L8
	.word	.L4
	.word	.L9
.L9:
	mov	r0, #3	@ tmp144,
	mov	ip, #1	@ tmp145,
	stmia	r1, {r0, ip}	@ phole stm
	ldmfd	sp, {fp, sp, pc}	@
.L4:
	mov	r3, #0	@ tmp146,
	str	r3, [r1, #4]	@ tmp146, <variable>.eCsRange
	str	r3, [r1, #0]	@ tmp146, <variable>.eCsType
	ldmfd	sp, {fp, sp, pc}	@
.L5:
	mov	r3, #2	@ tmp136,
	str	r3, [r1, #4]	@ tmp136, <variable>.eCsRange
	str	r3, [r1, #0]	@ tmp136, <variable>.eCsType
	ldmfd	sp, {fp, sp, pc}	@
.L6:
	mov	ip, #2	@ tmp138,
	mov	r2, #1	@ tmp139,
	str	ip, [r1, #0]	@ tmp138, <variable>.eCsType
	str	r2, [r1, #4]	@ tmp139, <variable>.eCsRange
	ldmfd	sp, {fp, sp, pc}	@
.L7:
	mov	r3, #1	@ tmp140,
	mov	r0, #2	@ tmp141,
	str	r3, [r1, #0]	@ tmp140, <variable>.eCsType
	str	r0, [r1, #4]	@ tmp141, <variable>.eCsRange
	ldmfd	sp, {fp, sp, pc}	@
.L8:
	mov	r2, #1	@ tmp142,
	str	r2, [r1, #4]	@ tmp142, <variable>.eCsRange
	str	r2, [r1, #0]	@ tmp142, <variable>.eCsType
	ldmfd	sp, {fp, sp, pc}	@
	.fnend
	.size	VDPGetCscType, .-VDPGetCscType
	.global	__aeabi_idiv
	.align	2
	.global	DISP_ALG_CscCoefSet
	.type	DISP_ALG_CscCoefSet, %function
DISP_ALG_CscCoefSet:
	.fnstart
.LFB8:
	@ args = 0, pretend = 0, frame = 160
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #164
	sub	sp, sp, #164	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	sub	r6, fp, #60	@ tmp895,,
	sub	r8, fp, #52	@ tmp896,,
	mov	r5, r0	@ pstCscDrvPara, pstCscDrvPara
	ldr	sl, [r0, #12]	@ <variable>.u32Bright, <variable>.u32Bright
	mov	r4, r1	@ pstCscRtlPara, pstCscRtlPara
	ldr	r0, [r0, #0]	@, <variable>.eInputCS
	mov	r1, r6	@, tmp895
	ldr	r7, [r5, #16]	@ tmpContrst, <variable>.u32Contrst
	bl	VDPGetCscType	@
	mov	r1, r8	@, tmp896
	ldr	r0, [r5, #4]	@, <variable>.eOutputCS
	bl	VDPGetCscType	@
	ldr	r0, [fp, #-60]	@ stAlgCSIn.eCsType, stAlgCSIn.eCsType
	cmp	r7, #0	@ tmpContrst,
	sub	sl, sl, #50	@ tmpBright, <variable>.u32Bright,
	sub	r3, r0, #1	@ tmp183, stAlgCSIn.eCsType,
	movne	r1, r7	@ tmpContrst, tmpContrst
	moveq	r1, #1	@ tmpContrst,
	cmp	r3, #1	@ tmp183,
	bls	.L65	@,
	ldr	r2, [fp, #-52]	@ stAlgCSOut.eCsType, stAlgCSOut.eCsType
	sub	r7, r2, #1	@ tmp242, stAlgCSOut.eCsType,
	cmp	r7, #1	@ tmp242,
	bls	.L66	@,
	ldr	r3, [fp, #-56]	@ stAlgCSIn.eCsRange, stAlgCSIn.eCsRange
	cmp	r3, #1	@ stAlgCSIn.eCsRange,
	beq	.L61	@,
	cmp	r3, #2	@ stAlgCSIn.eCsRange,
	beq	.L67	@,
.L61:
	mov	r0, #0	@ tmp270,
	str	r0, [r4, #0]	@ tmp270, <variable>.s32CscDcIn_0
	str	r0, [r4, #8]	@ tmp270, <variable>.s32CscDcIn_2
	str	r0, [r4, #4]	@ tmp270, <variable>.s32CscDcIn_1
.L44:
	ldr	r3, [fp, #-48]	@ stAlgCSOut.eCsRange, stAlgCSOut.eCsRange
	cmp	r3, #1	@ stAlgCSOut.eCsRange,
	beq	.L62	@,
	cmp	r3, #2	@ stAlgCSOut.eCsRange,
	beq	.L68	@,
.L62:
	movw	r2, #298	@ tmp288,
	mul	r7, r2, sl	@ tmp287, tmp288, tmpBright
	add	r3, r7, #128	@ tmp289, tmp287,
	add	r0, r3, #255	@ tmp291, tmp289,
	cmp	r3, #0	@ tmp289,
	movlt	r3, r0	@ tmp290, tmp291
	mov	sl, r3, asr #8	@ tmp292, tmp290,
	str	sl, [r4, #12]	@ tmp292, <variable>.s32CscDcOut_0
	str	sl, [r4, #20]	@ tmp292, <variable>.s32CscDcOut_2
	str	sl, [r4, #16]	@ tmp292, <variable>.s32CscDcOut_1
.L25:
	ldr	r0, [r4, #4]	@ <variable>.s32CscDcIn_1, <variable>.s32CscDcIn_1
	sub	r9, fp, #96	@ tmp894,,
	add	r2, r9, #4	@ tmp898, tmp894,
	ldr	sl, [r4, #8]	@ <variable>.s32CscDcIn_2, <variable>.s32CscDcIn_2
	mov	r3, #0	@ tmp306,
	mov	r1, r8	@, tmp896
	str	r3, [r2], #4	@ tmp306, stCscTbl
	mov	r7, r0, asl #2	@ tmp296, <variable>.s32CscDcIn_1,
	str	r3, [r2], #4	@ tmp306, stCscTbl
	mov	sl, sl, asl #2	@ tmp294, <variable>.s32CscDcIn_2,
	str	r3, [r2], #4	@ tmp306, stCscTbl
	ldr	r0, [r4, #12]	@ <variable>.s32CscDcOut_0, <variable>.s32CscDcOut_0
	ldr	ip, [r4, #0]	@ <variable>.s32CscDcIn_0, <variable>.s32CscDcIn_0
	ldr	r8, [r4, #20]	@ <variable>.s32CscDcOut_2, <variable>.s32CscDcOut_2
	ldr	lr, [r4, #16]	@ <variable>.s32CscDcOut_1, <variable>.s32CscDcOut_1
	mov	ip, ip, asl #2	@ tmp298, <variable>.s32CscDcIn_0,
	str	r3, [r2], #4	@ tmp306, stCscTbl
	mov	r8, r8, asl #2	@ tmp300, <variable>.s32CscDcOut_2,
	str	r3, [r2], #4	@ tmp306, stCscTbl
	mov	lr, lr, asl #2	@ tmp302, <variable>.s32CscDcOut_1,
	str	ip, [r4, #0]	@ tmp298, <variable>.s32CscDcIn_0
	str	r8, [r4, #20]	@ tmp300, <variable>.s32CscDcOut_2
	mov	ip, r0, asl #2	@ tmp304, <variable>.s32CscDcOut_0,
	str	lr, [r4, #16]	@ tmp302, <variable>.s32CscDcOut_1
	movw	r8, #:lower16:.LANCHOR1	@ tmp319,
	str	ip, [r4, #12]	@ tmp304, <variable>.s32CscDcOut_0
	movt	r8, #:upper16:.LANCHOR1	@ tmp319,
	stmib	r4, {r7, sl}	@ phole stm
	str	r3, [r2], #4	@ tmp306, stCscTbl
	ldr	r0, [r5, #0]	@, <variable>.eInputCS
	str	r3, [r2], #4	@ tmp306, stCscTbl
	str	r3, [r2, #0]	@ tmp306, stCscTbl
	str	r3, [fp, #-96]	@ tmp306, stCscTbl
	bl	VDPGetCscType	@
	mov	r1, r6	@, tmp895
	ldr	r0, [r5, #4]	@, <variable>.eOutputCS
	bl	VDPGetCscType	@
	ldr	r1, [fp, #-52]	@ D.2805, stAlgCSIn.eCsType
	ldr	r0, [fp, #-60]	@ D.2804, stAlgCSOut.eCsType
	cmp	r1, #2	@ D.2805,
	add	r6, r0, r1, asl #2	@, tmp321, D.2804, D.2805,
	ldr	r1, [r8, r6, asl #2]	@ pCSCMatrixOri, CSC_MatrixTbl
	bls	.L48	@,
	cmp	r0, #2	@ D.2804,
	bls	.L49	@,
	ldr	r3, [r5, #20]	@ <variable>.u32Hue, <variable>.u32Hue
	movw	r2, #:lower16:1374389535	@ tmp330,
	movt	r2, #:upper16:1374389535	@ tmp330,
	ldr	r9, [r5, #24]	@ <variable>.u32Satur, <variable>.u32Satur
	movw	sl, #:lower16:.LANCHOR0	@ tmp333,
	movt	sl, #:upper16:.LANCHOR0	@ tmp333,
	rsb	r1, r3, r3, asl #4	@, tmp327, <variable>.u32Hue, <variable>.u32Hue,
	mov	r8, #242	@ tmp343,
	mov	r3, r9, asl #1	@ s32SatAdj, <variable>.u32Satur,
	movw	r7, #475	@ tmp373,
	mov	ip, r1, asl #2	@ tmp328, tmp327,
	movw	r1, #906	@ tmp478,
	add	r9, r3, r9, asl #2	@, tmp436, s32SatAdj, <variable>.u32Satur,
	umull	r6, r0, r2, ip	@, tmp329, tmp330, tmp328
	movw	r6, #299	@ tmp418,
	mul	ip, r1, r3	@, tmp478, s32SatAdj
	mul	r8, r8, r3	@ tmp342, tmp343, s32SatAdj
	mov	r0, r0, lsr #5	@ u32Hue, tmp329,
	mul	r7, r7, r3	@ tmp372, tmp373, s32SatAdj
	add	r1, sl, r0, asl #2	@, tmp336, tmp333, u32Hue,
	str	ip, [fp, #-108]	@, %sfp
	movw	ip, #718	@,
	ldr	r2, [sl, r0, asl #2]	@ tmp339, VDPCOS_TABLE
	mov	r0, #146	@ tmp512,
	mul	sl, r0, r3	@ tmp511, tmp512, s32SatAdj
	ldr	r0, [r1, #244]	@ D.2910, VDPSIN_TABLE
	mul	r1, ip, r3	@ tmp395,, s32SatAdj
	rsb	ip, r3, r9, asl #5	@,, s32SatAdj, tmp436,
	movw	r9, #394	@ tmp462,
	str	ip, [fp, #-112]	@, %sfp
	mul	ip, r9, r3	@, tmp462, s32SatAdj
	mov	r9, #760	@ tmp495,
	mul	r6, r6, r3	@ tmp417, tmp418, s32SatAdj
	mul	r1, r0, r1	@ tmp397, D.2910, tmp395
	str	ip, [fp, #-120]	@, %sfp
	mul	ip, r9, r3	@, tmp495, s32SatAdj
	ldr	r9, [fp, #-108]	@, %sfp
	mul	r8, r0, r8	@ tmp344, D.2910, tmp342
	mul	r7, r0, r7	@ tmp374, D.2910, tmp372
	str	ip, [fp, #-104]	@, %sfp
	mul	ip, r0, r9	@ tmp479, D.2910,
	mul	r9, r0, sl	@, D.2910, tmp511
	ldr	sl, [fp, #-112]	@, %sfp
	mul	r6, r0, r6	@ tmp419, D.2910, tmp417
	mul	r2, r2, r3	@ D.2903, tmp339, s32SatAdj
	movw	r3, #:lower16:351843721	@ tmp347,
	str	r9, [fp, #-116]	@, %sfp
	movt	r3, #:upper16:351843721	@ tmp347,
	mul	r9, r0, sl	@ tmp439, D.2910,
	ldr	sl, [fp, #-120]	@, %sfp
	mul	sl, r0, sl	@, D.2910,
	str	sl, [fp, #-120]	@, %sfp
	ldr	sl, [fp, #-104]	@, %sfp
	mul	r0, r0, sl	@, D.2910,
	str	r0, [fp, #-128]	@, %sfp
	ldr	sl, [r5, #16]	@, <variable>.u32Contrst
	mov	r0, sl, asl #1	@ s32ContrstAdj,,
	movw	sl, #907	@ tmp521,
	mul	sl, sl, r2	@, tmp521, D.2903
	str	sl, [fp, #-132]	@, %sfp
	ldr	sl, [r5, #32]	@, <variable>.u32Kg
	mov	sl, sl, asl #1	@,,
	str	sl, [fp, #-152]	@, %sfp
	movw	sl, #601	@ tmp367,
	mul	sl, sl, r2	@, tmp367, D.2903
	str	sl, [fp, #-104]	@, %sfp
	ldr	sl, [r5, #36]	@, <variable>.u32Kb
	mov	sl, sl, asl #1	@,,
	str	sl, [fp, #-140]	@, %sfp
	mov	sl, #117	@ tmp390,
	mul	sl, sl, r2	@, tmp390, D.2903
	str	sl, [fp, #-112]	@, %sfp
	ldr	sl, [r5, #28]	@, <variable>.u32Kr
	mov	sl, sl, asl #1	@,,
	str	sl, [fp, #-144]	@, %sfp
	movw	sl, #306	@ tmp412,
	mul	sl, sl, r2	@, tmp412, D.2903
	str	sl, [fp, #-124]	@, %sfp
	mov	sl, r8, asr #31	@, tmp344,
	str	sl, [fp, #-148]	@, %sfp
	movw	sl, #423	@ tmp448,
	mul	sl, sl, r2	@, tmp448, D.2903
	str	sl, [fp, #-136]	@, %sfp
	mov	sl, r7, asr #31	@, tmp374,
	str	sl, [fp, #-160]	@, %sfp
	movw	sl, #718	@,
	mul	r2, sl, r2	@,, D.2903
	smull	sl, r8, r3, r8	@, tmp346, tmp347, tmp344
	mov	sl, ip, asr #31	@, tmp479,
	str	sl, [fp, #-164]	@, %sfp
	smull	sl, r7, r3, r7	@, tmp376, tmp347, tmp374
	mov	sl, r1, asr #31	@, tmp397,
	str	r2, [fp, #-108]	@, %sfp
	mov	r2, r6, asr #31	@ tmp424, tmp419,
	str	sl, [fp, #-156]	@, %sfp
	smull	sl, r6, r3, r6	@, tmp421, tmp347, tmp419
	ldr	sl, [fp, #-148]	@, %sfp
	rsb	r8, sl, r8, asr #13	@,,, tmp346,
	str	r8, [fp, #-148]	@, %sfp
	ldr	sl, [fp, #-160]	@, %sfp
	smull	r8, ip, r3, ip	@, tmp481, tmp347, tmp479
	rsb	r8, sl, r7, asr #13	@,,, tmp376,
	str	r8, [fp, #-160]	@, %sfp
	rsb	sl, r2, r6, asr #13	@,, tmp424, tmp421,
	smull	r8, r7, r3, r1	@,, tmp347, tmp397
	ldr	r6, [fp, #-120]	@, %sfp
	str	sl, [fp, #-172]	@, %sfp
	smull	r1, sl, r3, r9	@, tmp441, tmp347, tmp439
	str	r7, [fp, #-176]	@, %sfp
	ldr	r1, [fp, #-164]	@, %sfp
	mov	r9, r9, asr #31	@ tmp444, tmp439,
	ldr	r8, [fp, #-128]	@, %sfp
	rsb	r7, r1, ip, asr #13	@,,, tmp481,
	str	r7, [fp, #-188]	@, %sfp
	smull	r7, r2, r3, r6	@,, tmp347,
	rsb	r9, r9, sl, asr #13	@,, tmp444, tmp441,
	ldr	r1, [fp, #-116]	@, %sfp
	ldr	ip, [fp, #-120]	@, %sfp
	smull	r8, r7, r3, r8	@, tmp498, tmp347,
	str	r2, [fp, #-168]	@, %sfp
	smull	r2, r6, r3, r1	@,, tmp347,
	mov	r8, ip, asr #31	@ tmp468,,
	ldr	r2, [fp, #-128]	@, %sfp
	ldr	ip, [fp, #-132]	@, %sfp
	ldr	sl, [fp, #-168]	@, %sfp
	str	r6, [fp, #-120]	@, %sfp
	mov	r6, r2, asr #31	@ tmp501,,
	smull	r2, r1, r3, ip	@,, tmp347,
	ldr	ip, [fp, #-116]	@, %sfp
	mov	r2, ip, asr #31	@ tmp518,,
	str	r1, [fp, #-200]	@, %sfp
	ldr	r1, [fp, #-108]	@, %sfp
	smull	ip, r1, r3, r1	@,, tmp347,
	ldr	ip, [fp, #-176]	@, %sfp
	str	r9, [fp, #-176]	@, %sfp
	str	r1, [fp, #-128]	@, %sfp
	ldr	r1, [fp, #-156]	@, %sfp
	rsb	r1, r1, ip, asr #13	@,,,,
	str	r1, [fp, #-164]	@, %sfp
	ldr	r1, [fp, #-104]	@, %sfp
	smull	ip, r1, r3, r1	@,, tmp347,
	str	r1, [fp, #-116]	@, %sfp
	ldr	r1, [fp, #-112]	@, %sfp
	smull	r9, ip, r3, r1	@,, tmp347,
	rsb	r1, r8, sl, asr #13	@,, tmp468,,
	ldr	r9, [fp, #-124]	@, %sfp
	ldr	sl, [fp, #-136]	@, %sfp
	str	r1, [fp, #-184]	@, %sfp
	smull	r1, r8, r3, r9	@,, tmp347,
	str	ip, [fp, #-156]	@, %sfp
	rsb	ip, r6, r7, asr #13	@,, tmp501, tmp498,
	ldr	r1, [fp, #-120]	@, %sfp
	smull	r7, r9, r3, sl	@,, tmp347,
	rsb	r6, r2, r1, asr #13	@,, tmp518,,
	str	r8, [fp, #-168]	@, %sfp
	str	ip, [fp, #-192]	@, %sfp
	str	r9, [fp, #-180]	@, %sfp
	str	r6, [fp, #-196]	@, %sfp
	ldr	r3, [fp, #-140]	@, %sfp
	ldr	sl, [fp, #-144]	@, %sfp
	ldr	ip, [fp, #-148]	@, %sfp
	mul	r8, r0, r3	@, s32ContrstAdj,
	ldr	r1, [fp, #-104]	@, %sfp
	ldr	r3, [fp, #-112]	@, %sfp
	ldr	r7, [fp, #-152]	@, %sfp
	ldr	r2, [fp, #-108]	@, %sfp
	ldr	r6, [fp, #-160]	@, %sfp
	str	r8, [fp, #-120]	@, %sfp
	add	r8, ip, #304	@ tmp350,,
	mul	ip, r0, sl	@ D.2898, s32ContrstAdj,
	ldr	sl, [fp, #-124]	@, %sfp
	mul	r0, r0, r7	@ D.2887, s32ContrstAdj,
	mov	r7, r1, asr #31	@ tmp371,,
	add	r1, r6, #600	@ tmp380,,
	mov	r6, r3, asr #31	@ tmp394,,
	mov	r3, sl, asr #31	@ tmp416,,
	ldr	sl, [fp, #-136]	@, %sfp
	mov	r9, r2, asr #31	@,,
	ldr	r2, [fp, #-172]	@, %sfp
	str	r9, [fp, #-108]	@, %sfp
	add	r8, r8, #2	@ tmp350, tmp350,
	rsb	r9, r2, #304	@,,
	mov	r2, sl, asr #31	@,,
	ldr	sl, [fp, #-132]	@, %sfp
	add	r1, r1, #1	@ tmp380, tmp380,
	str	r9, [fp, #-112]	@, %sfp
	ldr	r9, [fp, #-188]	@, %sfp
	str	r2, [fp, #-124]	@, %sfp
	add	r2, r9, #304	@ tmp485,,
	mov	r9, sl, asr #31	@,,
	ldr	sl, [fp, #-116]	@, %sfp
	add	r2, r2, #2	@ tmp485, tmp485,
	str	r9, [fp, #-104]	@, %sfp
	ldr	r9, [fp, #-156]	@, %sfp
	rsb	r7, r7, sl, asr #13	@, D.2889, tmp371,,
	ldr	sl, [fp, #-168]	@, %sfp
	rsb	r6, r6, r9, asr #13	@, D.2878, tmp394,,
	ldr	r9, [fp, #-128]	@, %sfp
	rsb	r3, r3, sl, asr #13	@, D.2867, tmp416,,
	ldr	sl, [fp, #-108]	@, %sfp
	rsb	r2, r3, r2	@ tmp487, D.2867, tmp485
	rsb	sl, sl, r9, asr #13	@,,,,
	ldr	r9, [fp, #-180]	@, %sfp
	str	sl, [fp, #-108]	@, %sfp
	ldr	sl, [fp, #-124]	@, %sfp
	rsb	sl, sl, r9, asr #13	@,,,,
	ldr	r9, [fp, #-200]	@, %sfp
	str	sl, [fp, #-124]	@, %sfp
	ldr	sl, [fp, #-104]	@, %sfp
	rsb	sl, sl, r9, asr #13	@,,,,
	str	sl, [fp, #-132]	@, %sfp
	ldr	sl, [fp, #-164]	@, %sfp
	rsb	r9, sl, #117	@,,
	ldr	sl, [fp, #-112]	@, %sfp
	str	r9, [fp, #-104]	@, %sfp
	add	r9, sl, #2	@,,
	str	r9, [fp, #-112]	@, %sfp
	ldr	sl, [fp, #-176]	@, %sfp
	rsb	r9, sl, #600	@ tmp446,,
	ldr	sl, [fp, #-184]	@, %sfp
	add	sl, sl, #117	@,,
	str	sl, [fp, #-136]	@, %sfp
	ldr	sl, [fp, #-192]	@, %sfp
	rsb	sl, sl, #600	@,,
	str	sl, [fp, #-128]	@, %sfp
	ldr	sl, [fp, #-196]	@, %sfp
	rsb	sl, sl, #117	@,,
	str	sl, [fp, #-116]	@, %sfp
	ldr	sl, [fp, #-108]	@, %sfp
	add	r8, r8, sl	@, tmp350,
	str	r8, [fp, #-108]	@, %sfp
	ldr	r8, [fp, #-124]	@, %sfp
	ldr	sl, [fp, #-136]	@, %sfp
	add	r9, r9, r8	@, tmp446,
	ldr	r8, [fp, #-128]	@, %sfp
	str	r9, [fp, #-124]	@, %sfp
	rsb	r9, r6, sl	@, D.2878,
	ldr	sl, [fp, #-132]	@, %sfp
	str	r9, [fp, #-136]	@, %sfp
	rsb	r9, r7, r8	@ tmp504, D.2889,
	ldr	r8, [fp, #-116]	@, %sfp
	rsb	r7, r7, r1	@ tmp382, D.2889, tmp380
	mul	r1, ip, r2	@, D.2898, tmp487
	add	r8, r8, sl	@,,
	ldr	sl, [fp, #-104]	@, %sfp
	str	r8, [fp, #-116]	@, %sfp
	rsb	r6, r6, sl	@ tmp404, D.2878,
	ldr	r8, [fp, #-112]	@, %sfp
	mla	sl, r9, r0, r0	@, tmp504, D.2887, D.2887
	rsb	r3, r3, r8	@, D.2867,
	ldr	r9, [fp, #-120]	@, %sfp
	ldr	r8, [fp, #-116]	@, %sfp
	str	r1, [fp, #-140]	@, %sfp
	ldr	r2, [fp, #-108]	@, %sfp
	str	sl, [fp, #-144]	@, %sfp
	ldr	r1, [fp, #-140]	@, %sfp
	mul	sl, r9, r8	@ tmp528,,
	str	r3, [fp, #-104]	@, %sfp
	mul	r8, ip, r2	@ tmp360, D.2898,
	mov	r9, r1, asr #31	@,,
	mul	r7, r0, r7	@ tmp383, D.2887, tmp382
	ldr	r2, [fp, #-144]	@, %sfp
	str	r9, [fp, #-128]	@, %sfp
	movw	r3, #:lower16:1759218605	@ tmp362,
	ldr	r9, [fp, #-120]	@, %sfp
	movt	r3, #:upper16:1759218605	@ tmp362,
	mov	r1, r2, asr #31	@,,
	mov	r2, sl, asr #31	@, tmp528,
	str	r1, [fp, #-116]	@, %sfp
	mul	r6, r9, r6	@ tmp405,, tmp404
	ldr	r1, [fp, #-104]	@, %sfp
	str	r2, [fp, #-132]	@, %sfp
	mov	r9, r8, asr #31	@, tmp360,
	ldr	r2, [fp, #-124]	@, %sfp
	mul	ip, ip, r1	@ tmp428, D.2898,
	str	r9, [fp, #-108]	@, %sfp
	mla	r0, r2, r0, r0	@ tmp455,, D.2887, D.2887
	mov	r1, r7, asr #31	@, tmp383,
	str	r1, [fp, #-104]	@, %sfp
	ldr	r2, [fp, #-120]	@, %sfp
	ldr	r9, [fp, #-136]	@, %sfp
	mul	r1, r2, r9	@ tmp471,,
	mov	r2, r6, asr #31	@, tmp405,
	str	r2, [fp, #-120]	@, %sfp
	smull	r2, r9, r3, sl	@,, tmp362, tmp528
	mov	sl, ip, asr #31	@ tmp432, tmp428,
	smull	r2, r6, r3, r6	@, tmp406, tmp362, tmp405
	ldr	r2, [fp, #-108]	@, %sfp
	str	r9, [fp, #-136]	@, %sfp
	smull	r9, r8, r3, r8	@, tmp361, tmp362, tmp360
	mov	r9, r0, asr #31	@, tmp455,
	str	r9, [fp, #-112]	@, %sfp
	smull	r9, r7, r3, r7	@, tmp384, tmp362, tmp383
	mov	r9, r1, asr #31	@ tmp475, tmp471,
	rsb	r8, r2, r8, asr #12	@,,, tmp361,
	str	r8, [fp, #-96]	@, stCscTbl.s32C00
	smull	r8, ip, r3, ip	@, tmp429, tmp362, tmp428
	ldr	r8, [fp, #-104]	@, %sfp
	ldr	r2, [fp, #-120]	@, %sfp
	rsb	r7, r8, r7, asr #12	@, tmp388,, tmp384,
	str	r7, [fp, #-92]	@ tmp388, stCscTbl.s32C01
	smull	r8, r1, r3, r1	@, tmp472, tmp362, tmp471
	rsb	ip, sl, ip, asr #12	@, tmp433, tmp432, tmp429,
	smull	r8, r0, r3, r0	@, tmp456, tmp362, tmp455
	rsb	r6, r2, r6, asr #12	@, tmp410,, tmp406,
	ldr	sl, [fp, #-140]	@, %sfp
	ldr	r8, [fp, #-112]	@, %sfp
	rsb	r1, r9, r1, asr #12	@, tmp476, tmp475, tmp472,
	ldr	r9, [fp, #-136]	@, %sfp
	smull	sl, r2, r3, sl	@, tmp489, tmp362,
	rsb	r0, r8, r0, asr #12	@, tmp460,, tmp456,
	ldr	sl, [fp, #-144]	@, %sfp
	ldr	r8, [fp, #-132]	@, %sfp
	str	r6, [fp, #-88]	@ tmp410, stCscTbl.s32C02
	smull	sl, r3, r3, sl	@, tmp506, tmp362,
	rsb	sl, r8, r9, asr #12	@, tmp533,,,
	ldr	r9, [fp, #-128]	@, %sfp
	str	ip, [fp, #-84]	@ tmp433, stCscTbl.s32C10
	rsb	r2, r9, r2, asr #12	@, tmp493,, tmp489,
	ldr	r9, [fp, #-116]	@, %sfp
	str	r0, [fp, #-80]	@ tmp460, stCscTbl.s32C11
	rsb	r6, r9, r3, asr #12	@, tmp510,, tmp506,
	str	r1, [fp, #-76]	@ tmp476, stCscTbl.s32C12
	str	r2, [fp, #-72]	@ tmp493, stCscTbl.s32C20
	str	r6, [fp, #-68]	@ tmp510, stCscTbl.s32C21
	str	sl, [fp, #-64]	@ tmp533, stCscTbl.s32C22
.L50:
	ldr	r1, [r5, #8]	@ <variable>.bIsBGRIn, <variable>.bIsBGRIn
	cmp	r1, #0	@ <variable>.bIsBGRIn,
	beq	.L69	@,
.L52:
	ldr	r7, [fp, #-92]	@ stCscTbl.s32C01, stCscTbl.s32C01
	ldr	r5, [fp, #-96]	@ stCscTbl.s32C00, stCscTbl.s32C00
	ldr	ip, [fp, #-76]	@ stCscTbl.s32C12, stCscTbl.s32C12
	ldr	r0, [fp, #-80]	@ stCscTbl.s32C11, stCscTbl.s32C11
	ldr	r1, [fp, #-84]	@ stCscTbl.s32C10, stCscTbl.s32C10
	ldr	r2, [fp, #-64]	@ stCscTbl.s32C22, stCscTbl.s32C22
	ldr	r3, [fp, #-68]	@ stCscTbl.s32C21, stCscTbl.s32C21
	ldr	r6, [fp, #-72]	@ stCscTbl.s32C20, stCscTbl.s32C20
	ldr	r8, [fp, #-88]	@ stCscTbl.s32C02, stCscTbl.s32C02
.L64:
	str	r8, [r4, #24]	@ stCscTbl.s32C02, <variable>.s32CscCoef_00
	str	r7, [r4, #28]	@ stCscTbl.s32C01, <variable>.s32CscCoef_01
	str	r6, [r4, #56]	@ stCscTbl.s32C20, <variable>.s32CscCoef_22
	str	r5, [r4, #32]	@ stCscTbl.s32C00, <variable>.s32CscCoef_02
	str	ip, [r4, #36]	@ stCscTbl.s32C12, <variable>.s32CscCoef_10
	str	r0, [r4, #40]	@ stCscTbl.s32C11, <variable>.s32CscCoef_11
	str	r1, [r4, #44]	@ stCscTbl.s32C10, <variable>.s32CscCoef_12
	str	r2, [r4, #48]	@ stCscTbl.s32C22, <variable>.s32CscCoef_20
	str	r3, [r4, #52]	@ stCscTbl.s32C21, <variable>.s32CscCoef_21
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
.L48:
	cmp	r0, #2	@ D.2804,
	bls	.L51	@,
	ldr	r3, [r5, #20]	@ <variable>.u32Hue, <variable>.u32Hue
	movw	r2, #:lower16:1374389535	@ tmp710,
	ldr	sl, [r1, #20]	@, <variable>.s32C12
	movt	r2, #:upper16:1374389535	@ tmp710,
	ldr	r7, [r5, #24]	@ <variable>.u32Satur, <variable>.u32Satur
	movw	ip, #:lower16:.LANCHOR0	@ tmp722,
	rsb	r8, r3, r3, asl #4	@, tmp707, <variable>.u32Hue, <variable>.u32Hue,
	movt	ip, #:upper16:.LANCHOR0	@ tmp722,
	str	sl, [fp, #-132]	@, %sfp
	movw	r3, #:lower16:274877907	@ tmp732,
	mov	r9, r8, asl #2	@ tmp708, tmp707,
	mov	r7, r7, asl #1	@ s32SatAdj, <variable>.u32Satur,
	movt	r3, #:upper16:274877907	@ tmp732,
	umull	r0, r6, r2, r9	@, tmp709, tmp710, tmp708
	ldr	r0, [r1, #8]	@, <variable>.s32C02
	mul	r2, r0, r7	@,, s32SatAdj
	mov	r8, r6, lsr #5	@ u32Hue, tmp709,
	movw	r0, #:lower16:351843721	@ tmp740,
	movt	r0, #:upper16:351843721	@ tmp740,
	ldr	r6, [ip, r8, asl #2]	@ D.3128, VDPCOS_TABLE
	add	ip, ip, r8, asl #2	@, tmp726, tmp722, u32Hue,
	ldr	ip, [ip, #244]	@ D.3123, VDPSIN_TABLE
	str	r2, [fp, #-124]	@, %sfp
	movw	r2, #:lower16:1759218605	@ tmp718,
	ldr	r9, [r5, #36]	@ <variable>.u32Kb, <variable>.u32Kb
	movt	r2, #:upper16:1759218605	@ tmp718,
	ldr	sl, [r5, #16]	@ <variable>.u32Contrst, <variable>.u32Contrst
	mov	r8, r9, asl #1	@, <variable>.u32Kb,
	str	r8, [fp, #-112]	@, %sfp
	mov	sl, sl, asl #1	@, <variable>.u32Contrst,
	str	sl, [fp, #-120]	@, %sfp
	ldr	sl, [r5, #32]	@, <variable>.u32Kg
	mov	r9, sl, asl #1	@,,
	str	r9, [fp, #-136]	@, %sfp
	ldr	r8, [r1, #28]	@, <variable>.s32C21
	mul	sl, r8, r6	@,, D.3128
	str	sl, [fp, #-104]	@, %sfp
	ldr	r9, [r1, #32]	@, <variable>.s32C22
	mul	r8, r9, ip	@,, D.3123
	str	r8, [fp, #-156]	@, %sfp
	ldr	sl, [r1, #4]	@, <variable>.s32C01
	mul	r9, sl, r6	@,, D.3128
	str	r9, [fp, #-128]	@, %sfp
	ldr	r9, [fp, #-104]	@, %sfp
	ldr	r8, [r5, #28]	@, <variable>.u32Kr
	mov	sl, r8, asl #1	@,,
	str	sl, [fp, #-140]	@, %sfp
	mul	sl, r7, r9	@ tmp832, s32SatAdj,
	ldr	r9, [fp, #-124]	@, %sfp
	movw	r8, #:lower16:1125899907	@,
	str	r8, [fp, #-108]	@, %sfp
	mul	r8, ip, r9	@, D.3123,
	ldr	r9, [fp, #-156]	@, %sfp
	str	r8, [fp, #-104]	@, %sfp
	mov	r8, r9, asr #31	@,,
	str	r8, [fp, #-172]	@, %sfp
	ldr	r9, [r1, #16]	@, <variable>.s32C11
	mul	r8, r9, r6	@,, D.3128
	ldr	r9, [fp, #-128]	@, %sfp
	str	r8, [fp, #-124]	@, %sfp
	mov	r8, r9, asr #31	@,,
	ldr	r9, [fp, #-132]	@, %sfp
	str	r8, [fp, #-148]	@, %sfp
	mul	r8, r9, ip	@,, D.3123
	mov	r9, sl, asr #31	@, tmp832,
	str	r8, [fp, #-116]	@, %sfp
	str	r9, [fp, #-144]	@, %sfp
	smull	r9, r8, r0, sl	@,, tmp740, tmp832
	ldr	sl, [fp, #-104]	@, %sfp
	mov	r9, sl, asr #31	@,,
	str	r9, [fp, #-160]	@, %sfp
	str	r8, [fp, #-152]	@, %sfp
	ldr	r8, [fp, #-156]	@, %sfp
	smull	r9, sl, r3, r8	@,, tmp732,
	ldr	r8, [fp, #-124]	@, %sfp
	ldr	r9, [fp, #-112]	@, %sfp
	str	sl, [fp, #-176]	@, %sfp
	mov	sl, r8, asr #31	@,,
	ldr	r8, [fp, #-120]	@, %sfp
	str	sl, [fp, #-164]	@, %sfp
	mul	sl, r8, r9	@,,
	ldr	r8, [fp, #-152]	@, %sfp
	ldr	r9, [fp, #-144]	@, %sfp
	str	sl, [fp, #-112]	@, %sfp
	rsb	sl, r9, r8, asr #13	@,,,,
	ldr	r9, [fp, #-120]	@, %sfp
	str	sl, [fp, #-152]	@, %sfp
	ldr	r8, [fp, #-116]	@, %sfp
	mul	sl, r9, r7	@,, s32SatAdj
	mov	r9, r8, asr #31	@,,
	str	r9, [fp, #-168]	@, %sfp
	str	sl, [fp, #-156]	@, %sfp
	ldr	sl, [fp, #-128]	@, %sfp
	smull	r9, r8, r3, sl	@,, tmp732,
	ldr	r9, [fp, #-176]	@, %sfp
	ldr	sl, [fp, #-172]	@, %sfp
	str	r8, [fp, #-144]	@, %sfp
	rsb	r8, sl, r9, asr #6	@, tmp840,,,
	ldr	sl, [fp, #-104]	@, %sfp
	smull	r9, sl, r0, sl	@,, tmp740,
	str	sl, [fp, #-104]	@, %sfp
	ldr	sl, [fp, #-152]	@, %sfp
	rsb	r9, r8, sl	@, tmp840,
	ldr	r8, [fp, #-124]	@, %sfp
	str	r9, [fp, #-172]	@, %sfp
	smull	r9, sl, r3, r8	@,, tmp732,
	ldr	r8, [fp, #-144]	@, %sfp
	ldr	r9, [fp, #-148]	@, %sfp
	str	sl, [fp, #-152]	@, %sfp
	rsb	sl, r9, r8, asr #6	@,,,,
	ldr	r9, [fp, #-116]	@, %sfp
	str	sl, [fp, #-128]	@, %sfp
	smull	sl, r8, r3, r9	@,, tmp732,
	ldr	sl, [fp, #-104]	@, %sfp
	ldr	r9, [fp, #-160]	@, %sfp
	str	r8, [fp, #-144]	@, %sfp
	rsb	r8, r9, sl, asr #13	@, tmp738,,,
	ldr	sl, [fp, #-156]	@, %sfp
	ldr	r9, [fp, #-136]	@, %sfp
	mul	r9, r9, sl	@,,
	str	r9, [fp, #-124]	@, %sfp
	ldr	sl, [fp, #-152]	@, %sfp
	ldr	r9, [fp, #-164]	@, %sfp
	rsb	r9, r9, sl, asr #6	@,,,,
	ldr	sl, [fp, #-140]	@, %sfp
	str	r9, [fp, #-116]	@, %sfp
	ldr	r9, [fp, #-120]	@, %sfp
	mul	r9, r9, sl	@,,
	str	r9, [fp, #-104]	@, %sfp
	ldr	r9, [fp, #-128]	@, %sfp
	rsb	sl, r8, r9	@ tmp743, tmp738,
	ldr	r9, [fp, #-172]	@, %sfp
	ldr	r8, [fp, #-112]	@, %sfp
	mul	r8, r8, r9	@,,
	ldr	r9, [fp, #-144]	@, %sfp
	str	r8, [fp, #-148]	@, %sfp
	ldr	r8, [fp, #-168]	@, %sfp
	rsb	r8, r8, r9, asr #6	@,,,,
	ldr	r9, [fp, #-116]	@, %sfp
	str	r8, [fp, #-128]	@, %sfp
	rsb	r8, r8, r9	@,,
	movw	r9, #:lower16:1125899907	@,
	str	r8, [fp, #-116]	@, %sfp
	movt	r9, #:upper16:1125899907	@,
	str	r9, [fp, #-108]	@, %sfp
	ldr	r8, [r1, #8]	@, <variable>.s32C02
	mul	r9, r8, r6	@,, D.3128
	str	r9, [fp, #-140]	@, %sfp
	ldr	r9, [fp, #-104]	@, %sfp
	ldr	r8, [r1, #0]	@, <variable>.s32C00
	mul	sl, r9, sl	@,, tmp743
	str	r8, [fp, #-128]	@, %sfp
	ldr	r8, [fp, #-148]	@, %sfp
	mov	r9, r8, asr #31	@,,
	ldr	r8, [fp, #-124]	@, %sfp
	str	sl, [fp, #-152]	@, %sfp
	ldr	sl, [fp, #-116]	@, %sfp
	str	r9, [fp, #-160]	@, %sfp
	mul	r9, r8, sl	@ tmp796,,
	ldr	r8, [r1, #12]	@, <variable>.s32C10
	ldr	sl, [fp, #-148]	@, %sfp
	str	r8, [fp, #-144]	@, %sfp
	smull	r8, sl, r2, sl	@,, tmp718,
	ldr	r8, [fp, #-152]	@, %sfp
	str	sl, [fp, #-156]	@, %sfp
	mov	sl, r8, asr #31	@,,
	ldr	r8, [fp, #-152]	@, %sfp
	str	sl, [fp, #-116]	@, %sfp
	smull	sl, r8, r2, r8	@,, tmp718,
	str	r8, [fp, #-164]	@, %sfp
	mov	r8, r9, asr #31	@, tmp796,
	ldr	sl, [fp, #-108]	@, %sfp
	str	r8, [fp, #-148]	@, %sfp
	smull	r8, sl, sl, r9	@,,, tmp796
	ldr	r8, [fp, #-156]	@, %sfp
	ldr	r9, [fp, #-160]	@, %sfp
	str	sl, [fp, #-152]	@, %sfp
	rsb	sl, r9, r8, asr #12	@, tmp851,,,
	str	sl, [fp, #-68]	@ tmp851, stCscTbl.s32C21
	ldr	sl, [fp, #-164]	@, %sfp
	ldr	r9, [fp, #-116]	@, %sfp
	rsb	r8, r9, sl, asr #12	@, tmp749,,,
	str	r8, [fp, #-92]	@ tmp749, stCscTbl.s32C01
	ldr	sl, [r1, #4]	@, <variable>.s32C01
	str	sl, [fp, #-116]	@, %sfp
	ldr	r8, [r1, #28]	@, <variable>.s32C21
	ldr	sl, [fp, #-148]	@, %sfp
	mul	r9, r8, r7	@,, s32SatAdj
	ldr	r8, [fp, #-152]	@, %sfp
	str	r9, [fp, #-160]	@, %sfp
	rsb	r9, sl, r8, asr #18	@, tmp801,,,
	str	r9, [fp, #-80]	@ tmp801, stCscTbl.s32C11
	ldr	r8, [r1, #16]	@, <variable>.s32C11
	str	r8, [fp, #-156]	@, %sfp
	ldr	r9, [r1, #32]	@, <variable>.s32C22
	ldr	sl, [r1, #24]	@, <variable>.s32C20
	ldr	r1, [fp, #-140]	@, %sfp
	mul	r8, r9, r6	@ tmp860,, D.3128
	ldr	r9, [fp, #-160]	@, %sfp
	str	sl, [fp, #-152]	@, %sfp
	mul	r7, r7, r1	@ tmp752, s32SatAdj,
	mul	sl, ip, r9	@ tmp854, D.3123,
	ldr	r9, [fp, #-116]	@, %sfp
	mul	r1, r9, ip	@,, D.3123
	mov	r9, r8, asr #31	@, tmp860,
	str	r9, [fp, #-148]	@, %sfp
	mov	r9, sl, asr #31	@, tmp854,
	str	r1, [fp, #-168]	@, %sfp
	ldr	r1, [fp, #-132]	@, %sfp
	str	r9, [fp, #-132]	@, %sfp
	mul	r6, r1, r6	@ tmp802,, D.3128
	ldr	r1, [fp, #-156]	@, %sfp
	mul	r9, r1, ip	@ tmp809,, D.3123
	mov	ip, r7, asr #31	@, tmp752,
	str	ip, [fp, #-156]	@, %sfp
	smull	ip, r1, r0, sl	@,, tmp740, tmp854
	str	r1, [fp, #-164]	@, %sfp
	ldr	r1, [fp, #-168]	@, %sfp
	mov	ip, r1, asr #31	@,,
	str	ip, [fp, #-116]	@, %sfp
	smull	ip, sl, r3, r8	@,, tmp732, tmp860
	mov	r8, r6, asr #31	@ tmp808, tmp802,
	smull	r1, r0, r0, r7	@, tmp754, tmp740, tmp752
	mov	r1, r9, asr #31	@, tmp809,
	str	sl, [fp, #-140]	@, %sfp
	ldr	sl, [fp, #-168]	@, %sfp
	ldr	r7, [fp, #-164]	@, %sfp
	ldr	ip, [fp, #-132]	@, %sfp
	str	r1, [fp, #-160]	@, %sfp
	smull	sl, r1, r3, sl	@, tmp761, tmp732,
	rsb	sl, ip, r7, asr #13	@, tmp855,,,
	ldr	r7, [fp, #-156]	@, %sfp
	smull	ip, r6, r3, r6	@, tmp805, tmp732, tmp802
	rsb	r0, r7, r0, asr #13	@, tmp753,, tmp754,
	smull	ip, r9, r3, r9	@, tmp812, tmp732, tmp809
	ldr	r7, [fp, #-136]	@, %sfp
	ldr	ip, [fp, #-120]	@, %sfp
	rsb	r6, r8, r6, asr #6	@, tmp804, tmp808, tmp805,
	ldr	r3, [fp, #-116]	@, %sfp
	rsb	r1, r3, r1, asr #6	@, tmp760,, tmp761,
	mul	r3, r7, ip	@ tmp772,,
	ldr	ip, [fp, #-140]	@, %sfp
	ldr	r7, [fp, #-148]	@, %sfp
	add	r0, r0, r1	@ tmp765, tmp753, tmp760
	ldr	r1, [fp, #-104]	@, %sfp
	rsb	r8, r7, ip, asr #6	@, tmp862,,,
	ldr	ip, [fp, #-160]	@, %sfp
	rsb	sl, r8, sl	@ tmp867, tmp862, tmp855
	ldr	r8, [fp, #-144]	@, %sfp
	mul	r0, r1, r0	@ tmp766,, tmp765
	rsb	r7, ip, r9, asr #6	@, tmp811,, tmp812,
	mul	r1, r8, r3	@ tmp773,, tmp772
	ldr	r3, [fp, #-124]	@, %sfp
	add	ip, r6, r7	@ tmp816, tmp804, tmp811
	ldr	r9, [fp, #-104]	@, %sfp
	ldr	r8, [fp, #-128]	@, %sfp
	mul	r7, r3, ip	@ tmp817,, tmp816
	ldr	r3, [fp, #-112]	@, %sfp
	mov	r6, r0, asr #31	@, tmp766,
	str	r6, [fp, #-116]	@, %sfp
	mul	r6, r8, r9	@ tmp715,,
	ldr	r9, [fp, #-108]	@, %sfp
	mul	ip, r3, sl	@ tmp868,, tmp867
	ldr	r8, [fp, #-152]	@, %sfp
	mov	sl, r1, asr #31	@, tmp773,
	str	sl, [fp, #-120]	@, %sfp
	smull	r9, sl, r9, r7	@, tmp818,, tmp817
	mul	r3, r8, r3	@ tmp823,,
	mov	r8, r7, asr #31	@ tmp821, tmp817,
	mov	r7, r6, asr #31	@, tmp715,
	str	r7, [fp, #-112]	@, %sfp
	smull	r7, r6, r2, r6	@, tmp717, tmp718, tmp715
	rsb	r8, r8, sl, asr #18	@, tmp822, tmp821, tmp818,
	smull	r7, r9, r2, ip	@,, tmp718, tmp868
	mov	ip, ip, asr #31	@ tmp872, tmp868,
	smull	sl, r1, r2, r1	@, tmp775, tmp718, tmp773
	ldr	sl, [fp, #-112]	@, %sfp
	smull	r7, r0, r2, r0	@, tmp767, tmp718, tmp766
	rsb	r6, sl, r6, asr #12	@, tmp721,, tmp717,
	smull	sl, r2, r2, r3	@, tmp825, tmp718, tmp823
	str	r9, [fp, #-104]	@, %sfp
	mov	r9, r3, asr #31	@ tmp828, tmp823,
	ldr	sl, [fp, #-104]	@, %sfp
	ldr	r7, [fp, #-116]	@, %sfp
	str	r6, [fp, #-96]	@ tmp721, stCscTbl.s32C00
	rsb	ip, ip, sl, asr #12	@, tmp873, tmp872,,
	ldr	sl, [fp, #-120]	@, %sfp
	rsb	r0, r7, r0, asr #12	@, tmp771,, tmp767,
	str	r8, [fp, #-76]	@ tmp822, stCscTbl.s32C12
	rsb	r3, sl, r1, asr #12	@, tmp779,, tmp775,
	str	r0, [fp, #-88]	@ tmp771, stCscTbl.s32C02
	rsb	r1, r9, r2, asr #12	@, tmp829, tmp828, tmp825,
	str	r3, [fp, #-84]	@ tmp779, stCscTbl.s32C10
	str	ip, [fp, #-64]	@ tmp873, stCscTbl.s32C22
	str	r1, [fp, #-72]	@ tmp829, stCscTbl.s32C20
	ldr	r1, [r5, #8]	@ <variable>.bIsBGRIn, <variable>.bIsBGRIn
	cmp	r1, #0	@ <variable>.bIsBGRIn,
	bne	.L52	@,
.L69:
	ldr	r7, [fp, #-92]	@ stCscTbl.s32C01, stCscTbl.s32C01
	ldr	r5, [fp, #-88]	@ stCscTbl.s32C02, stCscTbl.s32C02
	ldr	ip, [fp, #-84]	@ stCscTbl.s32C10, stCscTbl.s32C10
	sub	r0, fp, #80
	ldmia	r0, {r0, r1, r2, r3}	@ phole ldm
	ldr	r6, [fp, #-64]	@ stCscTbl.s32C22, stCscTbl.s32C22
	ldr	r8, [fp, #-96]	@ stCscTbl.s32C00, stCscTbl.s32C00
	b	.L64	@
.L65:
	ldr	r9, [fp, #-52]	@ stAlgCSOut.eCsType, stAlgCSOut.eCsType
	sub	r2, r9, #1	@ tmp185, stAlgCSOut.eCsType,
	cmp	r2, #1	@ tmp185,
	bls	.L70	@,
	ldr	r2, [fp, #-56]	@ stAlgCSIn.eCsRange, stAlgCSIn.eCsRange
	cmp	r2, #1	@ stAlgCSIn.eCsRange,
	beq	.L27	@,
	mov	lr, #50	@ tmp224,
	mul	r0, lr, sl	@, tmp224, tmpBright
	bl	__aeabi_idiv	@
	mvn	ip, #127	@ tmp230,
	str	ip, [r4, #0]	@ tmp230, <variable>.s32CscDcIn_0
	str	ip, [r4, #4]	@ tmp230, <variable>.s32CscDcIn_1
	sub	r1, r0, #16	@ tmp229,,
	str	r1, [r4, #8]	@ tmp229, <variable>.s32CscDcIn_2
.L29:
	ldr	r3, [fp, #-48]	@ stAlgCSOut.eCsRange, stAlgCSOut.eCsRange
	cmp	r3, #1	@ stAlgCSOut.eCsRange,
	beq	.L58	@,
	cmp	r3, #2	@ stAlgCSOut.eCsRange,
	beq	.L71	@,
.L58:
	mov	r0, #0	@ tmp239,
	str	r0, [r4, #12]	@ tmp239, <variable>.s32CscDcOut_0
	str	r0, [r4, #20]	@ tmp239, <variable>.s32CscDcOut_2
	str	r0, [r4, #16]	@ tmp239, <variable>.s32CscDcOut_1
	b	.L25	@
.L49:
	ldr	r6, [r5, #20]	@ <variable>.u32Hue, <variable>.u32Hue
	movw	r2, #:lower16:1374389535	@ tmp542,
	ldr	r8, [r1, #24]	@, <variable>.s32C20
	movt	r2, #:upper16:1374389535	@ tmp542,
	movw	r0, #:lower16:.LANCHOR0	@ tmp570,
	movt	r0, #:upper16:.LANCHOR0	@ tmp570,
	rsb	sl, r6, r6, asl #4	@, tmp539, <variable>.u32Hue, <variable>.u32Hue,
	movw	r9, #:lower16:1759218605	@,
	str	r8, [fp, #-132]	@, %sfp
	movt	r9, #:upper16:1759218605	@,
	ldr	r7, [r1, #28]	@, <variable>.s32C21
	mov	ip, sl, asl #2	@ tmp540, tmp539,
	str	r7, [fp, #-152]	@, %sfp
	umull	r7, r8, r2, ip	@, tmp541, tmp542, tmp540
	movw	r2, #:lower16:1125899907	@ tmp593,
	ldr	r3, [r1, #32]	@, <variable>.s32C22
	movt	r2, #:upper16:1125899907	@ tmp593,
	str	r3, [fp, #-140]	@, %sfp
	movw	r3, #:lower16:274877907	@ tmp580,
	ldr	r6, [r5, #16]	@ <variable>.u32Contrst, <variable>.u32Contrst
	mov	ip, r8, lsr #5	@ u32Hue, tmp541,
	ldr	r7, [r5, #24]	@ <variable>.u32Satur, <variable>.u32Satur
	movt	r3, #:upper16:274877907	@ tmp580,
	ldr	sl, [r1, #12]	@ <variable>.s32C10, <variable>.s32C10
	mov	r8, r6, asl #1	@, <variable>.u32Contrst,
	str	r9, [fp, #-120]	@, %sfp
	mov	r6, r7, asl #1	@ s32SatAdj, <variable>.u32Satur,
	add	r7, r0, ip, asl #2	@, tmp574, tmp570, u32Hue,
	ldr	ip, [r0, ip, asl #2]	@ D.3008, VDPCOS_TABLE
	ldr	r0, [r7, #244]	@ D.3003, VDPSIN_TABLE
	str	r8, [fp, #-108]	@, %sfp
	mul	r7, sl, ip	@, <variable>.s32C10, D.3008
	ldr	r9, [r5, #32]	@ <variable>.u32Kg, <variable>.u32Kg
	mul	r6, r8, r6	@ D.2999,, s32SatAdj
	str	r7, [fp, #-148]	@, %sfp
	ldr	r8, [r1, #16]	@, <variable>.s32C11
	ldr	r7, [r5, #36]	@ <variable>.u32Kb, <variable>.u32Kb
	mul	sl, r8, ip	@,, D.3008
	str	sl, [fp, #-144]	@, %sfp
	ldr	r8, [r5, #28]	@, <variable>.u32Kr
	mov	sl, r8, asl #1	@,,
	str	sl, [fp, #-136]	@, %sfp
	ldr	r8, [r1, #20]	@, <variable>.s32C12
	mul	sl, r8, ip	@,, D.3008
	mov	r8, r9, asl #1	@, <variable>.u32Kg,
	mov	r9, r7, asl #1	@, <variable>.u32Kb,
	str	r8, [fp, #-128]	@, %sfp
	ldr	r7, [fp, #-152]	@, %sfp
	str	r9, [fp, #-116]	@, %sfp
	str	sl, [fp, #-104]	@, %sfp
	ldr	sl, [fp, #-132]	@, %sfp
	mul	r9, r7, r0	@ tmp604,, D.3003
	mul	r8, sl, r0	@ tmp583,, D.3003
	ldr	sl, [fp, #-148]	@, %sfp
	mov	r7, sl, asr #31	@,,
	ldr	sl, [fp, #-140]	@, %sfp
	str	r7, [fp, #-124]	@, %sfp
	mul	r7, sl, r0	@,, D.3003
	ldr	sl, [fp, #-144]	@, %sfp
	str	r7, [fp, #-112]	@, %sfp
	mov	r7, sl, asr #31	@,,
	ldr	sl, [fp, #-148]	@, %sfp
	str	r7, [fp, #-160]	@, %sfp
	smull	r7, sl, r3, sl	@,, tmp580,
	str	sl, [fp, #-188]	@, %sfp
	mov	sl, r8, asr #31	@, tmp583,
	str	sl, [fp, #-148]	@, %sfp
	smull	sl, r7, r3, r8	@,, tmp580, tmp583
	mov	sl, r9, asr #31	@, tmp604,
	str	sl, [fp, #-164]	@, %sfp
	str	r7, [fp, #-180]	@, %sfp
	ldr	r7, [fp, #-144]	@, %sfp
	smull	sl, r8, r3, r7	@,, tmp580,
	ldr	r7, [fp, #-104]	@, %sfp
	mov	sl, r7, asr #31	@,,
	str	sl, [fp, #-172]	@, %sfp
	str	r8, [fp, #-184]	@, %sfp
	smull	sl, r8, r3, r9	@,, tmp580, tmp604
	ldr	r7, [fp, #-112]	@, %sfp
	mov	r9, r7, asr #31	@,,
	str	r9, [fp, #-176]	@, %sfp
	str	r8, [fp, #-156]	@, %sfp
	ldr	r8, [fp, #-104]	@, %sfp
	ldr	r7, [fp, #-188]	@, %sfp
	smull	r9, sl, r3, r8	@,, tmp580,
	ldr	r8, [fp, #-124]	@, %sfp
	rsb	r9, r8, r7, asr #6	@,,,,
	str	r9, [fp, #-144]	@, %sfp
	ldr	r7, [fp, #-180]	@, %sfp
	str	sl, [fp, #-168]	@, %sfp
	ldr	sl, [fp, #-112]	@, %sfp
	smull	r9, r8, r3, sl	@,, tmp580,
	ldr	sl, [fp, #-148]	@, %sfp
	rsb	r9, sl, r7, asr #6	@,,,,
	ldr	sl, [fp, #-184]	@, %sfp
	str	r9, [fp, #-148]	@, %sfp
	str	r8, [fp, #-188]	@, %sfp
	ldr	r8, [fp, #-116]	@, %sfp
	ldr	r9, [fp, #-160]	@, %sfp
	mul	r7, r8, r6	@,, D.2999
	rsb	r8, r9, sl, asr #6	@,,,,
	ldr	r9, [fp, #-156]	@, %sfp
	str	r8, [fp, #-160]	@, %sfp
	ldr	r8, [fp, #-164]	@, %sfp
	str	r7, [fp, #-124]	@, %sfp
	ldr	r7, [fp, #-136]	@, %sfp
	mul	sl, r7, r6	@,, D.2999
	rsb	r7, r8, r9, asr #6	@,,,,
	str	r7, [fp, #-156]	@, %sfp
	ldr	r7, [fp, #-168]	@, %sfp
	str	sl, [fp, #-104]	@, %sfp
	ldr	sl, [fp, #-128]	@, %sfp
	mul	r8, sl, r6	@,, D.2999
	ldr	r6, [fp, #-172]	@, %sfp
	rsb	r9, r6, r7, asr #6	@,,,,
	str	r8, [fp, #-112]	@, %sfp
	str	r9, [fp, #-164]	@, %sfp
	ldr	sl, [fp, #-188]	@, %sfp
	ldr	r6, [fp, #-144]	@, %sfp
	ldr	r7, [fp, #-148]	@, %sfp
	ldr	r8, [fp, #-176]	@, %sfp
	rsb	r9, r8, sl, asr #6	@, tmp627,,,
	ldr	sl, [fp, #-160]	@, %sfp
	add	r8, r6, r7	@ tmp590,,
	ldr	r6, [fp, #-156]	@, %sfp
	add	r7, sl, r6	@ tmp611,,
	ldr	sl, [fp, #-164]	@, %sfp
	ldr	r6, [fp, #-104]	@, %sfp
	add	r9, sl, r9	@ tmp632,, tmp627
	ldr	sl, [r1, #0]	@, <variable>.s32C00
	mul	r8, r6, r8	@ tmp591,, tmp590
	str	sl, [fp, #-144]	@, %sfp
	ldr	r6, [fp, #-112]	@, %sfp
	ldr	sl, [r1, #4]	@, <variable>.s32C01
	mul	r7, r6, r7	@ tmp612,, tmp611
	str	sl, [fp, #-148]	@, %sfp
	ldr	r6, [fp, #-124]	@, %sfp
	ldr	sl, [r1, #8]	@, <variable>.s32C02
	mul	r9, r6, r9	@ tmp633,, tmp632
	str	sl, [fp, #-160]	@, %sfp
	smull	sl, r6, r2, r8	@,, tmp593, tmp591
	mov	r8, r8, asr #31	@ tmp595, tmp591,
	str	r6, [fp, #-164]	@, %sfp
	smull	sl, r6, r2, r7	@,, tmp593, tmp612
	mov	r7, r7, asr #31	@ tmp616, tmp612,
	str	r6, [fp, #-168]	@, %sfp
	smull	sl, r6, r2, r9	@,, tmp593, tmp633
	mov	r9, r9, asr #31	@ tmp637, tmp633,
	str	r6, [fp, #-156]	@, %sfp
	ldr	r6, [fp, #-164]	@, %sfp
	rsb	sl, r8, r6, asr #18	@, tmp596, tmp595,,
	ldr	r8, [fp, #-168]	@, %sfp
	str	sl, [fp, #-84]	@ tmp596, stCscTbl.s32C10
	ldr	sl, [fp, #-156]	@, %sfp
	rsb	r6, r7, r8, asr #18	@, tmp617, tmp616,,
	ldr	r8, [fp, #-140]	@, %sfp
	ldr	r7, [r1, #12]	@ <variable>.s32C10, <variable>.s32C10
	rsb	r9, r9, sl, asr #18	@, tmp638, tmp637,,
	str	r6, [fp, #-80]	@ tmp617, stCscTbl.s32C11
	str	r9, [fp, #-76]	@ tmp638, stCscTbl.s32C12
	ldr	r6, [r1, #16]	@ <variable>.s32C11, <variable>.s32C11
	ldr	sl, [r1, #20]	@ <variable>.s32C12, <variable>.s32C12
	ldr	r9, [fp, #-132]	@, %sfp
	mul	r1, r8, ip	@ tmp681,, D.3008
	mul	sl, sl, r0	@ tmp688, <variable>.s32C12, D.3003
	mul	r8, r9, ip	@ tmp639,, D.3008
	mov	r9, r1, asr #31	@, tmp681,
	str	r9, [fp, #-140]	@, %sfp
	ldr	r9, [fp, #-152]	@, %sfp
	mul	r7, r7, r0	@ tmp646, <variable>.s32C10, D.3003
	mul	ip, r9, ip	@ tmp660,, D.3008
	mov	r9, sl, asr #31	@, tmp688,
	mul	r0, r6, r0	@ tmp667, <variable>.s32C11, D.3003
	str	r9, [fp, #-164]	@, %sfp
	mov	r6, r8, asr #31	@, tmp639,
	str	r6, [fp, #-132]	@, %sfp
	smull	r6, r9, r3, sl	@,, tmp580, tmp688
	smull	sl, r8, r3, r8	@, tmp642, tmp580, tmp639
	mov	sl, ip, asr #31	@ tmp666, tmp660,
	str	r9, [fp, #-156]	@, %sfp
	mov	r9, r7, asr #31	@ tmp652, tmp646,
	smull	r6, r7, r3, r7	@, tmp649, tmp580, tmp646
	mov	r6, r0, asr #31	@, tmp667,
	str	r6, [fp, #-152]	@, %sfp
	smull	r6, ip, r3, ip	@, tmp663, tmp580, tmp660
	ldr	r6, [fp, #-132]	@, %sfp
	rsb	r9, r9, r7, asr #6	@,, tmp652, tmp649,
	str	r9, [fp, #-168]	@, %sfp
	rsb	r8, r6, r8, asr #6	@,,, tmp642,
	str	r8, [fp, #-132]	@, %sfp
	smull	r8, r0, r3, r0	@, tmp670, tmp580, tmp667
	rsb	ip, sl, ip, asr #6	@, tmp662, tmp666, tmp663,
	ldr	r8, [fp, #-136]	@, %sfp
	smull	r9, r3, r3, r1	@, tmp684, tmp580, tmp681
	ldr	r1, [fp, #-108]	@, %sfp
	ldr	sl, [fp, #-128]	@, %sfp
	ldr	r9, [fp, #-152]	@, %sfp
	mul	r6, r8, r1	@,,
	ldr	r7, [fp, #-156]	@, %sfp
	ldr	r8, [fp, #-164]	@, %sfp
	str	r6, [fp, #-136]	@, %sfp
	rsb	r6, r9, r0, asr #6	@, tmp669,, tmp670,
	ldr	r9, [fp, #-108]	@, %sfp
	rsb	r6, r6, ip	@ tmp674, tmp669, tmp662
	mul	r0, sl, r1	@ tmp554,,
	ldr	sl, [fp, #-116]	@, %sfp
	rsb	r1, r8, r7, asr #6	@, tmp690,,,
	ldr	r7, [fp, #-140]	@, %sfp
	ldr	ip, [fp, #-148]	@, %sfp
	mul	r8, sl, r9	@ tmp562,,
	ldr	r9, [fp, #-132]	@, %sfp
	ldr	sl, [fp, #-168]	@, %sfp
	rsb	r3, r7, r3, asr #6	@, tmp683,, tmp684,
	rsb	r3, r1, r3	@ tmp695, tmp690, tmp683
	ldr	r1, [fp, #-136]	@, %sfp
	rsb	r7, sl, r9	@ tmp653,,
	ldr	sl, [fp, #-144]	@, %sfp
	mul	r0, ip, r0	@ tmp555,, tmp554
	mul	r9, sl, r1	@ tmp547,,
	ldr	sl, [fp, #-160]	@, %sfp
	ldr	r1, [fp, #-104]	@, %sfp
	mul	r8, sl, r8	@ tmp563,, tmp562
	ldr	sl, [fp, #-112]	@, %sfp
	mov	ip, r9, asr #31	@, tmp547,
	str	ip, [fp, #-116]	@, %sfp
	ldr	ip, [fp, #-124]	@, %sfp
	mul	r7, r1, r7	@ tmp654,, tmp653
	mov	r1, r0, asr #31	@, tmp555,
	mul	r3, ip, r3	@ tmp696,, tmp695
	str	r1, [fp, #-108]	@, %sfp
	mul	r6, sl, r6	@ tmp675,, tmp674
	mov	sl, r8, asr #31	@, tmp563,
	str	sl, [fp, #-104]	@, %sfp
	mov	ip, r7, asr #31	@, tmp654,
	ldr	sl, [fp, #-120]	@, %sfp
	ldr	r1, [fp, #-120]	@, %sfp
	str	ip, [fp, #-112]	@, %sfp
	smull	ip, r9, sl, r9	@, tmp549,, tmp547
	mov	ip, r6, asr #31	@ tmp679, tmp675,
	smull	r1, r8, r1, r8	@, tmp565,, tmp563
	smull	r1, sl, r2, r3	@, tmp697, tmp593, tmp696
	mov	r3, r3, asr #31	@ tmp700, tmp696,
	ldr	r1, [fp, #-120]	@, %sfp
	smull	r1, r0, r1, r0	@, tmp557,, tmp555
	ldr	r1, [fp, #-116]	@, %sfp
	rsb	r9, r1, r9, asr #12	@, tmp553,, tmp549,
	smull	r1, r7, r2, r7	@, tmp655, tmp593, tmp654
	ldr	r1, [fp, #-104]	@, %sfp
	rsb	r8, r1, r8, asr #12	@, tmp569,, tmp565,
	smull	r1, r2, r2, r6	@, tmp676, tmp593, tmp675
	rsb	r1, r3, sl, asr #18	@, tmp701, tmp700, tmp697,
	ldr	r6, [fp, #-108]	@, %sfp
	str	r9, [fp, #-96]	@ tmp553, stCscTbl.s32C00
	ldr	r9, [fp, #-112]	@, %sfp
	rsb	r0, r6, r0, asr #12	@, tmp561,, tmp557,
	str	r8, [fp, #-88]	@ tmp569, stCscTbl.s32C02
	rsb	r7, r9, r7, asr #18	@, tmp659,, tmp655,
	rsb	r6, ip, r2, asr #18	@, tmp680, tmp679, tmp676,
	str	r0, [fp, #-92]	@ tmp561, stCscTbl.s32C01
	str	r7, [fp, #-72]	@ tmp659, stCscTbl.s32C20
	str	r1, [fp, #-64]	@ tmp701, stCscTbl.s32C22
	str	r6, [fp, #-68]	@ tmp680, stCscTbl.s32C21
	b	.L50	@
.L51:
	mov	r2, r9	@, tmp894
	mov	r0, r5	@, pstCscDrvPara
	bl	CalcCscCoef_YCbCrtoYCbCr	@
	b	.L50	@
.L70:
	ldr	r1, [fp, #-56]	@ stAlgCSIn.eCsRange, stAlgCSIn.eCsRange
	cmp	r1, #1	@ stAlgCSIn.eCsRange,
	beq	.L19	@,
	mvn	r0, #127	@ tmp195,
	mvn	r3, #15	@ tmp194,
	str	r0, [r4, #0]	@ tmp195, <variable>.s32CscDcIn_0
	stmib	r4, {r0, r3}	@ phole stm
.L21:
	ldr	r3, [fp, #-48]	@ stAlgCSOut.eCsRange, stAlgCSOut.eCsRange
	cmp	r3, #1	@ stAlgCSOut.eCsRange,
	beq	.L63	@,
	cmp	r3, #2	@ stAlgCSOut.eCsRange,
	beq	.L72	@,
.L60:
	add	lr, sl, #16	@ tmp260, tmpBright,
	mov	sl, #128	@ tmp261,
	str	lr, [r4, #20]	@ tmp260, <variable>.s32CscDcOut_2
	str	sl, [r4, #12]	@ tmp261, <variable>.s32CscDcOut_0
	str	sl, [r4, #16]	@ tmp261, <variable>.s32CscDcOut_1
	b	.L25	@
.L66:
	ldr	r3, [fp, #-56]	@ stAlgCSIn.eCsRange, stAlgCSIn.eCsRange
	cmp	r3, #1	@ stAlgCSIn.eCsRange,
	beq	.L59	@,
	cmp	r3, #2	@ stAlgCSIn.eCsRange,
	beq	.L73	@,
.L59:
	mov	r1, #0	@ tmp251,
	str	r1, [r4, #0]	@ tmp251, <variable>.s32CscDcIn_0
	str	r1, [r4, #8]	@ tmp251, <variable>.s32CscDcIn_2
	str	r1, [r4, #4]	@ tmp251, <variable>.s32CscDcIn_1
.L37:
	ldr	ip, [fp, #-48]	@ stAlgCSOut.eCsRange, stAlgCSOut.eCsRange
	cmp	ip, #1	@ stAlgCSOut.eCsRange,
	bne	.L60	@,
.L63:
	mov	r7, #128	@ tmp201,
	str	sl, [r4, #20]	@ tmp200, <variable>.s32CscDcOut_2
	str	r7, [r4, #12]	@ tmp201, <variable>.s32CscDcOut_0
	str	r7, [r4, #16]	@ tmp201, <variable>.s32CscDcOut_1
	b	.L25	@
.L68:
	movw	lr, #298	@ tmp281,
	mul	ip, lr, sl	@ tmp280, tmp281, tmpBright
	add	r9, ip, #128	@ tmp282, tmp280,
	add	r1, r9, #255	@ tmp285, tmp282,
	cmp	r9, #0	@ tmp282,
	movlt	r9, r1	@ tmp284, tmp285
	mov	r2, r9, asr #8	@ tmp286, tmp284,
	add	r7, r2, #16	@ D.2745, tmp286,
	str	r7, [r4, #12]	@ D.2745, <variable>.s32CscDcOut_0
	str	r7, [r4, #20]	@ D.2745, <variable>.s32CscDcOut_2
	str	r7, [r4, #16]	@ D.2745, <variable>.s32CscDcOut_1
	b	.L25	@
.L67:
	mov	r3, #16	@ tmp267,
	str	r3, [r4, #0]	@ tmp267, <variable>.s32CscDcIn_0
	str	r3, [r4, #8]	@ tmp267, <variable>.s32CscDcIn_2
	str	r3, [r4, #4]	@ tmp267, <variable>.s32CscDcIn_1
	b	.L44	@
.L73:
	mov	r9, #16	@ tmp248,
	str	r9, [r4, #0]	@ tmp248, <variable>.s32CscDcIn_0
	str	r9, [r4, #8]	@ tmp248, <variable>.s32CscDcIn_2
	str	r9, [r4, #4]	@ tmp248, <variable>.s32CscDcIn_1
	b	.L37	@
.L27:
	mov	r9, #50	@ tmp208,
	mul	r0, r9, sl	@, tmp208, tmpBright
	bl	__aeabi_idiv	@
	mvn	sl, #127	@ tmp212,
	str	sl, [r4, #0]	@ tmp212, <variable>.s32CscDcIn_0
	str	sl, [r4, #4]	@ tmp212, <variable>.s32CscDcIn_1
	str	r0, [r4, #8]	@, <variable>.s32CscDcIn_2
	b	.L29	@
.L71:
	mov	r3, #16	@ tmp236,
	str	r3, [r4, #12]	@ tmp236, <variable>.s32CscDcOut_0
	str	r3, [r4, #20]	@ tmp236, <variable>.s32CscDcOut_2
	str	r3, [r4, #16]	@ tmp236, <variable>.s32CscDcOut_1
	b	.L25	@
.L72:
	add	sl, sl, #16	@ tmp200, tmpBright,
	b	.L63	@
.L19:
	mvn	lr, #127	@ tmp189,
	mov	ip, #0	@ tmp188,
	str	lr, [r4, #0]	@ tmp189, <variable>.s32CscDcIn_0
	str	lr, [r4, #4]	@ tmp189, <variable>.s32CscDcIn_1
	str	ip, [r4, #8]	@ tmp188, <variable>.s32CscDcIn_2
	b	.L21	@
	.fnend
	.size	DISP_ALG_CscCoefSet, .-DISP_ALG_CscCoefSet
	.global	VDPSIN_TABLE
	.global	VDPCOS_TABLE
	.global	VDPCSCTable_YCbCr601L_to_YCbCr709L
	.global	VDPCSCTable_YCbCr709L_to_YCbCr601L
	.global	VDPCSCTable_sRGBF_to_YCbCr709L
	.global	VDPCSCTable_sRGBF_to_YCbCr601L
	.global	VDPCSCTable_YCbCr709L_to_sRGBF
	.global	VDPCSCTable_YCbCr601L_to_sRGBF
	.global	VDPCSCTable_Identity
	.global	CSC_MatrixTbl
	.section	.rodata
	.align	2
.LANCHOR0 = . + 0
	.type	VDPCOS_TABLE, %object
	.size	VDPCOS_TABLE, 244
VDPCOS_TABLE:
	.word	866
	.word	875
	.word	883
	.word	891
	.word	899
	.word	906
	.word	914
	.word	921
	.word	927
	.word	934
	.word	940
	.word	946
	.word	951
	.word	956
	.word	961
	.word	966
	.word	970
	.word	974
	.word	978
	.word	982
	.word	985
	.word	988
	.word	990
	.word	993
	.word	995
	.word	996
	.word	998
	.word	999
	.word	999
	.word	1000
	.word	1000
	.word	1000
	.word	999
	.word	999
	.word	998
	.word	996
	.word	995
	.word	993
	.word	990
	.word	988
	.word	985
	.word	982
	.word	978
	.word	974
	.word	970
	.word	966
	.word	961
	.word	956
	.word	951
	.word	946
	.word	940
	.word	934
	.word	927
	.word	921
	.word	914
	.word	906
	.word	899
	.word	891
	.word	883
	.word	875
	.word	866
	.type	VDPSIN_TABLE, %object
	.size	VDPSIN_TABLE, 244
VDPSIN_TABLE:
	.word	-500
	.word	-485
	.word	-469
	.word	-454
	.word	-438
	.word	-422
	.word	-407
	.word	-391
	.word	-374
	.word	-358
	.word	-342
	.word	-325
	.word	-309
	.word	-292
	.word	-276
	.word	-259
	.word	-242
	.word	-225
	.word	-208
	.word	-191
	.word	-174
	.word	-156
	.word	-139
	.word	-122
	.word	-104
	.word	-87
	.word	-70
	.word	-52
	.word	-35
	.word	-17
	.word	0
	.word	17
	.word	35
	.word	52
	.word	70
	.word	87
	.word	104
	.word	122
	.word	139
	.word	156
	.word	174
	.word	191
	.word	208
	.word	225
	.word	242
	.word	259
	.word	276
	.word	292
	.word	309
	.word	325
	.word	342
	.word	358
	.word	374
	.word	391
	.word	407
	.word	422
	.word	438
	.word	454
	.word	469
	.word	485
	.word	500
	.type	VDPCSCTable_YCbCr601L_to_YCbCr709L, %object
	.size	VDPCSCTable_YCbCr601L_to_YCbCr709L, 36
VDPCSCTable_YCbCr601L_to_YCbCr709L:
@ s32C00:
	.word	1024
@ s32C01:
	.word	-118
@ s32C02:
	.word	-213
@ s32C10:
	.word	0
@ s32C11:
	.word	1043
@ s32C12:
	.word	117
@ s32C20:
	.word	0
@ s32C21:
	.word	77
@ s32C22:
	.word	1050
	.type	VDPCSCTable_YCbCr709L_to_YCbCr601L, %object
	.size	VDPCSCTable_YCbCr709L_to_YCbCr601L, 36
VDPCSCTable_YCbCr709L_to_YCbCr601L:
@ s32C00:
	.word	1024
@ s32C01:
	.word	102
@ s32C02:
	.word	196
@ s32C10:
	.word	0
@ s32C11:
	.word	1014
@ s32C12:
	.word	-113
@ s32C20:
	.word	0
@ s32C21:
	.word	-74
@ s32C22:
	.word	1007
	.type	VDPCSCTable_sRGBF_to_YCbCr709L, %object
	.size	VDPCSCTable_sRGBF_to_YCbCr709L, 36
VDPCSCTable_sRGBF_to_YCbCr709L:
@ s32C00:
	.word	188
@ s32C01:
	.word	629
@ s32C02:
	.word	63
@ s32C10:
	.word	-103
@ s32C11:
	.word	-347
@ s32C12:
	.word	450
@ s32C20:
	.word	450
@ s32C21:
	.word	-409
@ s32C22:
	.word	-41
	.type	VDPCSCTable_sRGBF_to_YCbCr601L, %object
	.size	VDPCSCTable_sRGBF_to_YCbCr601L, 36
VDPCSCTable_sRGBF_to_YCbCr601L:
@ s32C00:
	.word	264
@ s32C01:
	.word	516
@ s32C02:
	.word	100
@ s32C10:
	.word	-152
@ s32C11:
	.word	-298
@ s32C12:
	.word	450
@ s32C20:
	.word	450
@ s32C21:
	.word	-377
@ s32C22:
	.word	-73
	.type	VDPCSCTable_YCbCr709L_to_sRGBF, %object
	.size	VDPCSCTable_YCbCr709L_to_sRGBF, 36
VDPCSCTable_YCbCr709L_to_sRGBF:
@ s32C00:
	.word	1192
@ s32C01:
	.word	0
@ s32C02:
	.word	1836
@ s32C10:
	.word	1192
@ s32C11:
	.word	-218
@ s32C12:
	.word	-547
@ s32C20:
	.word	1192
@ s32C21:
	.word	2166
@ s32C22:
	.word	0
	.type	VDPCSCTable_YCbCr601L_to_sRGBF, %object
	.size	VDPCSCTable_YCbCr601L_to_sRGBF, 36
VDPCSCTable_YCbCr601L_to_sRGBF:
@ s32C00:
	.word	1192
@ s32C01:
	.word	0
@ s32C02:
	.word	1634
@ s32C10:
	.word	1192
@ s32C11:
	.word	-400
@ s32C12:
	.word	-833
@ s32C20:
	.word	1192
@ s32C21:
	.word	2066
@ s32C22:
	.word	0
	.type	VDPCSCTable_Identity, %object
	.size	VDPCSCTable_Identity, 36
VDPCSCTable_Identity:
@ s32C00:
	.word	1024
@ s32C01:
	.word	0
@ s32C02:
	.word	0
@ s32C10:
	.word	0
@ s32C11:
	.word	1024
@ s32C12:
	.word	0
@ s32C20:
	.word	0
@ s32C21:
	.word	0
@ s32C22:
	.word	1024
	.data
	.align	2
.LANCHOR1 = . + 0
	.type	CSC_MatrixTbl, %object
	.size	CSC_MatrixTbl, 64
CSC_MatrixTbl:
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_YCbCr709L_to_YCbCr601L
	.word	VDPCSCTable_YCbCr709L_to_sRGBF
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_YCbCr601L_to_YCbCr709L
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_YCbCr601L_to_sRGBF
	.word	VDPCSCTable_Identity
	.word	VDPCSCTable_sRGBF_to_YCbCr709L
	.word	VDPCSCTable_sRGBF_to_YCbCr709L
	.word	VDPCSCTable_Identity
	.ident	"GCC: (Hisilicon_v200(gcc4.4-290+glibc-2.11+eabi+nptl)) 4.4.1"
	.section	.note.GNU-stack,"",%progbits
