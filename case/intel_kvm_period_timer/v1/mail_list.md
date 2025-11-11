> > When the guest uses the APIC periodic timer, if the delay exceeds the
> > period, the delta will be negative. 
> 
> IIUC, by "delay" you mean the time it takes for KVM to get (back) to
> advance_periodic_target_expiration().  If that's correct, I think it would be
> clearer to word this as:
> 
>   When the guest uses the APIC periodic timer, if the next period has already
>   expired, e.g. due to the period being smaller than the delay in processing
>   the timer, the delta will be negative.

Indeed, this explanation is much clearer. I will adopt it in the next version
patch.

> This isn't quite correct either.  E.g. if delta is negative while L1 TSC is tiny,
> then subtracting the delta will incorrectly result the deadline wrapping too.
> Very, very, theoretically, L1 TSC could even be '0', e.g. due to a weird offset
> for L1, so I don't think subtracting is ever safe.  Heh, of course we're hosed
> in that case no matter what since KVM treats tscdeadline==0 as "not programmed".
> 
> Anyways, can't we just skip adding negative value?  Whether or not the TSC deadline
> has expired is mostly a boolean value; for the vast majority of code it doesn't
> matter exactly when the timer expired.

I don’t think this patch will cause tscdeadline to wrap around to zero, because
the system is unlikely to start a timer when the guest tsc is zero and have it
expire immediately. However, keeping the logic to skip adding negative values is
a good idea, seems like there’s no downside.

> The only code that cares is __kvm_wait_lapic_expire(), and the only downside to
> setting tscdeadline=L1.TSC is that adjust_lapic_timer_advance() won't adjust as
> aggressively as it probably should.

I am not sure which type of timers should use the "advanced tscdeadline hrtimer
expiration feature".

I list the history of this feature.

1. Marcelo first introduce this feature, only support the tscdeadline sw timer.
2. Yunhong introduce vmx preemption timer(hv), only support for tscdeadline.
3. Liwanpeng extend the hv timer to oneshot and period timers.
4. Liwanpeng extend this feature to hv timer.
5. Sean and liwanpeng fix some BUG extend this feature to hv period/oneshot timer.

[1] d0659d946be0("KVM: x86: add option to advance tscdeadline hrtimer expiration")
    Marcelo Tosatti     Dec 16 2014
[2] ce7a058a2117("KVM: x86: support using the vmx preemption timer for tsc deadline timer")
    Yunhong Jiang       Jun 13 2016
[3] 8003c9ae204e("KVM: LAPIC: add APIC Timer periodic/oneshot mode VMX preemption timer support")
    liwanpeng           Oct 24 2016
[4] c5ce8235cffa("KVM: VMX: Optimize tscdeadline timer latency")
    liwanpeng           May 29 2018
[5] ee66e453db13("KVM: lapic: Busy wait for timer to expire when using hv_timer")
    Sean Christopherson Apr 16 2019

    d981dd15498b("KVM: LAPIC: Accurately guarantee busy wait for timer to expire when using hv_timer")
    liwanpeng           Apr 28 2021

Now, timers supported for this feature includes:
- sw: tscdeadline
- hv: all (tscdeadline, oneshot, period)

====
IMHO
====

1. for period timer
===================

I think for periodic timers emulation, the expiration time is already adjusted
to compensate for the delays introduced by timer emulation, so don't need this
feature to adjust again. But after use the feature, the first timer expiration
may be relatively accurate.

E.g., At time 0, start a periodic task (period: 10,000 ns) with a simulated
delay of 100 ns.

With this feature enabled and reasonably accurate prediction, the expiration
time set seen by the guest are: 10000, 20000, 30000...

With this feature not enabled, the expiration times set: 10100, 20100, 30100...
 
But IMHO, for periodic timers, accuracy of the period seems to be the main
concern, because it does not frequently start and stop. The incorrect period
caused by the first timer expiration can be ignored. 

2. for oneshot timer
====================

In [1], Marcelo treated oneshot and tscdeadline differently. Shouldn’t the
behavior of these two timers be similar? Unlike periodic timers, both oneshot
and tscdeadline timers set a specific expiration time, and what the guest cares
about is whether the expiration time is accurate. Moreover, this feature is
mainly intended to mitigate the latency introduced by timer virtualization.
Since software timers have higher latency compared to hardware virtual timers,
the need for this feature is actually more urgent for software timers. However,
in the current implementation, the feature is applied to hv oneshot/period
timers, but not to sw oneshot/period timers.

===============
Summary of IMHO
===============

The feature should be applied to the following timer types:
sw/hv tscdeadline and sw/hv oneshot

should not be applied to:
sw/hv period

However, so far I haven’t found any discussion on the mailing lists regarding
the commits summarized above. Please let me know if I’ve overlooked something.

> Ha!  That's essentially what update_target_expiration() already does:
> 
> 	now = ktime_get();
> 	remaining = ktime_sub(apic->lapic_timer.target_expiration, now);
> 	if (ktime_to_ns(remaining) < 0)
> 		remaining = 0;

Yes!


>
>
>

Thank you for your patient explanations and for helping me make the revisions. I
will update this in the next patch version.

Regards,
fuqiang
