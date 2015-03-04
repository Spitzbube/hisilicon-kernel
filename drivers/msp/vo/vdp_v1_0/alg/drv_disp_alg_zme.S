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
	.file	"drv_disp_alg_zme.c"
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
@ -DKBUILD_BASENAME=KBUILD_STR(drv_disp_alg_zme)
@ -DKBUILD_MODNAME=KBUILD_STR(hi_vou) -isystem
@ /opt/hisi-linux/x86-arm/arm-hisiv200-linux/bin/../lib/gcc/arm-hisiv200-linux-gnueabi/4.4.1/include
@ -include
@ /home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/include/linux/kconfig.h
@ -MD drivers/msp/vo/./vdp_v1_0/alg/.drv_disp_alg_zme.o.d
@ /home/x57522/X5HD/04.Locals/STBLinuxV1R2/Build/HiSTBLinuxV100R002C00SPC020/source/kernel/linux-3.4.y/drivers/msp/vo/./vdp_v1_0/alg/drv_disp_alg_zme.c
@ -mlittle-endian -marm -mapcs -mno-sched-prolog -mabi=aapcs-linux
@ -mno-thumb-interwork -march=armv7-a -msoft-float -auxbase-strip
@ drivers/msp/vo/./vdp_v1_0/alg/drv_disp_alg_zme.o -O2 -Wall -Wundef
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
	.type	VZmeTransCoefAlign, %function
VZmeTransCoefAlign:
	.fnstart
.LFB1246:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r3, r4, r5, r6, r7, r8, fp, ip, lr, pc}	@,
	.pad #4
	.save {r3, r4, r5, r6, r7, r8, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	cmp	r1, #3	@ enTap,
	bls	.L2	@,
	mov	r5, #6	@ u32Cnt,
.L4:
	mov	ip, #0	@ i,
	mov	r6, ip	@ tmp325, i
.L3:
	add	r1, r2, ip, asl #4	@, tmp187, pBitCoef, i,
	ldrh	r7, [r0, #0]	@ tmp192,* ps16Coef
	add	r3, r1, #8	@ tmp251, tmp250,
	add	ip, ip, #1	@ i, i,
	ldrh	r8, [r1, #4]	@,
	cmp	ip, r5	@ i, u32Cnt
	bfi	r8, r7, #0, #10	@ tmp189, tmp192,,
	strh	r8, [r1, #4]	@ movhi	@ tmp189,
	ldrh	r4, [r0, #2]	@ tmp200,
	ldr	r8, [r1, #4]	@ tmp198,
	bfi	r8, r4, #10, #10	@ tmp198, tmp200,,
	str	r8, [r1, #4]	@ tmp198,
	ldrh	r7, [r0, #4]	@ tmp209,
	mov	r4, r8, lsr #16	@ tmp206, tmp198,
	bfi	r4, r7, #4, #10	@ tmp206, tmp209,,
	strh	r4, [r1, #6]	@ movhi	@ tmp206,
	ldrsh	r7, [r0, #6]	@ u32Tmp,
	ldrb	r4, [r1, #7]	@ zero_extendqisi2	@,
	mov	r8, r7, lsr #2	@ tmp221, u32Tmp,
	bfi	r4, r7, #6, #2	@ tmp215, u32Tmp,,
	strb	r8, [r1, #8]	@ tmp221, <variable>.bits_38
	strb	r4, [r1, #7]	@ tmp215,
	ldr	r8, [r1, #8]	@ tmp227,
	ldrh	r4, [r0, #8]	@ tmp229,
	bfi	r8, r4, #8, #10	@ tmp227, tmp229,,
	str	r8, [r1, #8]	@ tmp227,
	ldrh	r7, [r0, #10]	@ tmp238,
	mov	r4, r8, lsr #16	@ tmp235, tmp227,
	bfi	r4, r7, #2, #10	@ tmp235, tmp238,,
	strh	r4, [r1, #10]	@ movhi	@ tmp235,
	ldrsh	r4, [r0, #12]	@ u32Tmp.475,
	ldrb	r8, [r1, #11]	@ zero_extendqisi2	@,
	bfi	r8, r4, #4, #4	@ tmp244, u32Tmp.475,,
	strb	r8, [r1, #11]	@ tmp244,
	ldrb	r1, [r3, #4]	@ zero_extendqisi2	@,
	mov	r7, r4, lsr #4	@ tmp248, u32Tmp.475,
	ldrh	r4, [r3, #6]	@,
	bfi	r1, r7, #0, #6	@ tmp253, tmp248,,
	strb	r1, [r3, #4]	@ tmp253,
	ldrh	r8, [r0, #14]	@ tmp264,
	ldrh	r7, [r3, #4]	@,
	ldrb	r1, [r3, #8]	@ zero_extendqisi2	@,
	bfi	r7, r8, #6, #10	@ tmp261, tmp264,,
	strh	r7, [r3, #4]	@ movhi	@ tmp261,
	ldrh	r7, [r0, #16]	@ tmp275,
	bfi	r4, r7, #0, #10	@ tmp272, tmp275,,
	strh	r4, [r3, #6]	@ movhi	@ tmp272,
	ldrsh	r7, [r0, #18]	@ u32Tmp.482,
	ldrb	r4, [r3, #7]	@ zero_extendqisi2	@,
	mov	r8, r7, lsr #6	@ tmp287, u32Tmp.482,
	bfi	r4, r7, #2, #6	@ tmp283, u32Tmp.482,,
	bfi	r1, r8, #0, #4	@ tmp292, tmp287,,
	strb	r4, [r3, #7]	@ tmp283,
	strb	r1, [r3, #8]	@ tmp292,
	ldrh	r4, [r0, #20]	@ tmp303,
	ldrh	r1, [r3, #8]	@,
	bfi	r1, r4, #4, #10	@ tmp300, tmp303,,
	strh	r1, [r3, #8]	@ movhi	@ tmp300,
	ldrh	r4, [r0, #22]	@ tmp313,
	add	r0, r0, #24	@ ps16Coef, ps16Coef,
	ldr	r1, [r3, #8]	@ tmp311,
	bfi	r1, r4, #14, #10	@ tmp311, tmp313,,
	str	r1, [r3, #8]	@ tmp311,
	strb	r6, [r3, #11]	@ tmp325, <variable>.bits_12
	bcc	.L3	@,
.L5:
	mov	r5, r5, asl #4	@ tmp323, u32Cnt,
	mov	r0, #0	@,
	str	r5, [r2, #0]	@ tmp323, <variable>.u32Size
	ldmfd	sp, {r3, r4, r5, r6, r7, r8, fp, sp, pc}	@
.L2:
	movw	ip, #:lower16:.LANCHOR0	@ tmp177,
	movt	ip, #:upper16:.LANCHOR0	@ tmp177,
	movw	r5, #:lower16:-1431655765	@ tmp184,
	movt	r5, #:upper16:-1431655765	@ tmp184,
	ldr	r3, [ip, r1, asl #2]	@ tmp178, CSWTCH.415
	add	ip, r3, r3, asl #3	@, tmp181, tmp178, tmp178,
	mov	r1, ip, asl #1	@ tmp182, tmp181,
	umull	ip, r3, r5, r1	@, tmp183, tmp184, tmp182
	movs	r5, r3, lsr #3	@ u32Cnt, tmp183,
	bne	.L4	@,
	b	.L5	@
	.fnend
	.size	VZmeTransCoefAlign, .-VZmeTransCoefAlign
	.align	2
	.type	GetVerticalScalerOffset, %function
GetVerticalScalerOffset:
	.fnstart
.LFB1252:
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
	ldr	r3, [r1, #20]	@ <variable>.bZmeFrmFmtIn, <variable>.bZmeFrmFmtIn
	cmp	r3, #0	@ <variable>.bZmeFrmFmtIn,
	beq	.L10	@,
	ldr	r2, [r1, #24]	@ <variable>.bZmeFrmFmtOut, <variable>.bZmeFrmFmtOut
	cmp	r2, #0	@ <variable>.bZmeFrmFmtOut,
	bne	.L11	@,
.L15:
	ldr	ip, [r1, #32]	@ <variable>.bZmeBFOut, <variable>.bZmeBFOut
	cmp	ip, #0	@ <variable>.bZmeBFOut,
	movne	r0, r0, asr #1	@ s32VOffset, u32VRatio,
	ldmnefd	sp, {fp, sp, pc}	@
.L11:
	mov	r0, #0	@ s32VOffset,
	ldmfd	sp, {fp, sp, pc}	@
.L10:
	ldr	ip, [r1, #24]	@ <variable>.bZmeFrmFmtOut, <variable>.bZmeFrmFmtOut
	cmp	ip, #0	@ <variable>.bZmeFrmFmtOut,
	bne	.L19	@,
	ldr	r3, [r1, #28]	@ <variable>.bZmeBFIn, <variable>.bZmeBFIn
	cmp	r3, #0	@ <variable>.bZmeBFIn,
	beq	.L15	@,
	ldr	r2, [r1, #32]	@ <variable>.bZmeBFOut, <variable>.bZmeBFOut
	cmp	r2, #0	@ <variable>.bZmeBFOut,
	subne	r0, r0, #4096	@ tmp144, u32VRatio,
	movne	r0, r0, asr #1	@ s32VOffset, tmp144,
	ldmnefd	sp, {fp, sp, pc}	@
	b	.L14	@
.L19:
	ldr	r0, [r1, #28]	@ <variable>.bZmeBFIn, <variable>.bZmeBFIn
	cmp	r0, #0	@ <variable>.bZmeBFIn,
	beq	.L11	@,
.L14:
	movw	r0, #:lower16:-2048	@ s32VOffset,
	movt	r0, #:upper16:-2048	@ s32VOffset,
	ldmfd	sp, {fp, sp, pc}	@
	.fnend
	.size	GetVerticalScalerOffset, .-GetVerticalScalerOffset
	.global	__aeabi_uidiv
	.align	2
	.type	GetVLCfirCoef, %function
GetVLCfirCoef:
	.fnstart
.LFB1254:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #20
	sub	sp, sp, #20	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	sub	ip, fp, #32	@ tmp137,,
	mov	r4, r0	@ pstCoefAddr, pstCoefAddr
	mov	r0, #16777216	@,
	stmia	ip, {r1, r2, r3}	@ tmp137,,,
	bl	__aeabi_uidiv	@
	ldr	r3, [fp, #-28]	@ stPickCoef.u32TapL, stPickCoef.u32TapL
	cmp	r3, #6	@ stPickCoef.u32TapL,
	beq	.L36	@,
	cmp	r0, #4096	@ tmp140,
	ldrhi	r0, [r4, #168]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_1
	bhi	.L23	@,
	ldreq	r0, [r4, #172]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_E1
	beq	.L23	@,
	cmp	r0, #3072	@ tmp140,
	ldrcs	r0, [r4, #176]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_075
	bcs	.L23	@,
	cmp	r0, #2048	@ tmp140,
	ldrcs	r0, [r4, #180]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_05
	bcs	.L23	@,
	movw	r2, #1364	@ tmp148,
	cmp	r0, r2	@ tmp140, tmp148
	ldrhi	r0, [r4, #184]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_033
	bhi	.L23	@,
	cmp	r0, #1024	@ tmp140,
	ldrcs	r0, [r4, #188]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_025
	ldrcc	r0, [r4, #192]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL4C4_0
.L23:
	sub	sp, fp, #16
	ldmfd	sp, {r4, fp, sp, pc}
.L36:
	cmp	r0, #4096	@ tmp140,
	ldrhi	r0, [r4, #140]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_1
	bhi	.L23	@,
	ldreq	r0, [r4, #144]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_E1
	beq	.L23	@,
	cmp	r0, #3072	@ tmp140,
	ldrcs	r0, [r4, #148]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_075
	bcs	.L23	@,
	cmp	r0, #2048	@ tmp140,
	ldrcs	r0, [r4, #152]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_05
	bcs	.L23	@,
	movw	r1, #1364	@ tmp144,
	cmp	r0, r1	@ tmp140, tmp144
	ldrhi	r0, [r4, #156]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_033
	bhi	.L23	@,
	cmp	r0, #1024	@ tmp140,
	ldrcs	r0, [r4, #160]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_025
	ldrcc	r0, [r4, #164]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL6C4_0
	b	.L23	@
	.fnend
	.size	GetVLCfirCoef, .-GetVLCfirCoef
	.align	2
	.type	ALG_VZmeComnSet, %function
ALG_VZmeComnSet:
	.fnstart
.LFB1259:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #12
	sub	sp, sp, #12	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	mov	sl, #1	@ tmp150,
	mov	r9, #0	@ tmp154,
	str	sl, [r1, #12]	@ tmp150, <variable>.bZmeEnVC
	str	sl, [r1, #0]	@ tmp150, <variable>.bZmeEnHL
	mov	r5, r0	@ pstZmeDrvPara, pstZmeDrvPara
	str	sl, [r1, #4]	@ tmp150, <variable>.bZmeEnHC
	mov	r4, r1	@ pstZmeRtlPara, pstZmeRtlPara
	str	sl, [r1, #8]	@ tmp150, <variable>.bZmeEnVL
	str	r9, [r1, #44]	@ tmp154, <variable>.bZmeMedVC
	str	r9, [r1, #32]	@ tmp154, <variable>.bZmeMedHL
	str	r9, [r1, #36]	@ tmp154, <variable>.bZmeMedHC
	str	r9, [r1, #40]	@ tmp154, <variable>.bZmeMedVL
	ldr	r9, [r0, #0]	@ u32ZmeCWIn, <variable>.u32ZmeFrmWIn
	str	r9, [r1, #72]	@ u32ZmeCWIn, <variable>.u32ZmeWIn
	mov	sl, r9, asl #20	@ prephitmp.566, u32ZmeCWIn,
	ldr	r6, [r0, #8]	@ u32ZmeCWOut, <variable>.u32ZmeFrmWOut
	mov	r0, sl	@, prephitmp.566
	str	r6, [r1, #80]	@ u32ZmeCWOut, <variable>.u32ZmeWOut
	mov	r1, r6	@, u32ZmeCWOut
	ldr	r8, [r5, #4]	@ u32ZmeCHIn, <variable>.u32ZmeFrmHIn
	mov	ip, r8, asl #12	@, u32ZmeCHIn,
	str	r8, [r4, #76]	@ u32ZmeCHIn, <variable>.u32ZmeHIn
	str	ip, [fp, #-48]	@, %sfp
	ldr	r7, [r5, #12]	@ u32ZmeCHOut, <variable>.u32ZmeFrmHOut
	str	r7, [r4, #84]	@ u32ZmeCHOut, <variable>.u32ZmeHOut
	str	r8, [fp, #-52]	@,
	bl	__aeabi_uidiv	@
	mov	r1, r7	@, u32ZmeCHOut
	str	r0, [r4, #88]	@, <variable>.u32ZmeRatioHL
	ldr	r0, [fp, #-48]	@, %sfp
	bl	__aeabi_uidiv	@
	str	r0, [r4, #92]	@ u32ZmeRatioVLReal, <variable>.u32ZmeRatioVL
	mov	r8, r0	@ u32ZmeRatioVLReal,
	ldrb	r1, [r5, #16]	@ zero_extendqisi2	@ D.24176, <variable>.u8ZmeYCFmtIn
	ldrb	r2, [r5, #17]	@ zero_extendqisi2	@ D.24181, <variable>.u8ZmeYCFmtOut
	cmp	r1, #2	@ D.24176,
	ldr	r0, [fp, #-52]	@,
	movne	sl, r9, lsr #1	@ tmp164, u32ZmeCWIn,
	movne	sl, sl, asl #20	@ prephitmp.566, tmp164,
	cmp	r2, #2	@ D.24181,
	movne	r6, r6, lsr #1	@ u32ZmeCWOut, u32ZmeCWOut,
	cmp	r1, #1	@ D.24176,
	moveq	r0, r0, lsr #1	@ tmp165, u32ZmeCHIn,
	mov	r1, r6	@, u32ZmeCWOut
	moveq	r0, r0, asl #12	@, tmp165,
	streq	r0, [fp, #-48]	@, %sfp
	cmp	r2, #1	@ D.24181,
	mov	r0, sl	@, prephitmp.566
	moveq	r7, r7, lsr #1	@ u32ZmeCHOut, u32ZmeCHOut,
	bl	__aeabi_uidiv	@
	mov	r1, r7	@, u32ZmeCHOut
	str	r0, [r4, #96]	@, <variable>.u32ZmeRatioHC
	ldr	r0, [fp, #-48]	@, %sfp
	bl	__aeabi_uidiv	@
	str	r0, [r4, #100]	@ u32ZmeRatioVCReal, <variable>.u32ZmeRatioVC
	mov	r6, r0	@ u32ZmeRatioVCReal,
	ldr	r3, [r5, #20]	@ <variable>.bZmeFrmFmtIn, <variable>.bZmeFrmFmtIn
	cmp	r3, #1	@ <variable>.bZmeFrmFmtIn,
	beq	.L53	@,
.L42:
	mov	r7, #0	@ tmp174,
	mov	r0, r8	@, u32ZmeRatioVLReal
	str	r7, [r4, #48]	@ tmp174, <variable>.s32ZmeOffsetHL
	mov	r1, r5	@, pstZmeDrvPara
	str	r7, [r5, #32]	@ tmp174, <variable>.bZmeBFOut
	bl	GetVerticalScalerOffset	@
	str	r0, [r4, #56]	@, <variable>.s32ZmeOffsetVL
	ldrb	r3, [r5, #16]	@ zero_extendqisi2	@ <variable>.u8ZmeYCFmtIn, <variable>.u8ZmeYCFmtIn
	cmp	r3, #1	@ <variable>.u8ZmeYCFmtIn,
	beq	.L54	@,
.L44:
	mov	lr, #0	@ tmp179,
	mov	r1, r5	@, pstZmeDrvPara
	str	lr, [r4, #52]	@ tmp179, <variable>.s32ZmeOffsetHC
	mov	r0, r6	@, u32ZmeRatioVCReal
	bl	GetVerticalScalerOffset	@
	mov	r3, #1	@ tmp181,
	mov	r1, r5	@, pstZmeDrvPara
	add	r0, r0, r7	@ tmp180,, s32VOffset
	str	r0, [r4, #60]	@ tmp180, <variable>.s32ZmeOffsetVC
	str	r3, [r5, #32]	@ tmp181, <variable>.bZmeBFOut
	mov	r0, r8	@, u32ZmeRatioVLReal
	bl	GetVerticalScalerOffset	@
	mov	r1, r5	@, pstZmeDrvPara
	str	r0, [r4, #64]	@, <variable>.s32ZmeOffsetVLBtm
	mov	r0, r6	@, u32ZmeRatioVCReal
	bl	GetVerticalScalerOffset	@
	ldr	r3, [r4, #88]	@ <variable>.u32ZmeRatioHL, <variable>.u32ZmeRatioHL
	ldr	r2, [r4, #96]	@ <variable>.u32ZmeRatioHC, <variable>.u32ZmeRatioHC
	ldr	r1, [r4, #92]	@ <variable>.u32ZmeRatioVL, <variable>.u32ZmeRatioVL
	subs	ip, r3, #1048576	@ tmp184, <variable>.u32ZmeRatioHL,
	movne	ip, #1	@ tmp184
	subs	r2, r2, #1048576	@ tmp186, <variable>.u32ZmeRatioHC,
	movne	r2, #1	@ tmp186
	str	ip, [r4, #16]	@ tmp184, <variable>.bZmeMdHL
	cmp	r1, #4096	@ <variable>.u32ZmeRatioVL,
	str	r2, [r4, #20]	@ tmp186, <variable>.bZmeMdHC
	add	r7, r0, r7	@ tmp182,, s32VOffset
	str	r7, [r4, #68]	@ tmp182, <variable>.s32ZmeOffsetVCBtm
	beq	.L55	@,
.L46:
	mov	r2, #1	@ tmp190,
.L51:
	ldr	r1, [r4, #100]	@ <variable>.u32ZmeRatioVC, <variable>.u32ZmeRatioVC
	str	r2, [r4, #24]	@ tmp190, <variable>.bZmeMdVL
	cmp	r1, #4096	@ <variable>.u32ZmeRatioVC,
	beq	.L56	@,
.L48:
	mov	r2, #1	@ tmp194,
.L52:
	str	r2, [r4, #28]	@ tmp194, <variable>.bZmeMdVC
	mov	ip, #0	@ tmp195,
	cmp	r3, #1048576	@ <variable>.u32ZmeRatioHL,
	movcs	r2, #0	@, tmp198
	movcc	r2, #1	@, tmp198
	str	ip, [r4, #108]	@ tmp195, <variable>.bZmeTapVC
	str	r2, [r4, #104]	@ tmp198, <variable>.bZmeOrder
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
.L56:
	ldr	r2, [r4, #60]	@ <variable>.s32ZmeOffsetVC, <variable>.s32ZmeOffsetVC
	cmp	r2, #0	@ <variable>.s32ZmeOffsetVC,
	bne	.L48	@,
	b	.L52	@
.L55:
	ldr	r2, [r4, #56]	@ <variable>.s32ZmeOffsetVL, <variable>.s32ZmeOffsetVL
	cmp	r2, #0	@ <variable>.s32ZmeOffsetVL,
	bne	.L46	@,
	b	.L51	@
.L54:
	ldr	r2, [r5, #20]	@ <variable>.bZmeFrmFmtIn, <variable>.bZmeFrmFmtIn
	cmp	r2, r7	@ <variable>.bZmeFrmFmtIn,
	beq	.L57	@,
.L45:
	movw	r7, #:lower16:-512	@ s32VOffset,
	movt	r7, #:upper16:-512	@ s32VOffset,
	b	.L44	@
.L53:
	ldr	lr, [r5, #24]	@ <variable>.bZmeFrmFmtOut, <variable>.bZmeFrmFmtOut
	cmp	lr, #0	@ <variable>.bZmeFrmFmtOut,
	moveq	r8, r8, asl #1	@ u32ZmeRatioVLReal, u32ZmeRatioVLReal,
	moveq	r6, r0, asl #1	@ u32ZmeRatioVCReal, u32ZmeRatioVCReal,
	b	.L42	@
.L57:
	ldr	r0, [r5, #28]	@ <variable>.bZmeBFIn, <variable>.bZmeBFIn
	cmp	r0, r7	@ <variable>.bZmeBFIn,
	movwne	r7, #:lower16:-1536	@ s32VOffset,
	movtne	r7, #:upper16:-1536	@ s32VOffset,
	bne	.L44	@,
	b	.L45	@
	.fnend
	.size	ALG_VZmeComnSet, .-ALG_VZmeComnSet
	.align	2
	.global	ALG_VZmeVdpSQSetSptAddr
	.type	ALG_VZmeVdpSQSetSptAddr, %function
ALG_VZmeVdpSQSetSptAddr:
	.fnstart
.LFB1262:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r4, r2	@ pstZmeRtlPara, pstZmeRtlPara
	mov	r5, r0	@ pstMem, pstMem
	mov	r0, r1	@, pstZmeDrvPara
	mov	r1, r2	@, pstZmeRtlPara
	bl	ALG_VZmeComnSet	@
	ldr	r1, [r4, #88]	@ <variable>.u32ZmeRatioHL, <variable>.u32ZmeRatioHL
	mov	r0, #16777216	@,
	mov	r1, r1, lsr #8	@, <variable>.u32ZmeRatioHL,
	bl	__aeabi_uidiv	@
	cmp	r0, #4096	@ tmp148,
	ldrhi	r3, [r5, #12]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_1
	bhi	.L60	@,
	ldreq	r3, [r5, #16]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_E1
	beq	.L60	@,
	cmp	r0, #3072	@ tmp148,
	ldrcs	r3, [r5, #20]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_075
	bcs	.L60	@,
	cmp	r0, #2048	@ tmp148,
	ldrcs	r3, [r5, #24]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_05
	bcs	.L60	@,
	movw	r3, #1364	@ tmp151,
	cmp	r0, r3	@ tmp148, tmp151
	ldrhi	r3, [r5, #28]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_033
	bls	.L88	@,
.L60:
	ldr	r2, [r4, #96]	@ <variable>.u32ZmeRatioHC, <variable>.u32ZmeRatioHC
	mov	r0, #16777216	@,
	str	r3, [r4, #116]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrHL
	mov	r1, r2, lsr #8	@, <variable>.u32ZmeRatioHC,
	bl	__aeabi_uidiv	@
	cmp	r0, #4096	@ tmp157,
	ldrhi	r3, [r5, #40]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_1
	bhi	.L67	@,
	ldreq	r3, [r5, #44]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_E1
	beq	.L67	@,
	cmp	r0, #3072	@ tmp157,
	ldrcs	r3, [r5, #48]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_075
	bcs	.L67	@,
	cmp	r0, #2048	@ tmp157,
	ldrcs	r3, [r5, #52]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_05
	bcs	.L67	@,
	movw	r3, #1364	@ tmp160,
	cmp	r0, r3	@ tmp157, tmp160
	ldrhi	r3, [r5, #56]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_033
	bls	.L89	@,
.L67:
	str	r3, [r4, #120]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrHC
	mov	r0, #16777216	@,
	ldr	r1, [r4, #92]	@, <variable>.u32ZmeRatioVL
	bl	__aeabi_uidiv	@
	cmp	r0, #4096	@ tmp164,
	ldrhi	r3, [r5, #208]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_1
	bhi	.L74	@,
	ldreq	r3, [r5, #212]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_E1
	beq	.L74	@,
	cmp	r0, #3072	@ tmp164,
	ldrcs	r3, [r5, #216]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_075
	bcs	.L74	@,
	cmp	r0, #2048	@ tmp164,
	ldrcs	r3, [r5, #220]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_05
	bcs	.L74	@,
	movw	r3, #1364	@ tmp167,
	cmp	r0, r3	@ tmp164, tmp167
	ldrhi	r3, [r5, #224]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_033
	bls	.L90	@,
.L74:
	str	r3, [r4, #124]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVL
	mov	r0, #16777216	@,
	ldr	r1, [r4, #100]	@, <variable>.u32ZmeRatioVC
	bl	__aeabi_uidiv	@
	cmp	r0, #4096	@ tmp171,
	ldrhi	r3, [r5, #96]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_1
	bhi	.L81	@,
	ldreq	r3, [r5, #100]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_E1
	beq	.L81	@,
	cmp	r0, #3072	@ tmp171,
	ldrcs	r3, [r5, #104]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_075
	bcs	.L81	@,
	cmp	r0, #2048	@ tmp171,
	ldrcs	r3, [r5, #108]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_05
	bcs	.L81	@,
	movw	r3, #1364	@ tmp174,
	cmp	r0, r3	@ tmp171, tmp174
	ldrhi	r3, [r5, #112]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_033
	bhi	.L81	@,
	cmp	r0, #1024	@ tmp171,
	ldrcs	r3, [r5, #116]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_025
	ldrcc	r3, [r5, #120]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_0
.L81:
	str	r3, [r4, #128]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrVC
	ldmfd	sp, {r4, r5, fp, sp, pc}	@
.L90:
	cmp	r0, #1024	@ tmp164,
	ldrcs	r3, [r5, #228]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_025
	ldrcc	r3, [r5, #232]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_0
	b	.L74	@
.L89:
	cmp	r0, #1024	@ tmp157,
	ldrcs	r3, [r5, #60]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_025
	ldrcc	r3, [r5, #64]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_0
	b	.L67	@
.L88:
	cmp	r0, #1024	@ tmp148,
	ldrcs	r3, [r5, #32]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_025
	ldrcc	r3, [r5, #36]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_0
	b	.L60	@
	.fnend
	.size	ALG_VZmeVdpSQSetSptAddr, .-ALG_VZmeVdpSQSetSptAddr
	.align	2
	.global	ALG_VZmeVdpSQSet
	.type	ALG_VZmeVdpSQSet, %function
ALG_VZmeVdpSQSet:
	.fnstart
.LFB1261:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #16
	sub	sp, sp, #16	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	sub	r5, fp, #40	@ tmp153,,
	mov	r4, r2	@ pstZmeRtlPara, pstZmeRtlPara
	mov	r7, r1	@ pstZmeDrvPara, pstZmeDrvPara
	mov	r6, r0	@ pstMem, pstMem
	mov	r1, #12	@,
	mov	r0, r5	@, tmp153
	bl	__memzero	@
	mov	r0, r7	@, pstZmeDrvPara
	mov	r1, r4	@, pstZmeRtlPara
	bl	ALG_VZmeComnSet	@
	ldr	r1, [r4, #88]	@ <variable>.u32ZmeRatioHL, <variable>.u32ZmeRatioHL
	mov	r0, #16777216	@,
	mov	r1, r1, lsr #8	@, <variable>.u32ZmeRatioHL,
	bl	__aeabi_uidiv	@
	cmp	r0, #4096	@ tmp144,
	ldrhi	r2, [r6, #124]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_1
	bhi	.L93	@,
	ldreq	r2, [r6, #128]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_E1
	beq	.L93	@,
	cmp	r0, #3072	@ tmp144,
	ldrcs	r2, [r6, #132]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_075
	bcs	.L93	@,
	cmp	r0, #2048	@ tmp144,
	ldrcs	r2, [r6, #136]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_05
	bcs	.L93	@,
	movw	r3, #1364	@ tmp147,
	cmp	r0, r3	@ tmp144, tmp147
	ldrhi	r2, [r6, #140]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_033
	bhi	.L93	@,
	cmp	r0, #1024	@ tmp144,
	ldrcs	r2, [r6, #144]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_025
	ldrcc	r2, [r6, #148]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_0
.L93:
	ldr	ip, [r4, #92]	@ <variable>.u32ZmeRatioVL, <variable>.u32ZmeRatioVL
	add	r0, r6, #12	@, pstMem,
	str	r2, [r4, #116]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrHL
	mov	r2, #4	@ tmp150,
	str	r2, [fp, #-36]	@ tmp150, stPickCoef.u32TapL
	str	ip, [fp, #-40]	@ <variable>.u32ZmeRatioVL, stPickCoef.u32Ratio
	ldmia	r5, {r1, r2, r3}	@ tmp153,,,
	bl	GetVLCfirCoef	@
	str	r0, [r4, #124]	@, <variable>.u32ZmeCoefAddrVL
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	.fnend
	.size	ALG_VZmeVdpSQSet, .-ALG_VZmeVdpSQSet
	.align	2
	.global	ALG_VZmeVdpHQSet
	.type	ALG_VZmeVdpHQSet, %function
ALG_VZmeVdpHQSet:
	.fnstart
.LFB1260:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #16
	sub	sp, sp, #16	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	sub	r5, fp, #40	@ tmp153,,
	mov	r4, r2	@ pstZmeRtlPara, pstZmeRtlPara
	mov	r7, r1	@ pstZmeDrvPara, pstZmeDrvPara
	mov	r6, r0	@ pstMem, pstMem
	mov	r1, #12	@,
	mov	r0, r5	@, tmp153
	bl	__memzero	@
	mov	r0, r7	@, pstZmeDrvPara
	mov	r1, r4	@, pstZmeRtlPara
	bl	ALG_VZmeComnSet	@
	ldr	r1, [r4, #88]	@ <variable>.u32ZmeRatioHL, <variable>.u32ZmeRatioHL
	mov	r0, #16777216	@,
	mov	r1, r1, lsr #8	@, <variable>.u32ZmeRatioHL,
	bl	__aeabi_uidiv	@
	cmp	r0, #4096	@ tmp144,
	ldrhi	r2, [r6, #124]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_1
	bhi	.L102	@,
	ldreq	r2, [r6, #128]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_E1
	beq	.L102	@,
	cmp	r0, #3072	@ tmp144,
	ldrcs	r2, [r6, #132]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_075
	bcs	.L102	@,
	cmp	r0, #2048	@ tmp144,
	ldrcs	r2, [r6, #136]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_05
	bcs	.L102	@,
	movw	r3, #1364	@ tmp147,
	cmp	r0, r3	@ tmp144, tmp147
	ldrhi	r2, [r6, #140]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_033
	bhi	.L102	@,
	cmp	r0, #1024	@ tmp144,
	ldrcs	r2, [r6, #144]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_025
	ldrcc	r2, [r6, #148]	@ u32CoefAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_0
.L102:
	ldr	ip, [r4, #92]	@ <variable>.u32ZmeRatioVL, <variable>.u32ZmeRatioVL
	add	r0, r6, #12	@, pstMem,
	str	r2, [r4, #116]	@ u32CoefAddr, <variable>.u32ZmeCoefAddrHL
	mov	r2, #6	@ tmp150,
	str	r2, [fp, #-36]	@ tmp150, stPickCoef.u32TapL
	str	ip, [fp, #-40]	@ <variable>.u32ZmeRatioVL, stPickCoef.u32Ratio
	ldmia	r5, {r1, r2, r3}	@ tmp153,,,
	bl	GetVLCfirCoef	@
	str	r0, [r4, #124]	@, <variable>.u32ZmeCoefAddrVL
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	.fnend
	.size	ALG_VZmeVdpHQSet, .-ALG_VZmeVdpHQSet
	.align	2
	.global	ALG_VZmeVdpComnDeInit
	.type	ALG_VZmeVdpComnDeInit, %function
ALG_VZmeVdpComnDeInit:
	.fnstart
.LFB1251:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r3, r4, fp, ip, lr, pc}	@,
	.pad #4
	.save {r3, r4, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	ldr	r3, [r0, #0]	@ <variable>.stMBuf.u32StartVirAddr, <variable>.stMBuf.u32StartVirAddr
	mov	r4, r0	@ pstVZmeCoefMem, pstVZmeCoefMem
	cmp	r3, #0	@ <variable>.stMBuf.u32StartVirAddr,
	ldmeqfd	sp, {r3, r4, fp, sp, pc}	@
	bl	DISP_OS_MMZ_UnmapAndRelease	@
	mov	r0, #0	@ tmp135,
	str	r0, [r4, #0]	@ tmp135, <variable>.stMBuf.u32StartVirAddr
	ldmfd	sp, {r3, r4, fp, sp, pc}	@
	.fnend
	.size	ALG_VZmeVdpComnDeInit, .-ALG_VZmeVdpComnDeInit
	.align	2
	.type	VZmeLoadVdpCoefH, %function
VZmeLoadVdpCoefH:
	.fnstart
.LFB1247:
	@ args = 0, pretend = 0, frame = 400
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #400
	sub	sp, sp, #400	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	add	r0, r0, r0, asl #2	@, tmp141, enCoefRatio, enCoefRatio,
	movw	r2, #:lower16:.LANCHOR0	@ tmp137,
	movt	r2, #:upper16:.LANCHOR0	@ tmp137,
	sub	r4, fp, #428	@ tmp146,,
	add	r7, r2, r0, asl #2	@, tmp143, tmp137, tmp141,
	add	r5, r4, #4	@ tmp150, tmp146,
	mov	r6, r1	@ pu8Addr, pu8Addr
	mov	r2, r4	@, tmp146
	mov	r1, #0	@,
	ldr	r0, [r7, #16]	@, g_pVZmeCoef
	bl	VZmeTransCoefAlign	@
	mov	r1, r5	@, tmp150
	ldr	r2, [fp, #-428]	@, stArray.u32Size
	mov	r0, r6	@, pu8Addr
	bl	memcpy	@
	ldr	r3, [fp, #-428]	@ stArray.u32Size, stArray.u32Size
	mov	r2, r4	@, tmp146
	ldr	r0, [r7, #28]	@, g_pVZmeCoef
	add	r6, r6, r3	@ pu8Addr.699, pu8Addr, stArray.u32Size
	mov	r1, #2	@,
	bl	VZmeTransCoefAlign	@
	mov	r1, r5	@, tmp150
	mov	r0, r6	@, pu8Addr.699
	ldr	r2, [fp, #-428]	@, stArray.u32Size
	bl	memcpy	@
	mov	r0, #0	@,
	sub	sp, fp, #28
	ldmfd	sp, {r4, r5, r6, r7, fp, sp, pc}
	.fnend
	.size	VZmeLoadVdpCoefH, .-VZmeLoadVdpCoefH
	.align	2
	.type	T.419, %function
T.419:
	.fnstart
.LFB1263:
	@ args = 0, pretend = 0, frame = 400
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, r8, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, r8, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #404
	sub	sp, sp, #404	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	add	r8, r0, r0, asl #2	@, tmp141, enCoefRatio, enCoefRatio,
	add	r0, r8, r1	@ tmp142, tmp141, enLumTap
	movw	r4, #:lower16:.LANCHOR0	@ tmp138,
	movt	r4, #:upper16:.LANCHOR0	@ tmp138,
	sub	r5, fp, #436	@ tmp148,,
	add	ip, r4, r0, asl #2	@, tmp145, tmp138, tmp142,
	add	r6, r5, #4	@ tmp152, tmp148,
	mov	r7, r2	@ pu8Addr, pu8Addr
	mov	r2, r5	@, tmp148
	ldr	r0, [ip, #16]	@, g_pVZmeCoef
	add	r4, r4, r8, asl #2	@, tmp164, tmp138, tmp141,
	bl	VZmeTransCoefAlign	@
	mov	r1, r6	@, tmp152
	ldr	r2, [fp, #-436]	@, stArray.u32Size
	mov	r0, r7	@, pu8Addr
	bl	memcpy	@
	ldr	r3, [fp, #-436]	@ stArray.u32Size, stArray.u32Size
	mov	r2, r5	@, tmp148
	ldr	r0, [r4, #24]	@, g_pVZmeCoef
	add	r7, r7, r3	@ pu8Addr.725, pu8Addr, stArray.u32Size
	mov	r1, #2	@,
	bl	VZmeTransCoefAlign	@
	mov	r1, r6	@, tmp152
	mov	r0, r7	@, pu8Addr.725
	ldr	r2, [fp, #-436]	@, stArray.u32Size
	bl	memcpy	@
	mov	r0, #0	@,
	sub	sp, fp, #32
	ldmfd	sp, {r4, r5, r6, r7, r8, fp, sp, pc}
	.fnend
	.size	T.419, .-T.419
	.align	2
	.global	ALG_VZmeVdpComnInit
	.type	ALG_VZmeVdpComnInit, %function
ALG_VZmeVdpComnInit:
	.fnstart
.LFB1250:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	.movsp ip
	mov	ip, sp	@,
	stmfd	sp!, {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr, pc}	@,
	.pad #4
	.save {r4, r5, r6, r7, r8, r9, sl, fp, ip, lr}
	.setfp fp, ip, #-4
	sub	fp, ip, #4	@,,
	.pad #12
	sub	sp, sp, #12	@,,
	push	{lr}
	bl	__gnu_mcount_nc
	mov	r1, #0	@,
	mov	r4, r0	@ pstVZmeCoefMem, pstVZmeCoefMem
	movw	r2, #5040	@,
	movw	r0, #:lower16:.LC0	@,
	mov	r3, #16	@,
	movt	r0, #:upper16:.LC0	@,
	str	r4, [sp, #0]	@ pstVZmeCoefMem,
	bl	DISP_OS_MMZ_AllocAndMap	@
	subs	sl, r0, #0	@ nRet,
	bne	.L120	@,
	ldr	r6, [r4, #0]	@ pu8CurAddr, <variable>.stMBuf.u32StartVirAddr
	ldr	r5, [r4, #4]	@ u32PhyAddr, <variable>.stMBuf.u32StartPhyAddr
	add	r7, r6, #4080	@ pu8CurAddr.796, pu8CurAddr,
	mov	r1, r6	@, pu8CurAddr
	add	r8, r5, #288	@ u32PhyAddr.750, u32PhyAddr,
	bl	VZmeLoadVdpCoefH	@
	add	r1, r6, #288	@, pu8CurAddr,
	add	r0, r5, #192	@ tmp180, u32PhyAddr,
	str	r5, [r4, #12]	@ u32PhyAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_1
	str	r5, [r4, #124]	@ u32PhyAddr, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_1
	add	r9, r5, #576	@ u32PhyAddr.753, u32PhyAddr,
	str	r0, [r4, #40]	@ tmp180, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_1
	mov	r0, #1	@,
	bl	VZmeLoadVdpCoefH	@
	add	ip, r5, #480	@ tmp181, u32PhyAddr,
	add	r1, r6, #576	@, pu8CurAddr,
	str	ip, [r4, #44]	@ tmp181, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_E1
	str	r8, [r4, #128]	@ u32PhyAddr.750, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_E1
	mov	r0, #2	@,
	str	r8, [r4, #16]	@ u32PhyAddr.750, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_E1
	add	r8, r5, #864	@ u32PhyAddr.756, u32PhyAddr,
	bl	VZmeLoadVdpCoefH	@
	add	r3, r5, #768	@ tmp182, u32PhyAddr,
	add	r1, r6, #864	@, pu8CurAddr,
	str	r3, [r4, #48]	@ tmp182, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_075
	str	r9, [r4, #132]	@ u32PhyAddr.753, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_075
	mov	r0, #3	@,
	str	r9, [r4, #20]	@ u32PhyAddr.753, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_075
	add	r9, r5, #1152	@ u32PhyAddr.759, u32PhyAddr,
	bl	VZmeLoadVdpCoefH	@
	add	r2, r5, #1056	@ tmp183, u32PhyAddr,
	add	r1, r6, #1152	@, pu8CurAddr,
	str	r2, [r4, #52]	@ tmp183, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_05
	str	r8, [r4, #136]	@ u32PhyAddr.756, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_05
	mov	r0, #4	@,
	str	r8, [r4, #24]	@ u32PhyAddr.756, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_05
	add	r8, r5, #1440	@ u32PhyAddr.762, u32PhyAddr,
	bl	VZmeLoadVdpCoefH	@
	add	r1, r6, #1440	@, pu8CurAddr,
	str	r9, [r4, #140]	@ u32PhyAddr.759, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_033
	mov	r0, #5	@,
	str	r9, [r4, #28]	@ u32PhyAddr.759, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_033
	add	r9, r5, #1344	@ tmp184, u32PhyAddr,
	str	r9, [r4, #56]	@ tmp184, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_033
	add	r9, r5, #1728	@ u32PhyAddr.765, u32PhyAddr,
	bl	VZmeLoadVdpCoefH	@
	add	r1, r6, #1728	@, pu8CurAddr,
	add	r0, r5, #1632	@ tmp185, u32PhyAddr,
	str	r8, [r4, #144]	@ u32PhyAddr.762, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_025
	str	r8, [r4, #32]	@ u32PhyAddr.762, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_025
	add	r8, r5, #2016	@ u32PhyAddr.768, u32PhyAddr,
	str	r0, [r4, #60]	@ tmp185, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_025
	mov	r0, #6	@,
	bl	VZmeLoadVdpCoefH	@
	add	ip, r5, #1920	@ tmp186, u32PhyAddr,
	add	r2, r6, #2016	@, pu8CurAddr,
	str	ip, [r4, #64]	@ tmp186, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHC4_0
	mov	r1, #1	@,
	str	r9, [r4, #148]	@ u32PhyAddr.765, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8C4_0
	str	r9, [r4, #36]	@ u32PhyAddr.765, <variable>.stZmeCoefAddr.u32ZmeCoefAddrHL8_0
	mov	r0, sl	@, nRet
	bl	T.419	@
	mov	r0, #1	@,
	add	r3, r5, #2160	@ tmp187, u32PhyAddr,
	add	r2, r6, #2256	@, pu8CurAddr,
	str	r3, [r4, #96]	@ tmp187, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_1
	mov	r1, r0	@,
	str	r8, [r4, #152]	@ u32PhyAddr.768, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_1
	str	r8, [r4, #68]	@ u32PhyAddr.768, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_1
	add	r8, r5, #2496	@ u32PhyAddr.774, u32PhyAddr,
	bl	T.419	@
	add	r2, r5, #2256	@ u32PhyAddr.771, u32PhyAddr,
	add	r0, r5, #2400	@ tmp188, u32PhyAddr,
	str	r2, [r4, #156]	@ u32PhyAddr.771, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_E1
	mov	r1, #1	@,
	str	r2, [r4, #72]	@ u32PhyAddr.771, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_E1
	str	r0, [r4, #100]	@ tmp188, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_E1
	add	r2, r6, #2496	@, pu8CurAddr,
	mov	r0, #2	@,
	bl	T.419	@
	add	ip, r5, #2640	@ tmp189, u32PhyAddr,
	add	r2, r6, #2736	@, pu8CurAddr,
	str	ip, [r4, #104]	@ tmp189, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_075
	str	r8, [r4, #160]	@ u32PhyAddr.774, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_075
	mov	r1, #1	@,
	str	r8, [r4, #76]	@ u32PhyAddr.774, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_075
	mov	r0, #3	@,
	bl	T.419	@
	add	r3, r5, #2736	@ u32PhyAddr.777, u32PhyAddr,
	add	r2, r6, #2976	@, pu8CurAddr,
	str	r3, [r4, #164]	@ u32PhyAddr.777, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_05
	str	r3, [r4, #80]	@ u32PhyAddr.777, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_05
	add	r0, r5, #2880	@ tmp190, u32PhyAddr,
	mov	r1, #1	@,
	str	r0, [r4, #108]	@ tmp190, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_05
	mov	r0, #4	@,
	add	r8, r5, #2976	@ u32PhyAddr.780, u32PhyAddr,
	bl	T.419	@
	add	ip, r5, #3120	@ tmp191, u32PhyAddr,
	add	r2, r6, #3216	@, pu8CurAddr,
	str	ip, [r4, #112]	@ tmp191, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_033
	str	r8, [r4, #168]	@ u32PhyAddr.780, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_033
	mov	r1, #1	@,
	str	r8, [r4, #84]	@ u32PhyAddr.780, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_033
	mov	r0, #5	@,
	bl	T.419	@
	add	r8, r5, #3360	@ tmp192, u32PhyAddr,
	add	r2, r5, #3216	@ u32PhyAddr.783, u32PhyAddr,
	mov	r1, #1	@,
	str	r2, [r4, #172]	@ u32PhyAddr.783, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_025
	mov	r0, #6	@,
	str	r2, [r4, #88]	@ u32PhyAddr.783, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_025
	add	r2, r6, #3456	@, pu8CurAddr,
	str	r8, [r4, #116]	@ tmp192, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_025
	add	r8, r5, #3456	@ u32PhyAddr.786, u32PhyAddr,
	bl	T.419	@
	add	r3, r5, #3600	@ tmp193, u32PhyAddr,
	add	r2, r6, #3696	@, pu8CurAddr,
	str	r3, [r4, #120]	@ tmp193, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_0
	mov	r1, #2	@,
	str	r8, [r4, #176]	@ u32PhyAddr.786, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6C4_0
	str	r8, [r4, #92]	@ u32PhyAddr.786, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL6_0
	mov	r0, sl	@, nRet
	bl	T.419	@
	add	r0, r5, #3696	@ u32PhyAddr.789, u32PhyAddr,
	add	r2, r6, #3888	@, pu8CurAddr,
	str	r0, [r4, #180]	@ u32PhyAddr.789, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_1
	add	r6, r5, #3792	@ tmp194, u32PhyAddr,
	mov	r1, #2	@,
	str	r0, [r4, #208]	@ u32PhyAddr.789, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_1
	mov	r0, #1	@,
	str	r6, [r4, #96]	@ tmp194, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_1
	add	r6, r5, #4080	@ u32PhyAddr.795, u32PhyAddr,
	bl	T.419	@
	mov	r0, #2	@,
	add	ip, r5, #3984	@ tmp195, u32PhyAddr,
	add	r1, r5, #3888	@ u32PhyAddr.792, u32PhyAddr,
	str	ip, [r4, #100]	@ tmp195, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_E1
	mov	r2, r7	@, pu8CurAddr.796
	str	r1, [r4, #184]	@ u32PhyAddr.792, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_E1
	add	r5, r6, #192	@ u32PhyAddr.798, u32PhyAddr.795,
	str	r1, [r4, #212]	@ u32PhyAddr.792, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_E1
	mov	r1, r0	@,
	bl	T.419	@
	add	r3, r6, #96	@ tmp196, u32PhyAddr.795,
	str	r6, [r4, #216]	@ u32PhyAddr.795, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_075
	mov	r1, #2	@,
	str	r3, [r4, #104]	@ tmp196, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_075
	add	r2, r7, #192	@, pu8CurAddr.796,
	str	r6, [r4, #188]	@ u32PhyAddr.795, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_075
	mov	r0, #3	@,
	bl	T.419	@
	add	ip, r6, #288	@ tmp197, u32PhyAddr.795,
	str	r5, [r4, #192]	@ u32PhyAddr.798, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_05
	mov	r1, #2	@,
	str	ip, [r4, #108]	@ tmp197, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_05
	add	r2, r7, #384	@, pu8CurAddr.796,
	str	r5, [r4, #220]	@ u32PhyAddr.798, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_05
	mov	r0, #4	@,
	bl	T.419	@
	add	r5, r6, #480	@ tmp198, u32PhyAddr.795,
	add	r2, r6, #384	@ u32PhyAddr.801, u32PhyAddr.795,
	mov	r1, #2	@,
	str	r2, [r4, #196]	@ u32PhyAddr.801, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_033
	mov	r0, #5	@,
	str	r2, [r4, #224]	@ u32PhyAddr.801, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_033
	add	r2, r7, #576	@, pu8CurAddr.796,
	str	r5, [r4, #112]	@ tmp198, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_033
	add	r5, r6, #576	@ u32PhyAddr.804, u32PhyAddr.795,
	bl	T.419	@
	add	r0, r6, #672	@ tmp199, u32PhyAddr.795,
	str	r5, [r4, #200]	@ u32PhyAddr.804, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_025
	add	r2, r7, #768	@, pu8CurAddr.796,
	str	r0, [r4, #116]	@ tmp199, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_025
	mov	r1, #2	@,
	str	r5, [r4, #228]	@ u32PhyAddr.804, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_025
	mov	r0, #6	@,
	bl	T.419	@
	mov	r0, sl	@ D.23934, nRet
	add	r3, r6, #864	@ tmp201, u32PhyAddr.795,
	add	r6, r6, #768	@ u32PhyAddr.807, u32PhyAddr.795,
	str	r3, [r4, #120]	@ tmp201, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVC4_0
	str	r6, [r4, #204]	@ u32PhyAddr.807, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4C4_0
	str	r6, [r4, #232]	@ u32PhyAddr.807, <variable>.stZmeCoefAddr.u32ZmeCoefAddrVL4_0
.L118:
	sub	sp, fp, #40
	ldmfd	sp, {r4, r5, r6, r7, r8, r9, sl, fp, sp, pc}
.L120:
	movw	r0, #:lower16:.LC1	@,
	movt	r0, #:upper16:.LC1	@,
	bl	printk	@
	mvn	r0, #0	@ D.23934,
	b	.L118	@
	.fnend
	.size	ALG_VZmeVdpComnInit, .-ALG_VZmeVdpComnInit
	.section	.rodata
	.align	2
.LANCHOR0 = . + 0
	.type	CSWTCH.415, %object
	.size	CSWTCH.415, 16
CSWTCH.415:
	.word	8
	.word	6
	.word	4
	.word	2
	.type	g_pVZmeCoef, %object
	.size	g_pVZmeCoef, 140
g_pVZmeCoef:
	.word	s16ZmeCoef_8T32P_Lanc3
	.word	s16ZmeCoef_6T32P_6M_a25
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_8T32P_Cubic
	.word	s16ZmeCoef_6T32P_Cubic
	.word	s16ZmeCoef_4T32P_Cubic
	.word	s16ZmeCoef_4T32P_Cubic
	.word	s16ZmeCoef_4T32P_Cubic
	.word	s16ZmeCoef_8T32P_Lanc2
	.word	s16ZmeCoef_6T32P_5M_a25
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_8T32P_Lanc2
	.word	s16ZmeCoef_6T32P_4M_a20
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_4T32P_5M_a15
	.word	s16ZmeCoef_8T32P_3M_a19
	.word	s16ZmeCoef_6T32P_3M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_8T32P_3M_a19
	.word	s16ZmeCoef_6T32P_3M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_8T32P_3M_a19
	.word	s16ZmeCoef_6T32P_3M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.word	s16ZmeCoef_4T32P_4M_a15
	.data
	.align	2
	.type	s16ZmeCoef_8T32P_Lanc3, %object
	.size	s16ZmeCoef_8T32P_Lanc3, 288
s16ZmeCoef_8T32P_Lanc3:
	.short	4
	.short	-22
	.short	40
	.short	468
	.short	40
	.short	-22
	.short	4
	.short	0
	.short	3
	.short	-18
	.short	26
	.short	468
	.short	54
	.short	-26
	.short	5
	.short	0
	.short	2
	.short	-14
	.short	14
	.short	466
	.short	68
	.short	-30
	.short	6
	.short	0
	.short	2
	.short	-11
	.short	2
	.short	462
	.short	84
	.short	-34
	.short	7
	.short	0
	.short	1
	.short	-7
	.short	-9
	.short	457
	.short	100
	.short	-38
	.short	8
	.short	0
	.short	1
	.short	-4
	.short	-18
	.short	450
	.short	116
	.short	-42
	.short	9
	.short	0
	.short	1
	.short	-2
	.short	-27
	.short	443
	.short	133
	.short	-46
	.short	10
	.short	0
	.short	0
	.short	2
	.short	-35
	.short	434
	.short	151
	.short	-50
	.short	10
	.short	0
	.short	0
	.short	4
	.short	-42
	.short	425
	.short	168
	.short	-54
	.short	11
	.short	0
	.short	0
	.short	6
	.short	-49
	.short	414
	.short	186
	.short	-57
	.short	12
	.short	0
	.short	0
	.short	8
	.short	-54
	.short	401
	.short	204
	.short	-60
	.short	13
	.short	0
	.short	0
	.short	10
	.short	-58
	.short	387
	.short	222
	.short	-62
	.short	13
	.short	0
	.short	0
	.short	11
	.short	-62
	.short	374
	.short	240
	.short	-65
	.short	14
	.short	0
	.short	0
	.short	12
	.short	-65
	.short	359
	.short	258
	.short	-66
	.short	14
	.short	0
	.short	0
	.short	13
	.short	-67
	.short	344
	.short	276
	.short	-68
	.short	14
	.short	0
	.short	0
	.short	14
	.short	-68
	.short	327
	.short	293
	.short	-68
	.short	14
	.short	0
	.short	0
	.short	14
	.short	-68
	.short	310
	.short	310
	.short	-68
	.short	14
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_6T32P_6M_a25, %object
	.size	s16ZmeCoef_6T32P_6M_a25, 216
s16ZmeCoef_6T32P_6M_a25:
	.short	-34
	.short	78
	.short	415
	.short	78
	.short	-34
	.short	9
	.short	-32
	.short	66
	.short	415
	.short	90
	.short	-36
	.short	9
	.short	-29
	.short	54
	.short	413
	.short	103
	.short	-38
	.short	9
	.short	-26
	.short	43
	.short	411
	.short	116
	.short	-41
	.short	9
	.short	-23
	.short	33
	.short	406
	.short	129
	.short	-42
	.short	9
	.short	-21
	.short	24
	.short	401
	.short	143
	.short	-44
	.short	9
	.short	-18
	.short	14
	.short	396
	.short	157
	.short	-46
	.short	9
	.short	-16
	.short	6
	.short	389
	.short	171
	.short	-47
	.short	9
	.short	-14
	.short	-2
	.short	382
	.short	185
	.short	-48
	.short	9
	.short	-11
	.short	-9
	.short	374
	.short	199
	.short	-49
	.short	8
	.short	-9
	.short	-16
	.short	365
	.short	213
	.short	-49
	.short	8
	.short	-7
	.short	-21
	.short	354
	.short	228
	.short	-49
	.short	7
	.short	-5
	.short	-27
	.short	345
	.short	242
	.short	-49
	.short	6
	.short	-3
	.short	-32
	.short	333
	.short	256
	.short	-48
	.short	6
	.short	-1
	.short	-36
	.short	322
	.short	269
	.short	-46
	.short	4
	.short	0
	.short	-39
	.short	309
	.short	283
	.short	-44
	.short	3
	.short	2
	.short	-42
	.short	296
	.short	296
	.short	-42
	.short	2
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_4T32P_5M_a15, %object
	.size	s16ZmeCoef_4T32P_5M_a15, 144
s16ZmeCoef_4T32P_5M_a15:
	.short	103
	.short	335
	.short	103
	.short	-29
	.short	92
	.short	335
	.short	112
	.short	-27
	.short	84
	.short	335
	.short	121
	.short	-28
	.short	75
	.short	334
	.short	131
	.short	-28
	.short	67
	.short	332
	.short	141
	.short	-28
	.short	59
	.short	329
	.short	152
	.short	-28
	.short	51
	.short	326
	.short	162
	.short	-27
	.short	43
	.short	323
	.short	173
	.short	-27
	.short	36
	.short	319
	.short	183
	.short	-26
	.short	30
	.short	313
	.short	194
	.short	-25
	.short	23
	.short	308
	.short	204
	.short	-23
	.short	17
	.short	301
	.short	215
	.short	-21
	.short	12
	.short	295
	.short	225
	.short	-20
	.short	6
	.short	288
	.short	235
	.short	-17
	.short	2
	.short	280
	.short	244
	.short	-14
	.short	-3
	.short	271
	.short	254
	.short	-10
	.short	-7
	.short	263
	.short	263
	.short	-7
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_8T32P_Cubic, %object
	.size	s16ZmeCoef_8T32P_Cubic, 288
s16ZmeCoef_8T32P_Cubic:
	.short	0
	.short	0
	.short	0
	.short	511
	.short	0
	.short	0
	.short	0
	.short	0
	.short	-1
	.short	3
	.short	-12
	.short	511
	.short	14
	.short	-4
	.short	1
	.short	0
	.short	-2
	.short	6
	.short	-23
	.short	509
	.short	28
	.short	-8
	.short	2
	.short	0
	.short	-2
	.short	9
	.short	-33
	.short	503
	.short	44
	.short	-12
	.short	3
	.short	0
	.short	-3
	.short	11
	.short	-41
	.short	496
	.short	61
	.short	-16
	.short	4
	.short	0
	.short	-3
	.short	13
	.short	-48
	.short	488
	.short	79
	.short	-21
	.short	5
	.short	-1
	.short	-3
	.short	14
	.short	-54
	.short	477
	.short	98
	.short	-25
	.short	7
	.short	-2
	.short	-4
	.short	16
	.short	-59
	.short	465
	.short	118
	.short	-30
	.short	8
	.short	-2
	.short	-4
	.short	17
	.short	-63
	.short	451
	.short	138
	.short	-35
	.short	9
	.short	-1
	.short	-4
	.short	18
	.short	-66
	.short	437
	.short	158
	.short	-39
	.short	10
	.short	-2
	.short	-4
	.short	18
	.short	-68
	.short	421
	.short	180
	.short	-44
	.short	11
	.short	-2
	.short	-4
	.short	18
	.short	-69
	.short	404
	.short	201
	.short	-48
	.short	13
	.short	-3
	.short	-4
	.short	18
	.short	-70
	.short	386
	.short	222
	.short	-52
	.short	14
	.short	-2
	.short	-4
	.short	18
	.short	-70
	.short	368
	.short	244
	.short	-56
	.short	15
	.short	-3
	.short	-4
	.short	18
	.short	-69
	.short	348
	.short	265
	.short	-59
	.short	16
	.short	-3
	.short	-4
	.short	18
	.short	-67
	.short	329
	.short	286
	.short	-63
	.short	16
	.short	-3
	.short	-3
	.short	17
	.short	-65
	.short	307
	.short	307
	.short	-65
	.short	17
	.short	-3
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_6T32P_Cubic, %object
	.size	s16ZmeCoef_6T32P_Cubic, 216
s16ZmeCoef_6T32P_Cubic:
	.short	0
	.short	0
	.short	511
	.short	0
	.short	0
	.short	0
	.short	3
	.short	-12
	.short	511
	.short	13
	.short	-3
	.short	0
	.short	6
	.short	-22
	.short	507
	.short	28
	.short	-7
	.short	0
	.short	8
	.short	-32
	.short	502
	.short	44
	.short	-11
	.short	1
	.short	10
	.short	-40
	.short	495
	.short	61
	.short	-15
	.short	1
	.short	11
	.short	-47
	.short	486
	.short	79
	.short	-19
	.short	2
	.short	12
	.short	-53
	.short	476
	.short	98
	.short	-24
	.short	3
	.short	13
	.short	-58
	.short	464
	.short	117
	.short	-28
	.short	4
	.short	14
	.short	-62
	.short	451
	.short	137
	.short	-33
	.short	5
	.short	15
	.short	-65
	.short	437
	.short	157
	.short	-38
	.short	6
	.short	15
	.short	-67
	.short	420
	.short	179
	.short	-42
	.short	7
	.short	15
	.short	-68
	.short	404
	.short	200
	.short	-46
	.short	7
	.short	14
	.short	-68
	.short	386
	.short	221
	.short	-50
	.short	9
	.short	14
	.short	-68
	.short	367
	.short	243
	.short	-54
	.short	10
	.short	14
	.short	-67
	.short	348
	.short	264
	.short	-58
	.short	11
	.short	13
	.short	-66
	.short	328
	.short	286
	.short	-61
	.short	12
	.short	13
	.short	-63
	.short	306
	.short	306
	.short	-63
	.short	13
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_4T32P_Cubic, %object
	.size	s16ZmeCoef_4T32P_Cubic, 144
s16ZmeCoef_4T32P_Cubic:
	.short	0
	.short	511
	.short	0
	.short	0
	.short	-19
	.short	511
	.short	21
	.short	-1
	.short	-37
	.short	509
	.short	42
	.short	-2
	.short	-51
	.short	504
	.short	64
	.short	-5
	.short	-64
	.short	499
	.short	86
	.short	-9
	.short	-74
	.short	492
	.short	108
	.short	-14
	.short	-82
	.short	484
	.short	129
	.short	-19
	.short	-89
	.short	474
	.short	152
	.short	-25
	.short	-94
	.short	463
	.short	174
	.short	-31
	.short	-97
	.short	451
	.short	196
	.short	-38
	.short	-98
	.short	438
	.short	217
	.short	-45
	.short	-98
	.short	424
	.short	238
	.short	-52
	.short	-98
	.short	409
	.short	260
	.short	-59
	.short	-95
	.short	392
	.short	280
	.short	-65
	.short	-92
	.short	376
	.short	300
	.short	-72
	.short	-88
	.short	358
	.short	320
	.short	-78
	.short	-83
	.short	339
	.short	339
	.short	-83
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_8T32P_Lanc2, %object
	.size	s16ZmeCoef_8T32P_Lanc2, 288
s16ZmeCoef_8T32P_Lanc2:
	.short	-16
	.short	0
	.short	145
	.short	254
	.short	145
	.short	0
	.short	-16
	.short	0
	.short	-16
	.short	-2
	.short	140
	.short	253
	.short	151
	.short	3
	.short	-17
	.short	0
	.short	-15
	.short	-5
	.short	135
	.short	253
	.short	157
	.short	5
	.short	-18
	.short	0
	.short	-14
	.short	-7
	.short	129
	.short	252
	.short	162
	.short	8
	.short	-18
	.short	0
	.short	-13
	.short	-9
	.short	123
	.short	252
	.short	167
	.short	11
	.short	-19
	.short	0
	.short	-13
	.short	-11
	.short	118
	.short	250
	.short	172
	.short	15
	.short	-19
	.short	0
	.short	-12
	.short	-12
	.short	112
	.short	250
	.short	177
	.short	18
	.short	-20
	.short	-1
	.short	-11
	.short	-14
	.short	107
	.short	247
	.short	183
	.short	21
	.short	-20
	.short	-1
	.short	-10
	.short	-15
	.short	101
	.short	245
	.short	188
	.short	25
	.short	-21
	.short	-1
	.short	-9
	.short	-16
	.short	96
	.short	243
	.short	192
	.short	29
	.short	-21
	.short	-2
	.short	-8
	.short	-18
	.short	90
	.short	242
	.short	197
	.short	33
	.short	-22
	.short	-2
	.short	-8
	.short	-19
	.short	85
	.short	239
	.short	202
	.short	37
	.short	-22
	.short	-2
	.short	-7
	.short	-19
	.short	80
	.short	236
	.short	206
	.short	41
	.short	-22
	.short	-3
	.short	-7
	.short	-20
	.short	75
	.short	233
	.short	210
	.short	46
	.short	-22
	.short	-3
	.short	-6
	.short	-21
	.short	69
	.short	230
	.short	215
	.short	50
	.short	-22
	.short	-3
	.short	-5
	.short	-21
	.short	65
	.short	226
	.short	219
	.short	55
	.short	-22
	.short	-5
	.short	-5
	.short	-21
	.short	60
	.short	222
	.short	222
	.short	60
	.short	-21
	.short	-5
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_6T32P_5M_a25, %object
	.size	s16ZmeCoef_6T32P_5M_a25, 216
s16ZmeCoef_6T32P_5M_a25:
	.short	-31
	.short	104
	.short	362
	.short	104
	.short	-31
	.short	4
	.short	-30
	.short	94
	.short	362
	.short	114
	.short	-32
	.short	4
	.short	-29
	.short	84
	.short	361
	.short	125
	.short	-32
	.short	3
	.short	-28
	.short	75
	.short	359
	.short	136
	.short	-33
	.short	3
	.short	-27
	.short	66
	.short	356
	.short	147
	.short	-33
	.short	3
	.short	-25
	.short	57
	.short	353
	.short	158
	.short	-33
	.short	2
	.short	-24
	.short	49
	.short	349
	.short	169
	.short	-33
	.short	2
	.short	-22
	.short	41
	.short	344
	.short	180
	.short	-32
	.short	1
	.short	-20
	.short	33
	.short	339
	.short	191
	.short	-31
	.short	0
	.short	-19
	.short	26
	.short	333
	.short	203
	.short	-30
	.short	-1
	.short	-17
	.short	19
	.short	327
	.short	214
	.short	-29
	.short	-2
	.short	-16
	.short	13
	.short	320
	.short	225
	.short	-27
	.short	-3
	.short	-14
	.short	7
	.short	312
	.short	236
	.short	-25
	.short	-4
	.short	-13
	.short	1
	.short	305
	.short	246
	.short	-22
	.short	-5
	.short	-11
	.short	-4
	.short	295
	.short	257
	.short	-19
	.short	-6
	.short	-10
	.short	-8
	.short	286
	.short	267
	.short	-16
	.short	-7
	.short	-9
	.short	-12
	.short	277
	.short	277
	.short	-12
	.short	-9
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_6T32P_4M_a20, %object
	.size	s16ZmeCoef_6T32P_4M_a20, 216
s16ZmeCoef_6T32P_4M_a20:
	.short	-20
	.short	130
	.short	297
	.short	130
	.short	-20
	.short	-5
	.short	-21
	.short	122
	.short	298
	.short	138
	.short	-19
	.short	-6
	.short	-22
	.short	115
	.short	297
	.short	146
	.short	-17
	.short	-7
	.short	-22
	.short	108
	.short	296
	.short	153
	.short	-16
	.short	-7
	.short	-23
	.short	101
	.short	295
	.short	161
	.short	-14
	.short	-8
	.short	-23
	.short	93
	.short	294
	.short	169
	.short	-12
	.short	-9
	.short	-24
	.short	87
	.short	292
	.short	177
	.short	-10
	.short	-10
	.short	-24
	.short	80
	.short	289
	.short	185
	.short	-7
	.short	-11
	.short	-24
	.short	73
	.short	286
	.short	193
	.short	-4
	.short	-12
	.short	-23
	.short	66
	.short	283
	.short	200
	.short	-1
	.short	-13
	.short	-23
	.short	60
	.short	279
	.short	208
	.short	2
	.short	-14
	.short	-23
	.short	54
	.short	276
	.short	215
	.short	5
	.short	-15
	.short	-22
	.short	48
	.short	271
	.short	222
	.short	9
	.short	-16
	.short	-21
	.short	42
	.short	266
	.short	229
	.short	13
	.short	-17
	.short	-21
	.short	37
	.short	261
	.short	236
	.short	17
	.short	-18
	.short	-21
	.short	32
	.short	255
	.short	242
	.short	22
	.short	-18
	.short	-20
	.short	27
	.short	249
	.short	249
	.short	27
	.short	-20
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_8T32P_3M_a19, %object
	.size	s16ZmeCoef_8T32P_3M_a19, 288
s16ZmeCoef_8T32P_3M_a19:
	.short	-18
	.short	18
	.short	144
	.short	226
	.short	144
	.short	19
	.short	-17
	.short	-4
	.short	-17
	.short	16
	.short	139
	.short	226
	.short	148
	.short	21
	.short	-17
	.short	-4
	.short	-17
	.short	13
	.short	135
	.short	227
	.short	153
	.short	24
	.short	-18
	.short	-5
	.short	-17
	.short	11
	.short	131
	.short	226
	.short	157
	.short	27
	.short	-18
	.short	-5
	.short	-17
	.short	9
	.short	126
	.short	225
	.short	161
	.short	30
	.short	-17
	.short	-5
	.short	-16
	.short	6
	.short	122
	.short	225
	.short	165
	.short	33
	.short	-17
	.short	-6
	.short	-16
	.short	4
	.short	118
	.short	224
	.short	169
	.short	37
	.short	-17
	.short	-7
	.short	-16
	.short	2
	.short	113
	.short	224
	.short	173
	.short	40
	.short	-17
	.short	-7
	.short	-15
	.short	0
	.short	109
	.short	222
	.short	177
	.short	43
	.short	-17
	.short	-7
	.short	-15
	.short	-1
	.short	104
	.short	220
	.short	181
	.short	47
	.short	-16
	.short	-8
	.short	-14
	.short	-3
	.short	100
	.short	218
	.short	185
	.short	51
	.short	-16
	.short	-9
	.short	-14
	.short	-5
	.short	96
	.short	217
	.short	188
	.short	54
	.short	-15
	.short	-9
	.short	-14
	.short	-6
	.short	91
	.short	214
	.short	192
	.short	58
	.short	-14
	.short	-9
	.short	-13
	.short	-7
	.short	87
	.short	212
	.short	195
	.short	62
	.short	-14
	.short	-10
	.short	-13
	.short	-9
	.short	83
	.short	210
	.short	198
	.short	66
	.short	-13
	.short	-10
	.short	-12
	.short	-10
	.short	79
	.short	207
	.short	201
	.short	70
	.short	-12
	.short	-11
	.short	-12
	.short	-11
	.short	74
	.short	205
	.short	205
	.short	74
	.short	-11
	.short	-12
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_6T32P_3M_a15, %object
	.size	s16ZmeCoef_6T32P_3M_a15, 216
s16ZmeCoef_6T32P_3M_a15:
	.short	16
	.short	136
	.short	217
	.short	136
	.short	16
	.short	-9
	.short	13
	.short	132
	.short	217
	.short	141
	.short	18
	.short	-9
	.short	11
	.short	128
	.short	217
	.short	145
	.short	21
	.short	-10
	.short	9
	.short	124
	.short	216
	.short	149
	.short	24
	.short	-10
	.short	7
	.short	119
	.short	216
	.short	153
	.short	27
	.short	-10
	.short	5
	.short	115
	.short	216
	.short	157
	.short	30
	.short	-11
	.short	3
	.short	111
	.short	215
	.short	161
	.short	33
	.short	-11
	.short	1
	.short	107
	.short	214
	.short	165
	.short	36
	.short	-11
	.short	0
	.short	102
	.short	213
	.short	169
	.short	39
	.short	-11
	.short	-2
	.short	98
	.short	211
	.short	173
	.short	43
	.short	-11
	.short	-3
	.short	94
	.short	209
	.short	177
	.short	46
	.short	-11
	.short	-4
	.short	90
	.short	207
	.short	180
	.short	50
	.short	-11
	.short	-5
	.short	85
	.short	206
	.short	184
	.short	53
	.short	-11
	.short	-6
	.short	81
	.short	203
	.short	187
	.short	57
	.short	-10
	.short	-7
	.short	77
	.short	201
	.short	190
	.short	61
	.short	-10
	.short	-8
	.short	73
	.short	198
	.short	193
	.short	65
	.short	-9
	.short	-9
	.short	69
	.short	196
	.short	196
	.short	69
	.short	-9
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.short	0
	.type	s16ZmeCoef_4T32P_4M_a15, %object
	.size	s16ZmeCoef_4T32P_4M_a15, 144
s16ZmeCoef_4T32P_4M_a15:
	.short	120
	.short	281
	.short	120
	.short	-9
	.short	113
	.short	281
	.short	127
	.short	-9
	.short	106
	.short	280
	.short	134
	.short	-8
	.short	99
	.short	279
	.short	141
	.short	-7
	.short	92
	.short	277
	.short	148
	.short	-5
	.short	85
	.short	275
	.short	156
	.short	-4
	.short	79
	.short	273
	.short	162
	.short	-2
	.short	72
	.short	270
	.short	170
	.short	0
	.short	66
	.short	267
	.short	177
	.short	2
	.short	61
	.short	263
	.short	184
	.short	4
	.short	56
	.short	259
	.short	191
	.short	6
	.short	50
	.short	255
	.short	198
	.short	9
	.short	44
	.short	251
	.short	205
	.short	12
	.short	40
	.short	246
	.short	211
	.short	15
	.short	34
	.short	241
	.short	218
	.short	19
	.short	31
	.short	235
	.short	224
	.short	22
	.short	26
	.short	230
	.short	230
	.short	26
	.short	0
	.short	0
	.short	0
	.short	0
	.section	.rodata.str1.4,"aMS",%progbits,1
	.align	2
.LC0:
	.ascii	"VDP_Zme\000"
.LC1:
	.ascii	"Get zme_buf failed\012\000"
	.ident	"GCC: (Hisilicon_v200(gcc4.4-290+glibc-2.11+eabi+nptl)) 4.4.1"
	.section	.note.GNU-stack,"",%progbits
