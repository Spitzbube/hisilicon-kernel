/*
 * drivers/cpufreq/cpufreq_interactive.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Author: Mike Chan (mike@android.com)
 *
 */

#include <linux/cpu.h>
#include <linux/cpumask.h>
#include <linux/cpufreq.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/rwsem.h>
#include <linux/sched.h>
#include <linux/tick.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/kernel_stat.h>
#include <asm/cputime.h>

#include <linux/sched/rt.h>

#define CREATE_TRACE_POINTS
#include <trace/events/cpufreq_interactive.h>

static int active_count;

struct cpufreq_interactive_cpuinfo {
	struct timer_list cpu_timer; //0
	struct timer_list cpu_slack_timer; //28
	spinlock_t load_lock; /* protects the next 4 fields */ //56
	u64 time_in_idle;
	u64 time_in_idle_timestamp;
	u64 cputime_speedadj;
	u64 cputime_speedadj_timestamp;
	struct cpufreq_policy *policy; //96
	struct cpufreq_frequency_table *freq_table;
	unsigned int target_freq; //104
	unsigned int floor_freq;
	u64 floor_validate_time;
	u64 hispeed_validate_time;
	struct rw_semaphore enable_sem; //128
	int governor_enabled;
};

typedef struct cpufreq_interactive_tunables
{
	int Data_0; //0
	unsigned int hispeed_freq; //4
	unsigned int Data_8/*go_hispeed_load*/; //8
	spinlock_t Data_12/*target_loads_lock*/; //12
	int* Data_16/*target_loads*/; //16
	int Data_20/*ntarget_loads*/; //20
	int Data_24; //24
	int Data_28/*timer_rate*/; //28
	spinlock_t Data_32; //32
	int* Data_36; //36
	int Data_40/*nabove_hispeed_delay*/; //40
	int Data_44/*boost_val*/; //44
	int Data_48/*boostpulse_duration_val*/; //48
	u64 boostpulse_endtime; //56
	int Data_64/*timer_slack_val*/; //64
	bool io_is_busy; //68
} Struct_810f3bc8;

static Struct_810f3bc8* Data_810f3bc8; //810f3bc8

//static int Data_80f9c810 = 20000; //80f9c810
//static int Data_80f9c814 = 80; //80f9c814

static DEFINE_PER_CPU(struct cpufreq_interactive_cpuinfo, cpuinfo);

/* realtime thread handles frequency scaling */
static struct task_struct *speedchange_task; //810f3be8
static cpumask_t speedchange_cpumask;
static spinlock_t speedchange_cpumask_lock; //810f3bcc
static struct mutex gov_lock;

/* Hi speed to bump to from lo speed when load burst (default max) */
static unsigned int hispeed_freq;

#if 0 //STC2HI: TODO
/* Go to hi speed when CPU load at or above this value. */
#define DEFAULT_GO_HISPEED_LOAD CONFIG_DEFAULT_GO_HISPEED_LOAD
static unsigned long go_hispeed_load = DEFAULT_GO_HISPEED_LOAD;
#endif

/* Target load.  Lower values result in higher CPU speeds. */
#define DEFAULT_TARGET_LOAD 80 //CONFIG_DEFAULT_TARGET_LOAD
static unsigned int default_target_loads[] = {DEFAULT_TARGET_LOAD}; //80f9c814
static spinlock_t target_loads_lock;
#if 0
static unsigned int *target_loads = default_target_loads;
static int ntarget_loads = ARRAY_SIZE(default_target_loads);
#endif

/*
 * The minimum amount of time to spend at a frequency before we can ramp down.
 */
#define DEFAULT_MIN_SAMPLE_TIME (80 * USEC_PER_MSEC)
static unsigned long min_sample_time = DEFAULT_MIN_SAMPLE_TIME;

/*
 * The sample rate of the timer used to increase frequency
 */
#define DEFAULT_TIMER_RATE (20 * USEC_PER_MSEC)
static unsigned long timer_rate = DEFAULT_TIMER_RATE;

/*
 * Wait this long before raising speed above hispeed, by default a single
 * timer interval.
 */
#define DEFAULT_ABOVE_HISPEED_DELAY DEFAULT_TIMER_RATE
static unsigned int default_above_hispeed_delay[] = { //80f9c810
	DEFAULT_ABOVE_HISPEED_DELAY };
static spinlock_t above_hispeed_delay_lock;
static unsigned int *above_hispeed_delay = default_above_hispeed_delay;
static int nabove_hispeed_delay = ARRAY_SIZE(default_above_hispeed_delay);

/* Non-zero means indefinite speed boost active */
static int boost_val;
/* Duration of a boot pulse in usecs */
static int boostpulse_duration_val = DEFAULT_MIN_SAMPLE_TIME;
/* End time of boost pulse in ktime converted to usecs */
static u64 boostpulse_endtime;

/*
 * Max additional time to wait in idle, beyond timer_rate, at speeds above
 * minimum before wakeup to reduce speed, or -1 if unnecessary.
 */
#define DEFAULT_TIMER_SLACK (4 * DEFAULT_TIMER_RATE)
static int timer_slack_val = DEFAULT_TIMER_SLACK;

static bool io_is_busy;

static int cpufreq_governor_interactive(struct cpufreq_policy *policy,
		unsigned int event);

#ifndef CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE
static
#endif
struct cpufreq_governor cpufreq_gov_interactive = {
	.name = "interactive",
	.governor = cpufreq_governor_interactive,
	.max_transition_latency = 10000000,
	.owner = THIS_MODULE,
};

static inline cputime64_t get_cpu_idle_time_jiffy(unsigned int cpu,
						  cputime64_t *wall)
{
	u64 idle_time;
	u64 cur_wall_time;
	u64 busy_time;

	cur_wall_time = jiffies64_to_cputime64(get_jiffies_64());

	busy_time  = kcpustat_cpu(cpu).cpustat[CPUTIME_USER];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_SYSTEM];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_IRQ];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_SOFTIRQ];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_STEAL];
	busy_time += kcpustat_cpu(cpu).cpustat[CPUTIME_NICE];

	idle_time = cur_wall_time - busy_time;
	if (wall)
		*wall = jiffies_to_usecs(cur_wall_time);

	return jiffies_to_usecs(idle_time);
}

static inline cputime64_t get_cpu_idle_time(unsigned int cpu,
					    cputime64_t *wall, bool io_is_busy)
{
	u64 idle_time = get_cpu_idle_time_us(cpu, wall);

	if (idle_time == -1ULL)
		idle_time = get_cpu_idle_time_jiffy(cpu, wall);
	else if (!io_is_busy)
		idle_time += get_cpu_iowait_time_us(cpu, wall);

	return idle_time;
}

static void cpufreq_interactive_timer_resched(
	struct cpufreq_interactive_cpuinfo *pcpu)
{
	unsigned long expires = jiffies + usecs_to_jiffies(timer_rate);
	unsigned long flags;

	mod_timer_pinned(&pcpu->cpu_timer, expires);
	if (timer_slack_val >= 0 && pcpu->target_freq > pcpu->policy->min) {
		expires += usecs_to_jiffies(timer_slack_val);
		mod_timer_pinned(&pcpu->cpu_slack_timer, expires);
	}

	spin_lock_irqsave(&pcpu->load_lock, flags);
	pcpu->time_in_idle =
		get_cpu_idle_time(smp_processor_id(),
				     &pcpu->time_in_idle_timestamp, /*TODO*/0);
	pcpu->cputime_speedadj = 0;
	pcpu->cputime_speedadj_timestamp = pcpu->time_in_idle_timestamp;
	spin_unlock_irqrestore(&pcpu->load_lock, flags);
}

#if 1
static void cpufreq_interactive_timer_start(Struct_810f3bc8* a, int cpu)
#else
static void cpufreq_interactive_timer_start(
	struct cpufreq_interactive_tunables *tunables, int cpu)
#endif
{
	struct cpufreq_interactive_cpuinfo *pcpu = &per_cpu(cpuinfo, cpu);
	unsigned long expires = jiffies +
		usecs_to_jiffies(/*tunables->timer_rate*/a->Data_28);
	unsigned long flags;

	pcpu->cpu_timer.expires = expires;
	add_timer_on(&pcpu->cpu_timer, cpu);
	if (/*tunables->timer_slack_val*/a->Data_64 >= 0 &&
	    pcpu->target_freq > pcpu->policy->min) {
		expires += usecs_to_jiffies(/*tunables->timer_slack_val*/a->Data_64);
		pcpu->cpu_slack_timer.expires = expires;
		add_timer_on(&pcpu->cpu_slack_timer, cpu);
	}

	spin_lock_irqsave(&pcpu->load_lock, flags);
	pcpu->time_in_idle =
		get_cpu_idle_time(cpu, &pcpu->time_in_idle_timestamp,
				  /*tunables*/a->io_is_busy);
	pcpu->cputime_speedadj = 0;
	pcpu->cputime_speedadj_timestamp = pcpu->time_in_idle_timestamp;
	spin_unlock_irqrestore(&pcpu->load_lock, flags);
}

static unsigned int freq_to_above_hispeed_delay(
	struct cpufreq_interactive_tunables *tunables, unsigned int freq)
{
	int i;
	unsigned int ret;
	unsigned long flags;

	spin_lock_irqsave(&tunables->Data_32/*above_hispeed_delay_lock*/, flags);

	for (i = 0; i < tunables->Data_40/*nabove_hispeed_delay*/ - 1 &&
			freq >= tunables->Data_36/*above_hispeed_delay*/[i+1]; i += 2)
		;

	ret = tunables->Data_36/*above_hispeed_delay*/[i];
	spin_unlock_irqrestore(&tunables->Data_32/*above_hispeed_delay_lock*/, flags);
	return ret;
}

static unsigned int freq_to_targetload(
	struct cpufreq_interactive_tunables *tunables, unsigned int freq)
{
	int i;
	unsigned int ret;
	unsigned long flags;

	spin_lock_irqsave(&tunables->Data_12/*target_loads_lock*/, flags);

	for (i = 0; i < tunables->Data_20/*ntarget_loads*/ - 1 &&
			freq >= tunables->Data_16/*target_loads*/[i+1]; i += 2)
		;

	ret = tunables->Data_16/*target_loads*/[i];
	spin_unlock_irqrestore(&tunables->Data_12/*target_loads_lock*/, flags);
	return ret;
}

/*
 * If increasing frequencies never map to a lower target load then
 * choose_freq() will find the minimum frequency that does not exceed its
 * target load given the current load.
 */

static unsigned int choose_freq(
	struct cpufreq_interactive_cpuinfo *pcpu, unsigned int loadadjfreq)
{
	unsigned int freq = pcpu->policy->cur;
	unsigned int prevfreq, freqmin, freqmax;
	unsigned int tl;
	int index;

	freqmin = 0;
	freqmax = UINT_MAX;

	do {
		prevfreq = freq;
		tl = freq_to_targetload(pcpu->policy->governor_data, freq);

		/*
		 * Find the lowest frequency where the computed load is less
		 * than or equal to the target load.
		 */

		if (cpufreq_frequency_table_target(
			pcpu->policy, pcpu->freq_table, loadadjfreq / tl,
			CPUFREQ_RELATION_L, &index))
			break;
		freq = pcpu->freq_table[index].frequency;

		if (freq > prevfreq) {
			/* The previous frequency is too low. */
			freqmin = prevfreq;

			if (freq >= freqmax) {
				/*
				 * Find the highest frequency that is less
				 * than freqmax.
				 */
				if (cpufreq_frequency_table_target(
					pcpu->policy, pcpu->freq_table,
					freqmax - 1, CPUFREQ_RELATION_H,
					&index))
					break;
				freq = pcpu->freq_table[index].frequency;

				if (freq == freqmin) {
					/*
					 * The first frequency below freqmax
					 * has already been found to be too
					 * low.  freqmax is the lowest speed
					 * we found that is fast enough.
					 */
					freq = freqmax;
					break;
				}
			}
		} else if (freq < prevfreq) {
			/* The previous frequency is high enough. */
			freqmax = prevfreq;

			if (freq <= freqmin) {
				/*
				 * Find the lowest frequency that is higher
				 * than freqmin.
				 */
				if (cpufreq_frequency_table_target(
					pcpu->policy, pcpu->freq_table,
					freqmin + 1, CPUFREQ_RELATION_L,
					&index))
					break;
				freq = pcpu->freq_table[index].frequency;

				/*
				 * If freqmax is the first frequency above
				 * freqmin then we have already found that
				 * this speed is fast enough.
				 */
				if (freq == freqmax)
					break;
			}
		}

		/* If same frequency chosen as previous then done. */
	} while (freq != prevfreq);

	return freq;
}

static u64 update_load(int cpu)
{
	struct cpufreq_interactive_cpuinfo *pcpu = &per_cpu(cpuinfo, cpu);
	struct cpufreq_interactive_tunables *tunables =
		pcpu->policy->governor_data;
	u64 now;
	u64 now_idle;
	unsigned int delta_idle;
	unsigned int delta_time;
	u64 active_time;

	now_idle = get_cpu_idle_time(cpu, &now, tunables->io_is_busy);
	delta_idle = (unsigned int)(now_idle - pcpu->time_in_idle);
	delta_time = (unsigned int)(now - pcpu->time_in_idle_timestamp);
	if (delta_time <= delta_idle)
		active_time = 0;
	else
		active_time = delta_time - delta_idle;
	pcpu->cputime_speedadj += active_time * pcpu->policy->cur;

	pcpu->time_in_idle = now_idle;
	pcpu->time_in_idle_timestamp = now;
	return now;
}

static void cpufreq_interactive_timer(unsigned long data)
{
#if 1 //STC2HI: TODO
	u64 now;
	unsigned int delta_time;
	u64 cputime_speedadj;
	int cpu_load;
	struct cpufreq_interactive_cpuinfo *pcpu =
		&per_cpu(cpuinfo, data);
	struct cpufreq_interactive_tunables *tunables =
		pcpu->policy->governor_data;
	unsigned int new_freq;
	unsigned int loadadjfreq;
	unsigned int index;
	unsigned long flags;
	bool boosted;

	if (!down_read_trylock(&pcpu->enable_sem))
		return;
	if (!pcpu->governor_enabled)
		goto exit;

	spin_lock_irqsave(&pcpu->load_lock, flags);
	now = update_load(data);
	delta_time = (unsigned int)(now - pcpu->cputime_speedadj_timestamp);
	cputime_speedadj = pcpu->cputime_speedadj;
	spin_unlock_irqrestore(&pcpu->load_lock, flags);

	if (WARN_ON_ONCE(!delta_time))
		goto rearm;

	do_div(cputime_speedadj, delta_time);
	loadadjfreq = (unsigned int)cputime_speedadj * 100;
	cpu_load = loadadjfreq / pcpu->target_freq;
	boosted = tunables->Data_44/*boost_val*/ || now < tunables->boostpulse_endtime;

#if 1 //STC2HI: TODO
	if (cpu_load >= tunables->Data_8/*go_hispeed_load*/ || boosted) {
		if (pcpu->target_freq < tunables->hispeed_freq) {
			new_freq = tunables->hispeed_freq;
		} else {
			new_freq = choose_freq(pcpu, loadadjfreq);

			if (new_freq < tunables->hispeed_freq)
				new_freq = tunables->hispeed_freq;
		}
	} else {
		new_freq = choose_freq(pcpu, loadadjfreq);
	}
#endif

	if (pcpu->target_freq >= tunables->hispeed_freq &&
	    new_freq > pcpu->target_freq &&
	    now - pcpu->hispeed_validate_time <
	    freq_to_above_hispeed_delay(tunables, pcpu->target_freq)) {
		trace_cpufreq_interactive_notyet(
			data, cpu_load, pcpu->target_freq,
			pcpu->policy->cur, new_freq);
		goto rearm;
	}

	pcpu->hispeed_validate_time = now;

	if (cpufreq_frequency_table_target(pcpu->policy, pcpu->freq_table,
					   new_freq, CPUFREQ_RELATION_L,
					   &index)) {
		pr_warn_once("timer %d: cpufreq_frequency_table_target error\n",
			     (int) data);
		goto rearm;
	}

	new_freq = pcpu->freq_table[index].frequency;

	/*
	 * Do not scale below floor_freq unless we have been at or above the
	 * floor frequency for the minimum sample time since last validated.
	 */
	if (new_freq < pcpu->floor_freq) {
		if (now - pcpu->floor_validate_time < min_sample_time) {
			trace_cpufreq_interactive_notyet(
				data, cpu_load, pcpu->target_freq,
				pcpu->policy->cur, new_freq);
			goto rearm;
		}
	}

	/*
	 * Update the timestamp for checking whether speed has been held at
	 * or above the selected frequency for a minimum of min_sample_time,
	 * if not boosted to hispeed_freq.  If boosted to hispeed_freq then we
	 * allow the speed to drop as soon as the boostpulse duration expires
	 * (or the indefinite boost is turned off).
	 */

	if (!boosted || new_freq > tunables->hispeed_freq) {
		pcpu->floor_freq = new_freq;
		pcpu->floor_validate_time = now;
	}

	if (pcpu->target_freq == new_freq) {
		trace_cpufreq_interactive_already(
			data, cpu_load, pcpu->target_freq,
			pcpu->policy->cur, new_freq);
		goto rearm_if_notmax;
	}

	trace_cpufreq_interactive_target(data, cpu_load, pcpu->target_freq,
					 pcpu->policy->cur, new_freq);

	pcpu->target_freq = new_freq;
	spin_lock_irqsave(&speedchange_cpumask_lock, flags);
	cpumask_set_cpu(data, &speedchange_cpumask);
	spin_unlock_irqrestore(&speedchange_cpumask_lock, flags);
	wake_up_process(speedchange_task);

rearm_if_notmax:
	/*
	 * Already set max speed and don't see a need to change that,
	 * wait until next idle to re-evaluate, don't need timer.
	 */
	if (pcpu->target_freq == pcpu->policy->max)
		goto exit;

rearm:
	if (!timer_pending(&pcpu->cpu_timer))
		cpufreq_interactive_timer_resched(pcpu);

exit:
	up_read(&pcpu->enable_sem);
#endif
	return;
}

static void cpufreq_interactive_idle_start(void)
{
	struct cpufreq_interactive_cpuinfo *pcpu =
		&per_cpu(cpuinfo, smp_processor_id());
	int pending;

	if (!down_read_trylock(&pcpu->enable_sem))
		return;
	if (!pcpu->governor_enabled) {
		up_read(&pcpu->enable_sem);
		return;
	}

	pending = timer_pending(&pcpu->cpu_timer);

	if (pcpu->target_freq != pcpu->policy->min) {
		/*
		 * Entering idle while not at lowest speed.  On some
		 * platforms this can hold the other CPU(s) at that speed
		 * even though the CPU is idle. Set a timer to re-evaluate
		 * speed so this idle CPU doesn't hold the other CPUs above
		 * min indefinitely.  This should probably be a quirk of
		 * the CPUFreq driver.
		 */
		if (!pending)
			cpufreq_interactive_timer_resched(pcpu);
	}

	up_read(&pcpu->enable_sem);
}

static void cpufreq_interactive_idle_end(void)
{
	struct cpufreq_interactive_cpuinfo *pcpu =
		&per_cpu(cpuinfo, smp_processor_id());

	if (!down_read_trylock(&pcpu->enable_sem))
		return;
	if (!pcpu->governor_enabled) {
		up_read(&pcpu->enable_sem);
		return;
	}

	/* Arm the timer for 1-2 ticks later if not already. */
	if (!timer_pending(&pcpu->cpu_timer)) {
		cpufreq_interactive_timer_resched(pcpu);
	} else if (time_after_eq(jiffies, pcpu->cpu_timer.expires)) {
		del_timer(&pcpu->cpu_timer);
		del_timer(&pcpu->cpu_slack_timer);
		cpufreq_interactive_timer(smp_processor_id());
	}

	up_read(&pcpu->enable_sem);
}

static int cpufreq_interactive_speedchange_task(void *data)
{
	unsigned int cpu;
	cpumask_t tmp_mask;
	unsigned long flags;
	struct cpufreq_interactive_cpuinfo *pcpu;

	while (1) {
		set_current_state(TASK_INTERRUPTIBLE);
		spin_lock_irqsave(&speedchange_cpumask_lock, flags);

		if (cpumask_empty(&speedchange_cpumask)) {
			spin_unlock_irqrestore(&speedchange_cpumask_lock,
					       flags);
			schedule();

			if (kthread_should_stop())
				break;

			spin_lock_irqsave(&speedchange_cpumask_lock, flags);
		}

		set_current_state(TASK_RUNNING);
		tmp_mask = speedchange_cpumask;
		cpumask_clear(&speedchange_cpumask);
		spin_unlock_irqrestore(&speedchange_cpumask_lock, flags);

		for_each_cpu(cpu, &tmp_mask) {
			unsigned int j;
			unsigned int max_freq = 0;

			pcpu = &per_cpu(cpuinfo, cpu);
			if (!down_read_trylock(&pcpu->enable_sem))
				continue;
			if (!pcpu->governor_enabled) {
				up_read(&pcpu->enable_sem);
				continue;
			}

			for_each_cpu(j, pcpu->policy->cpus) {
				struct cpufreq_interactive_cpuinfo *pjcpu =
					&per_cpu(cpuinfo, j);

				if (pjcpu->target_freq > max_freq)
					max_freq = pjcpu->target_freq;
			}

			if (max_freq != pcpu->policy->cur)
				__cpufreq_driver_target(pcpu->policy,
							max_freq,
							CPUFREQ_RELATION_H);
			trace_cpufreq_interactive_setspeed(cpu,
						     pcpu->target_freq,
						     pcpu->policy->cur);

			up_read(&pcpu->enable_sem);
		}
	}

	return 0;
}

static void cpufreq_interactive_boost(void)
{
	int i;
	int anyboost = 0;
	unsigned long flags;
	struct cpufreq_interactive_cpuinfo *pcpu;
	Struct_810f3bc8 *tunables;

	spin_lock_irqsave(&speedchange_cpumask_lock, flags);

	for_each_online_cpu(i) {
		pcpu = &per_cpu(cpuinfo, i);
		tunables = pcpu->policy->governor_data;

		if (pcpu->target_freq < /*hispeed_freq*/tunables->hispeed_freq) {
			pcpu->target_freq = /*hispeed_freq*/tunables->hispeed_freq;
			cpumask_set_cpu(i, &speedchange_cpumask);
			pcpu->hispeed_validate_time =
				ktime_to_us(ktime_get());
			anyboost = 1;
		}

		/*
		 * Set floor freq and (re)start timer for when last
		 * validated.
		 */

		pcpu->floor_freq = /*hispeed_freq*/tunables->hispeed_freq;
		pcpu->floor_validate_time = ktime_to_us(ktime_get());
	}

	spin_unlock_irqrestore(&speedchange_cpumask_lock, flags);

	if (anyboost)
		wake_up_process(speedchange_task);
}

static int cpufreq_interactive_notifier(
	struct notifier_block *nb, unsigned long val, void *data)
{
	struct cpufreq_freqs *freq = data;
	struct cpufreq_interactive_cpuinfo *pcpu;
	int cpu;
	unsigned long flags;

	if (val == CPUFREQ_POSTCHANGE) {
		pcpu = &per_cpu(cpuinfo, freq->cpu);
		if (!down_read_trylock(&pcpu->enable_sem))
			return 0;
		if (!pcpu->governor_enabled) {
			up_read(&pcpu->enable_sem);
			return 0;
		}

		for_each_cpu(cpu, pcpu->policy->cpus) {
			struct cpufreq_interactive_cpuinfo *pjcpu =
				&per_cpu(cpuinfo, cpu);
			if (cpu != freq->cpu) {
				if (!down_read_trylock(&pjcpu->enable_sem))
					continue;
				if (!pjcpu->governor_enabled) {
					up_read(&pjcpu->enable_sem);
					continue;
				}
			}
			spin_lock_irqsave(&pjcpu->load_lock, flags);
			update_load(cpu);
			spin_unlock_irqrestore(&pjcpu->load_lock, flags);
			if (cpu != freq->cpu)
				up_read(&pjcpu->enable_sem);
		}

		up_read(&pcpu->enable_sem);
	}
	return 0;
}

static struct notifier_block cpufreq_notifier_block = {
	.notifier_call = cpufreq_interactive_notifier,
};

static unsigned int *get_tokenized_data(const char *buf, int *num_tokens)
{
	const char *cp;
	int i;
	int ntokens = 1;
	unsigned int *tokenized_data;
	int err = -EINVAL;

	cp = buf;
	while ((cp = strpbrk(cp + 1, " :")))
		ntokens++;

	if (!(ntokens & 0x1))
		goto err;

	tokenized_data = kmalloc(ntokens * sizeof(unsigned int), GFP_KERNEL);
	if (!tokenized_data) {
		err = -ENOMEM;
		goto err;
	}

	cp = buf;
	i = 0;
	while (i < ntokens) {
		if (sscanf(cp, "%u", &tokenized_data[i++]) != 1)
			goto err_kfree;

		cp = strpbrk(cp, " :");
		if (!cp)
			break;
		cp++;
	}

	if (i != ntokens)
		goto err_kfree;

	*num_tokens = ntokens;
	return tokenized_data;

err_kfree:
	kfree(tokenized_data);
err:
	return ERR_PTR(err);
}

#if 0
static ssize_t show_target_loads(
	struct kobject *kobj, struct attribute *attr, char *buf)
#else
static ssize_t show_target_loads(Struct_810f3bc8* a, char *buf)
#endif
{
	int i;
	ssize_t ret = 0;
	unsigned long flags;

#if 0
	spin_lock_irqsave(&target_loads_lock, flags);

	for (i = 0; i < ntarget_loads; i++)
		ret += sprintf(buf + ret, "%u%s", target_loads[i],
			       i & 0x1 ? ":" : " ");

	ret += sprintf(buf + ret, "\n");
	spin_unlock_irqrestore(&target_loads_lock, flags);
#else
	spin_lock_irqsave(&a->Data_12, flags);

	for (i = 0; i < a->Data_20; i++)
		ret += sprintf(buf + ret, "%u%s", a->Data_16[i],
			       i & 0x1 ? ":" : " ");

	ret += sprintf(buf + ret-1, "\n");
	spin_unlock_irqrestore(&a->Data_12, flags);
#endif
	return ret-1;
}

ssize_t show_target_loads_gov_pol(struct cpufreq_policy *policy,
	/*struct kobject *kobj, struct attribute *attr,*/ const char *buf)
{
	return show_target_loads(policy->governor_data, buf);
}

ssize_t show_target_loads_gov_sys(
	struct kobject *kobj, struct attribute *attr, const char *buf)
{
	return show_target_loads(Data_810f3bc8, buf);
}

#if 0
static ssize_t store_target_loads(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
#else
static ssize_t store_target_loads(Struct_810f3bc8* a, const char *buf, size_t count)
#endif
{
	int ntokens;
	unsigned int *new_target_loads = NULL;
	unsigned long flags;

	new_target_loads = get_tokenized_data(buf, &ntokens);
	if (IS_ERR(new_target_loads))
		return PTR_RET(new_target_loads);

#if 0 //STC2HI: TODO
	spin_lock_irqsave(&target_loads_lock, flags);
	if (target_loads != default_target_loads)
		kfree(target_loads);
	target_loads = new_target_loads;
	ntarget_loads = ntokens;
	spin_unlock_irqrestore(&target_loads_lock, flags);
#else
	spin_lock_irqsave(&a->Data_12, flags);
	if (a->Data_16 != /*&Data_80f9c814*/default_target_loads)
		kfree(a->Data_16);
	a->Data_16 = new_target_loads;
	a->Data_20 = ntokens;
	spin_unlock_irqrestore(&a->Data_12, flags);
#endif
	return count;
}

ssize_t store_target_loads_gov_pol(struct cpufreq_policy *policy,
	/*struct kobject *kobj, struct attribute *attr,*/ const char *buf,
	size_t count)
{
	return store_target_loads(policy->governor_data, buf, count);
}

ssize_t store_target_loads_gov_sys(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
{
	return store_target_loads(Data_810f3bc8, buf, count);
}

static struct global_attr target_loads_attr =
	__ATTR(target_loads, S_IRUGO | S_IWUSR,
		show_target_loads, store_target_loads);

#if 0
static ssize_t show_above_hispeed_delay(
	struct kobject *kobj, struct attribute *attr, char *buf)
#else
static ssize_t show_above_hispeed_delay(Struct_810f3bc8* a, char *buf)
#endif
{
	int i;
	ssize_t ret = 0;
	unsigned long flags;

#if 0
	spin_lock_irqsave(&above_hispeed_delay_lock, flags);

	for (i = 0; i < nabove_hispeed_delay; i++)
		ret += sprintf(buf + ret, "%u%s", above_hispeed_delay[i],
			       i & 0x1 ? ":" : " ");

	ret += sprintf(buf + ret, "\n");
	spin_unlock_irqrestore(&above_hispeed_delay_lock, flags);
#else
	spin_lock_irqsave(&a->Data_32, flags);

	for (i = 0; i < a->Data_40; i++)
		ret += sprintf(buf + ret, "%u%s", a->Data_36[i],
			       i & 0x1 ? ":" : " ");

	ret += sprintf(buf + ret-1, "\n");
	spin_unlock_irqrestore(&a->Data_32, flags);
#endif
	return ret-1;
}

ssize_t show_above_hispeed_delay_gov_pol(struct cpufreq_policy *policy,
	/*struct kobject *kobj, struct attribute *attr,*/ const char *buf)
{
	return show_above_hispeed_delay(policy->governor_data, buf);
}

ssize_t show_above_hispeed_delay_gov_sys(
	struct kobject *kobj, struct attribute *attr, const char *buf)
{
	return show_above_hispeed_delay(Data_810f3bc8, buf);
}


#if 0
static ssize_t store_above_hispeed_delay(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
#else
static ssize_t store_above_hispeed_delay(Struct_810f3bc8* a, const char *buf, size_t count)
#endif
{
	int ntokens;
	unsigned int *new_above_hispeed_delay = NULL;
	unsigned long flags;

	new_above_hispeed_delay = get_tokenized_data(buf, &ntokens);
	if (IS_ERR(new_above_hispeed_delay))
		return PTR_RET(new_above_hispeed_delay);

#if 0
	spin_lock_irqsave(&above_hispeed_delay_lock, flags);
	if (above_hispeed_delay != default_above_hispeed_delay)
		kfree(above_hispeed_delay);
	above_hispeed_delay = new_above_hispeed_delay;
	nabove_hispeed_delay = ntokens;
	spin_unlock_irqrestore(&above_hispeed_delay_lock, flags);
#else
	spin_lock_irqsave(&a->Data_32, flags);
	if (a->Data_36 != /*&Data_80f9c810*/default_above_hispeed_delay)
		kfree(a->Data_36);
	a->Data_36 = new_above_hispeed_delay;
	a->Data_40 = ntokens;
	spin_unlock_irqrestore(&a->Data_32, flags);
#endif
	return count;

}

ssize_t store_above_hispeed_delay_gov_pol(struct cpufreq_policy *policy,
	/*struct kobject *kobj, struct attribute *attr,*/ const char *buf,
	size_t count)
{
	return store_above_hispeed_delay(policy->governor_data, buf, count);
}

ssize_t store_above_hispeed_delay_gov_sys(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
{
	return store_above_hispeed_delay(Data_810f3bc8, buf, count);
}

static struct global_attr above_hispeed_delay_attr =
	__ATTR(above_hispeed_delay, S_IRUGO | S_IWUSR,
		show_above_hispeed_delay, store_above_hispeed_delay);

static ssize_t show_hispeed_freq(struct kobject *kobj,
				 struct attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", hispeed_freq);
}

static ssize_t store_hispeed_freq(struct kobject *kobj,
				  struct attribute *attr, const char *buf,
				  size_t count)
{
	int ret;
	long unsigned int val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	hispeed_freq = val;
	return count;
}

static struct global_attr hispeed_freq_attr = __ATTR(hispeed_freq, 0644,
		show_hispeed_freq, store_hispeed_freq);


static ssize_t show_go_hispeed_load(struct kobject *kobj,
				     struct attribute *attr, char *buf)
{
#if 0 //STC2HI: TODO
	return sprintf(buf, "%lu\n", go_hispeed_load);
#endif
}

static ssize_t store_go_hispeed_load(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
#if 0 //STC2HI: TODO
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	go_hispeed_load = val;
#endif
	return count;
}

static struct global_attr go_hispeed_load_attr = __ATTR(go_hispeed_load, 0644,
		show_go_hispeed_load, store_go_hispeed_load);

static ssize_t show_min_sample_time(struct kobject *kobj,
				struct attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", min_sample_time);
}

static ssize_t store_min_sample_time(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	min_sample_time = val;
	return count;
}

static struct global_attr min_sample_time_attr = __ATTR(min_sample_time, 0644,
		show_min_sample_time, store_min_sample_time);

static ssize_t show_timer_rate(struct kobject *kobj,
			struct attribute *attr, char *buf)
{
	return sprintf(buf, "%lu\n", timer_rate);
}

static ssize_t store_timer_rate(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = strict_strtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	timer_rate = val;
	return count;
}

static struct global_attr timer_rate_attr = __ATTR(timer_rate, 0644,
		show_timer_rate, store_timer_rate);

static ssize_t show_timer_slack(
	struct kobject *kobj, struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", timer_slack_val);
}

static ssize_t store_timer_slack(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
{
	int ret;
	unsigned long val;

	ret = kstrtol(buf, 10, &val);
	if (ret < 0)
		return ret;

	timer_slack_val = val;
	return count;
}

define_one_global_rw(timer_slack);

static ssize_t show_boost(struct kobject *kobj, struct attribute *attr,
			  char *buf)
{
	return sprintf(buf, "%d\n", boost_val);
}

static ssize_t store_boost(struct cpufreq_interactive_tunables *tunables,
		/*struct kobject *kobj, struct attribute *attr,*/
			   const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	tunables->Data_44/*boost_val*/ = val;

	if (tunables->Data_44/*boost_val*/) {
		trace_cpufreq_interactive_boost("on");
		cpufreq_interactive_boost();
	} else {
		trace_cpufreq_interactive_unboost("off");
	}

	return count;
}

ssize_t store_boost_gov_pol(struct cpufreq_policy *policy,
	/*struct kobject *kobj, struct attribute *attr,*/ const char *buf,
	size_t count)
{
	return store_boost(policy->governor_data, buf, count);
}

ssize_t store_boost_gov_sys(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
{
	return store_boost(Data_810f3bc8, buf, count);
}

define_one_global_rw(boost);

static ssize_t store_boostpulse(struct cpufreq_interactive_tunables *tunables,
		//struct kobject *kobj, struct attribute *attr,
				const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	/*boostpulse_endtime*/tunables->boostpulse_endtime = ktime_to_us(ktime_get()) +
			/*boostpulse_duration_val*/tunables->/*boostpulse_duration_val*/Data_48;
	trace_cpufreq_interactive_boost("pulse");
	cpufreq_interactive_boost();
	return count;
}

ssize_t store_boostpulse_gov_pol(struct cpufreq_policy *policy,
	/*struct kobject *kobj, struct attribute *attr,*/ const char *buf,
	size_t count)
{
	return store_boostpulse(policy->governor_data, buf, count);
}

ssize_t store_boostpulse_gov_sys(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
{
	return store_boostpulse(Data_810f3bc8, buf, count);
}

static struct global_attr boostpulse =
	__ATTR(boostpulse, 0200, NULL, store_boostpulse);

static ssize_t show_boostpulse_duration(
	struct kobject *kobj, struct attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", boostpulse_duration_val);
}

static ssize_t store_boostpulse_duration(
	struct kobject *kobj, struct attribute *attr, const char *buf,
	size_t count)
{
	int ret;
	unsigned long val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;

	boostpulse_duration_val = val;
	return count;
}

define_one_global_rw(boostpulse_duration);

static ssize_t show_io_is_busy(struct kobject *kobj,
			struct attribute *attr, char *buf)
{
	return sprintf(buf, "%u\n", io_is_busy);
}

static ssize_t store_io_is_busy(struct kobject *kobj,
			struct attribute *attr, const char *buf, size_t count)
{
	int ret;
	unsigned long val;

	ret = kstrtoul(buf, 0, &val);
	if (ret < 0)
		return ret;
	io_is_busy = val;
	return count;
}

static struct global_attr io_is_busy_attr = __ATTR(io_is_busy, 0644,
		show_io_is_busy, store_io_is_busy);

static struct attribute *interactive_attributes[] = {
	&target_loads_attr.attr,
	&above_hispeed_delay_attr.attr,
	&hispeed_freq_attr.attr,
	&go_hispeed_load_attr.attr,
	&min_sample_time_attr.attr,
	&timer_rate_attr.attr,
	&timer_slack.attr,
	&boost.attr,
	&boostpulse.attr,
	&boostpulse_duration.attr,
	&io_is_busy_attr.attr,
	NULL,
};

static struct attribute_group interactive_attr_group = {
	.attrs = interactive_attributes,
	.name = "interactive",
};

static int cpufreq_interactive_idle_notifier(struct notifier_block *nb,
					     unsigned long val,
					     void *data)
{
#if 1 //STC2HI: TODO
	switch (val) {
	case IDLE_START:
		cpufreq_interactive_idle_start();
		break;
	case IDLE_END:
		cpufreq_interactive_idle_end();
		break;
	}
#endif

	return 0;
}

static struct attribute_group interactive_attr_group_gov_sys = {
	.attrs = interactive_attributes, //dbs_attributes_gov_sys,
	.name = "conservative",
};

static struct attribute_group interactive_attr_group_gov_pol = {
	.attrs = interactive_attributes, //dbs_attributes_gov_pol,
	.name = "conservative",
};

static struct notifier_block cpufreq_interactive_idle_nb = { //80f9c860
	.notifier_call = cpufreq_interactive_idle_notifier,
};

static struct attribute_group *get_sysfs_attr(void)
{
	if (have_governor_per_policy())
		return &interactive_attr_group_gov_pol;
	else
		return &interactive_attr_group_gov_sys;
}

static int cpufreq_governor_interactive(struct cpufreq_policy *policy,
		unsigned int event)
{
	int rc;
	unsigned int j;
	struct cpufreq_interactive_cpuinfo *pcpu;
	struct cpufreq_frequency_table *freq_table;
	Struct_810f3bc8 *r7;
	Struct_810f3bc8* r6;
	extern struct kobject *get_governor_parent_kobj(struct cpufreq_policy *policy);

	if (have_governor_per_policy())
		r7 = policy->governor_data;
	else
		r7 = Data_810f3bc8;

	WARN_ON(!r7 && (event != CPUFREQ_GOV_POLICY_INIT)); //#1165

	switch (event) {
	case CPUFREQ_GOV_POLICY_INIT: //804cd1fc
		if (have_governor_per_policy())
		{
			//804cd208
			WARN_ON(r7); //#1170
		}
		else
		{
			//804cd2d8
			if (r7)
			{
				//804cd2e0
				r7->Data_0++;
				policy->governor_data = r7;
				//->804ccf00
				break;
			}
		}
		//804cd210
		r6 = kzalloc(72, GFP_KERNEL);
		if (r6 == 0)
		{
			printk("\0013%s: POLICY_INIT: kzalloc failed\n", "cpufreq_governor_interactive");
			return -ENOMEM;
		}

		rc = sysfs_create_group(get_governor_parent_kobj(policy), get_sysfs_attr());
#if 0
				have_governor_per_policy()?
						&interactive_attr_group_gov_pol:
						&interactive_attr_group_gov_sys);
#endif
		if (rc)
		{
			kfree(r6);
			return rc;
		}
		//804cd264
		r6->Data_36 = default_above_hispeed_delay; //&Data_80f9c810;
		r6->Data_20 = 1;
		r6->Data_0 = 1;
		r6->Data_8 = 90;
		r6->Data_16 = default_target_loads; //&Data_80f9c814;
		r6->Data_40 = 1;
		r6->Data_64 = 80000;
		r6->Data_28 = 20000;
		r6->Data_24 = 80000;
		r6->Data_48 = 80000;
#if 0 //TODO
		r6->Data_12 = 0;
		r6->Data_32 = 0;
#endif

		if (policy->governor->initialized == 0)
		{
			//->804cd314
			extern void idle_notifier_register(struct notifier_block *n);
			idle_notifier_register(&cpufreq_interactive_idle_nb);

			cpufreq_register_notifier(
				&cpufreq_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
		}
		//804cd2bc
		policy->governor_data = r6;

		if (!have_governor_per_policy())
		{
			Data_810f3bc8 = r6;
		}
		//->804ccf00
		break;

	case CPUFREQ_GOV_START:
#if 0
		if (!cpu_online(policy->cpu))
			return -EINVAL;
#endif
		mutex_lock(&gov_lock);

		freq_table =
			cpufreq_frequency_get_table(policy->cpu);

#if 0
		if (!hispeed_freq)
			hispeed_freq = policy->max;
#else
		if (!r7->hispeed_freq)
		{
			r7->hispeed_freq = policy->max;
		}
#endif

		for_each_cpu(j, policy->cpus) {
#if 0
			unsigned long expires;
#endif

			pcpu = &per_cpu(cpuinfo, j);
			pcpu->policy = policy;
			pcpu->target_freq = policy->cur;
			pcpu->freq_table = freq_table;
			pcpu->floor_freq = pcpu->target_freq;
			pcpu->floor_validate_time =
				ktime_to_us(ktime_get());
			pcpu->hispeed_validate_time =
				pcpu->floor_validate_time;
			down_write(&pcpu->enable_sem);
#if 0
			expires = jiffies + usecs_to_jiffies(timer_rate);
			pcpu->cpu_timer.expires = expires;
			add_timer_on(&pcpu->cpu_timer, j);
			if (timer_slack_val >= 0) {
				expires += usecs_to_jiffies(timer_slack_val);
				pcpu->cpu_slack_timer.expires = expires;
				add_timer_on(&pcpu->cpu_slack_timer, j);
			}
#else
			del_timer_sync(&pcpu->cpu_timer);
			del_timer_sync(&pcpu->cpu_slack_timer);
			cpufreq_interactive_timer_start(r7, j);
#endif
			pcpu->governor_enabled = 1;
			up_write(&pcpu->enable_sem);
		}

#if 0
		/*
		 * Do not register the idle hook and create sysfs
		 * entries if we have already done so.
		 */
		if (++active_count > 1) {
			mutex_unlock(&gov_lock);
			return 0;
		}

		rc = sysfs_create_group(cpufreq_global_kobject,
				&interactive_attr_group);
		if (rc) {
			mutex_unlock(&gov_lock);
			return rc;
		}

		idle_notifier_register(&cpufreq_interactive_idle_nb);
		cpufreq_register_notifier(
			&cpufreq_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
#endif
		mutex_unlock(&gov_lock);
		break;

	case CPUFREQ_GOV_STOP:
		mutex_lock(&gov_lock);
		for_each_cpu(j, policy->cpus) {
			pcpu = &per_cpu(cpuinfo, j);
			down_write(&pcpu->enable_sem);
			pcpu->governor_enabled = 0;
			del_timer_sync(&pcpu->cpu_timer);
			del_timer_sync(&pcpu->cpu_slack_timer);
			up_write(&pcpu->enable_sem);
		}

#if 0
		if (--active_count > 0) {
			mutex_unlock(&gov_lock);
			return 0;
		}

		cpufreq_unregister_notifier(
			&cpufreq_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
		idle_notifier_unregister(&cpufreq_interactive_idle_nb);
		sysfs_remove_group(cpufreq_global_kobject,
				&interactive_attr_group);
#endif
		mutex_unlock(&gov_lock);

		break;

	case CPUFREQ_GOV_LIMITS:
		if (policy->max < policy->cur)
			__cpufreq_driver_target(policy,
					policy->max, CPUFREQ_RELATION_H);
		else if (policy->min > policy->cur)
			__cpufreq_driver_target(policy,
					policy->min, CPUFREQ_RELATION_L);

#if 1
		for_each_cpu(j, policy->cpus) {
			pcpu = &per_cpu(cpuinfo, j);
			down_write(&pcpu->enable_sem);

			if (pcpu->governor_enabled)
			{
				//804cd1a8
				if (policy->max < pcpu->target_freq)
					pcpu->target_freq = policy->max;
				else if (pcpu->target_freq < policy->min)
					pcpu->target_freq = policy->min;

				del_timer_sync(&pcpu->cpu_timer);
				del_timer_sync(&pcpu->cpu_slack_timer);

				cpufreq_interactive_timer_start(r7, j);
			}
			//804cd1e8
			up_write(&pcpu->enable_sem);
		}
#endif

		break;

	case CPUFREQ_GOV_POLICY_EXIT:
		//804ccf0c
		r7->Data_0--;
		if (r7->Data_0 == 0)
		{
			//804ccf20
			struct cpufreq_governor	*governor = policy->governor;

			if (governor->initialized == 1)
			{
				//804cd338
				extern void idle_notifier_unregister(struct notifier_block *n);

				cpufreq_unregister_notifier(&cpufreq_notifier_block,
						CPUFREQ_TRANSITION_NOTIFIER);
				idle_notifier_unregister(&cpufreq_interactive_idle_nb);
			}
			//804ccf30
			sysfs_remove_group(get_governor_parent_kobj(policy), get_sysfs_attr());
#if 0
					have_governor_per_policy()?
							&interactive_attr_group_gov_pol:
							&interactive_attr_group_gov_sys);
#endif
			kfree(r7);

			Data_810f3bc8 = 0;
		}
		//804ccf70
		policy->governor_data = 0;
		//->804ccf00
		break;
	}

	return 0;
}

static void cpufreq_interactive_nop_timer(unsigned long data)
{
}

static int __init cpufreq_interactive_init(void)
{
	unsigned int i;
	struct cpufreq_interactive_cpuinfo *pcpu;
	struct sched_param param = { .sched_priority = MAX_RT_PRIO-1 };

	/* Initalize per-cpu timers */
	for_each_possible_cpu(i) {
		pcpu = &per_cpu(cpuinfo, i);
		init_timer_deferrable(&pcpu->cpu_timer);
		pcpu->cpu_timer.function = cpufreq_interactive_timer;
		pcpu->cpu_timer.data = i;
		init_timer(&pcpu->cpu_slack_timer);
		pcpu->cpu_slack_timer.function = cpufreq_interactive_nop_timer;
		spin_lock_init(&pcpu->load_lock);
		init_rwsem(&pcpu->enable_sem);
	}

	spin_lock_init(&target_loads_lock);
	spin_lock_init(&speedchange_cpumask_lock);
	mutex_init(&gov_lock);
	speedchange_task =
		kthread_create(cpufreq_interactive_speedchange_task, NULL,
			       "cfinteractive");
	if (IS_ERR(speedchange_task))
		return PTR_ERR(speedchange_task);

	sched_setscheduler_nocheck(speedchange_task, SCHED_FIFO, &param);
	get_task_struct(speedchange_task);

	/* NB: wake up so the thread does not look hung to the freezer */
	wake_up_process(speedchange_task);

	return cpufreq_register_governor(&cpufreq_gov_interactive);
}

#ifdef CONFIG_CPU_FREQ_DEFAULT_GOV_INTERACTIVE
fs_initcall(cpufreq_interactive_init);
#else
module_init(cpufreq_interactive_init);
#endif

static void __exit cpufreq_interactive_exit(void)
{
	cpufreq_unregister_governor(&cpufreq_gov_interactive);
	kthread_stop(speedchange_task);
	put_task_struct(speedchange_task);
}

module_exit(cpufreq_interactive_exit);

MODULE_AUTHOR("Mike Chan <mike@android.com>");
MODULE_DESCRIPTION("'cpufreq_interactive' - A cpufreq governor for "
	"Latency sensitive workloads");
MODULE_LICENSE("GPL");
