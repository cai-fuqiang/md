// SPDX-License-Identifier: GPL-2.0
/*
 * Out-of-tree Magic SysRq Alt+F12 trigger implemented by kprobe.
 *
 * The real SysRq input filter already handles Alt state, SysRq activation,
 * subkey translation and event suppression. Keep those semantics intact and
 * only rewrite the sysrq_filter() keycode argument from KEY_F12 to KEY_SYSRQ.
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/input.h>
#include <linux/jiffies.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/ptrace.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

#define SYSRQ_ALT_F12_MAX_HANDLES	32
#define SYSRQ_ALT_F12_RIGHTALT_DELAY_MS	5000

struct sysrq_alt_f12_state {
	struct delayed_work rightalt_up_work;
	struct input_handle *handle;
	bool f12_down;
	bool defer_rightalt_up;
	bool injecting_rightalt_up;
};

static DEFINE_RAW_SPINLOCK(sysrq_alt_f12_lock);
static struct sysrq_alt_f12_state sysrq_alt_f12_states[SYSRQ_ALT_F12_MAX_HANDLES];

static void sysrq_alt_f12_set_code(struct pt_regs *regs, unsigned long code)
{
#if defined(CONFIG_ARM64)
	regs->regs[2] = code;
#elif defined(CONFIG_RISCV)
	regs->a2 = code;
#elif defined(CONFIG_X86_64)
	regs->dx = code;
#elif defined(CONFIG_LOONGARCH)
	regs->regs[6] = code;
#else
#error "Unsupported architecture for sysrq_alt_f12_kprobe"
#endif
}
NOKPROBE_SYMBOL(sysrq_alt_f12_set_code);

static void sysrq_alt_f12_set_value(struct pt_regs *regs, unsigned long value)
{
#if defined(CONFIG_ARM64)
	regs->regs[3] = value;
#elif defined(CONFIG_RISCV)
	regs->a3 = value;
#elif defined(CONFIG_X86_64)
	regs->cx = value;
#elif defined(CONFIG_LOONGARCH)
	regs->regs[7] = value;
#else
#error "Unsupported architecture for sysrq_alt_f12_kprobe"
#endif
}
NOKPROBE_SYMBOL(sysrq_alt_f12_set_value);

static void sysrq_alt_f12_rightalt_up_work(struct work_struct *work)
{
	struct sysrq_alt_f12_state *state =
		container_of(to_delayed_work(work), struct sysrq_alt_f12_state,
			     rightalt_up_work);
	struct input_handle *handle;
	unsigned long flags;

	raw_spin_lock_irqsave(&sysrq_alt_f12_lock, flags);
	handle = state->handle;
	state->defer_rightalt_up = false;
	state->injecting_rightalt_up = true;
	raw_spin_unlock_irqrestore(&sysrq_alt_f12_lock, flags);

	if (handle) {
		input_inject_event(handle, EV_KEY, KEY_RIGHTALT, 1);
		input_inject_event(handle, EV_KEY, KEY_RIGHTALT, 0);
		input_inject_event(handle, EV_SYN, SYN_REPORT, 1);
	}

	raw_spin_lock_irqsave(&sysrq_alt_f12_lock, flags);
	state->injecting_rightalt_up = false;
	raw_spin_unlock_irqrestore(&sysrq_alt_f12_lock, flags);
}

static struct sysrq_alt_f12_state *sysrq_alt_f12_get_state(struct input_handle *handle)
{
	struct sysrq_alt_f12_state *free = NULL;
	int i;

	for (i = 0; i < ARRAY_SIZE(sysrq_alt_f12_states); i++) {
		if (sysrq_alt_f12_states[i].handle == handle)
			return &sysrq_alt_f12_states[i];
		if (!sysrq_alt_f12_states[i].handle && !free)
			free = &sysrq_alt_f12_states[i];
	}

	if (free)
		free->handle = handle;

	return free;
}
NOKPROBE_SYMBOL(sysrq_alt_f12_get_state);

static int sysrq_alt_f12_pre_handler(struct kprobe *p, struct pt_regs *regs)
{
	struct input_handle *handle =
		(struct input_handle *)regs_get_kernel_argument(regs, 0);
	unsigned int type = regs_get_kernel_argument(regs, 1);
	unsigned int code = regs_get_kernel_argument(regs, 2);
	int value = regs_get_kernel_argument(regs, 3);
	struct sysrq_alt_f12_state *state;
	unsigned long flags;
	bool schedule_rightalt_up = false;

	if (type != EV_KEY)
		return 0;

	raw_spin_lock_irqsave(&sysrq_alt_f12_lock, flags);
	state = sysrq_alt_f12_get_state(handle);
	if (!state)
		goto out;

	if (code == KEY_F12) {
		if (value == 1) {
			state->f12_down = true;
			sysrq_alt_f12_set_code(regs, KEY_SYSRQ);
		} else if (value == 0) {
			state->f12_down = false;
			sysrq_alt_f12_set_code(regs, KEY_SYSRQ);
		} else {
			sysrq_alt_f12_set_code(regs, KEY_SYSRQ);
		}
	} else if (code == KEY_RIGHTALT && value == 0 && state->f12_down &&
		   !state->injecting_rightalt_up) {
		state->defer_rightalt_up = true;
		sysrq_alt_f12_set_value(regs, 2);
		schedule_rightalt_up = true;
	}

out:
	raw_spin_unlock_irqrestore(&sysrq_alt_f12_lock, flags);

	if (schedule_rightalt_up)
		mod_delayed_work(system_wq, &state->rightalt_up_work,
				 msecs_to_jiffies(SYSRQ_ALT_F12_RIGHTALT_DELAY_MS));

	return 0;
}
NOKPROBE_SYMBOL(sysrq_alt_f12_pre_handler);

static struct kprobe sysrq_alt_f12_kp = {
	.symbol_name = "sysrq_filter",
	.pre_handler = sysrq_alt_f12_pre_handler,
};

static int __init sysrq_alt_f12_init(void)
{
	int ret;
	int i;

	for (i = 0; i < ARRAY_SIZE(sysrq_alt_f12_states); i++)
		INIT_DELAYED_WORK(&sysrq_alt_f12_states[i].rightalt_up_work,
				  sysrq_alt_f12_rightalt_up_work);

	ret = register_kprobe(&sysrq_alt_f12_kp);
	if (ret) {
		pr_err("failed to register kprobe on sysrq_filter: %d\n", ret);
		return ret;
	}

	pr_info("enabled, probing %s at %px\n",
		sysrq_alt_f12_kp.symbol_name, sysrq_alt_f12_kp.addr);
	return 0;
}
module_init(sysrq_alt_f12_init);

static void __exit sysrq_alt_f12_exit(void)
{
	int i;

	unregister_kprobe(&sysrq_alt_f12_kp);

	for (i = 0; i < ARRAY_SIZE(sysrq_alt_f12_states); i++)
		cancel_delayed_work_sync(&sysrq_alt_f12_states[i].rightalt_up_work);

	pr_info("disabled\n");
}
module_exit(sysrq_alt_f12_exit);

MODULE_DESCRIPTION("Magic SysRq Alt+F12 trigger via kprobe");
MODULE_AUTHOR("openEuler");
MODULE_LICENSE("GPL");
