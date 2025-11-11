> On Fri, Nov 07, 2025, fuqiang wang wrote:
> > When a VM is suspended while using the periodic HV timer, the KVM timer
> > also ceases to advance. After the VM resumes from a prolonged suspend,
> 
> Maybe it's because I've been dealing with a variety of different suspend+resume
> issue, but I find it confusing to talk about suspend+resume, partly because it's
> not immediately clear if you're talking about _host_ suspend+resume versus guest
> suspend+resume.

Yes, using guest suspend+resume is clearer.

> And suspend+resume isn't the only way this issue could be hit,
> e.g. userspace could simply stop running a VM for whatever reason and get the
> same result.

Yes, it just requires staying in user space for a rather long time(I haven’t
tested it specifically, it might take several days or even dozens of days). As a
comment or commit message in the KVM, it is indeed better to make the
description more general, rather than restricting it to specific scenarios.

>
> I even think we should downplay the HV timer / VMX Preemption Timer angle to
> some extent, because restarting the hrtimer if it expires while the vCPU is in
> userspace is wasteful and unnecessary.  And if we ever "fix" that, e.g. maybe by
> not restarting the hrtimer if vcpu->wants_to_run is false, then the hard lockup
> could be hittable even without the HV timer.

Yes, I also noticed this issue. But initially, I didn’t have a good idea for how
to fix it, because KVM can’t determine how long it will take to return to
userspace. If the guest is in user space only briefly, not restarting the KVM
software period timer is unreasonable. Using vcpu->wants_to_run seems good.
However, I am not very familiar with KVM_CAP_IMMEDIATE_EXIT. I will review its
history and try to run some tests.

> > This patch makes the following modification: When handling KVM periodic
> > timer expiration, if we find that the advanced target_expiration is still
> > less than now, we set target_expiration directly to now (just like how
> > update_target_expiration handles the remaining).
> 
> Please don't use "this patch" (it's superfluous since this is obvioulsy a patch),
> or pronouns like "we" and "us" as pronouns are often ambiguous in the world of
> KVM.  E.g. "we" might me "we the cloud provider", "we the VMM", "we as KVM", "we
> as the host kernel", etc.

Thank you for patiently. This is a big mistake I shouldn’t have made and I will
thoroughly review the code submission guidelines for kernel/KVM-x86 as provided
in following links:

[1] https://docs.kernel.org/process/maintainer-kvm-x86.html#comments
[2] https://docs.kernel.org/process/submitting-patches.html#describe-your-changes

> 
> > Fixes: d8f2f498d9ed ("x86/kvm: fix LAPIC timer drift when guest uses periodic mode")
> > Signed-off-by: fuqiang wang <fuqiang.wng@gmail.com>
> > ---
> >  arch/x86/kvm/lapic.c | 32 ++++++++++++++++++++++++--------
> >  1 file changed, 24 insertions(+), 8 deletions(-)
> > 
> > diff --git a/arch/x86/kvm/lapic.c b/arch/x86/kvm/lapic.c
> > index 0ae7f913d782..bc082271c81c 100644
> > --- a/arch/x86/kvm/lapic.c
> > +++ b/arch/x86/kvm/lapic.c
> > @@ -2131,18 +2131,34 @@ static void advance_periodic_target_expiration(struct kvm_lapic *apic)
> >  	ktime_t delta;
> >
> >  	/*
> > -	 * Synchronize both deadlines to the same time source or
> > -	 * differences in the periods (caused by differences in the
> > -	 * underlying clocks or numerical approximation errors) will
> > -	 * cause the two to drift apart over time as the errors
> > -	 * accumulate.
> > +	 * Use kernel time as the time source for both deadlines so that they
> > +	 * stay synchronized.  Computing each deadline independently will cause
> > +	 * the two deadlines to drift apart over time as differences in the
> > +	 * periods accumulate, e.g. due to differences in the underlying clocks
> > +	 * or numerical approximation errors.
> >  	 */
> >  	apic->lapic_timer.target_expiration =
> >  		ktime_add_ns(apic->lapic_timer.target_expiration,
> >  				apic->lapic_timer.period);
> > +
> > +	/*
> > +	 * When the vm is suspend, the hv timer also stops advancing. After it
> > +	 * is resumed, this may result in a large delta. If the
> 
> This definitely shouldn't talk about suspend+resume, because that's fully a VMM
> concept and isn't the _only_ way to end up with a huge delta.

Yes, it's better!

> 
> > +	 * target_expiration only advances by one period each time, it will
> > +	 * cause KVM to frequently handle timer expirations.
> 
> It's also worth calling out that KVM will only deliver a single IRQ to the guest,
> i.e. restarting the timer over and over (and over and over) is completey usless.
> *Except* if KVM is posting IRQs across CPUs, e.g. running hrtimers on housekeeping
> CPUs, in which case KVM really will deliver an interrupt storm to the guest.
>
> But I don't want to try and special case that mode, because there's no guarantee
> KVM can post a timer, and I don't want to have arbitrarily different guest-visible
> behavior based on a fairly obscure module param (and other settings).  So unless
> I too am missing something, setting the target expiration to "now" seems like the
> way to go.

Yep, I also think it’s better to handle this in a simple and consistent way.
Here are my thoughts:

There are two possible reasons why the new target_expiration might lag behind:

1. The KVM timer is stopped.
2. The period is smaller than the delay in processing the timer.

In the first case, the timer shouldn’t be counting down because cpu is stopped.
Therefore, we should keep the LAPIC current count as it was before stopping.
That means we need to record the delta between target_expiration and now before
stopping, and set it when starting timer in future.

***

In the second case, it appears to the guest that the timer source is not very
accurate and cannot deliver interrupts at precise times. In d8f2f498d9ed
("x86/kvm: fix LAPIC timer drift when guest uses periodic mode"), David made the
KVM periodic timer more accurate by adjusting the next target_expiration, as a
result, over a relatively long duration, we can observe more accurate timer
interrupt counts and intervals. 

However, this approach doesn’t apply when the target_expiration is already
significantly behind. In that situation, even though the total number of timer
interrupts over a long duration may be correct, the interrupts themselves are
triggered too close together. In this situation, the guest would prefer to see
regular timer period, rather than an almost frantic burst of timer interrupts.

And as you mentioned, if KVM is not posting timer IRQs across CPUs, the guest
won’t even have a chance to see an interrupt storm, or even two consecutive
interrupts. 

***

Given all the situations above, I don’t think it’s worth adding a lot of
complexity for this, it’s better to handle this in a simple and consistent way.

> > +	 */
> > +	if (apic->lapic_timer.period > 0 &&
> 
> apic->lapic_timer.period is guaranteed to be zero here.  kvm_lapic_expired_hv_timer()
> and start_sw_period() check it explicitly, and apic_timer_fn() should be unreachable
> with period==0 (KVM is supposed to cancel the hrtimer before changing the period).
> 

Yes, that was my mistake. Thank you for pointing it out.

> I do think it's worth hardening apic_timer_fn() though, but in a separate prep
> patch.
> 
> > +	    ktime_before(apic->lapic_timer.target_expiration, now))
> > +		apic->lapic_timer.target_expiration = now;
> > +
> >  	delta = ktime_sub(apic->lapic_timer.target_expiration, now);
> > -	apic->lapic_timer.tscdeadline = kvm_read_l1_tsc(apic->vcpu, tscl) +
> > -		nsec_to_cycles(apic->vcpu, delta);
> > +	apic->lapic_timer.tscdeadline = kvm_read_l1_tsc(apic->vcpu, tscl);
> > +	/*
> > +	 * Note: delta must not be negative. Otherwise, blindly adding a
> > +	 * negative delta could cause the deadline to become excessively large
> > +	 * due to the tscdeadline being an unsigned value.
> > +	 */
> 
> Rather than say "delta must not be negative", instead call out that the delta
> _can't_ be negative thanks to the above calculation (which is also why moving
> the "period > 0" check to the caller is important).  And I think it's worth
> putting this comment above the calculation of "delta" to preserve the existing
> code that does "tscdead = l1_tsc + delta" (and it yields a smaller diff).

Yes, it's better.

> > +	apic->lapic_timer.tscdeadline += nsec_to_cycles(apic->vcpu, delta);
> >  }
> >  
> >  static void start_sw_period(struct kvm_lapic *apic)
> > @@ -2972,7 +2988,7 @@ static enum hrtimer_restart apic_timer_fn(struct hrtimer *data)
> >  
> >  	if (lapic_is_periodic(apic)) {
> >  		advance_periodic_target_expiration(apic);
> > -		hrtimer_add_expires_ns(&ktimer->timer, ktimer->period);
> > +		hrtimer_set_expires(&ktimer->timer, ktimer->target_expiration);
> 
> This can also go in a separate prep patch.  Logically it stands on its own (use
> the computed expiration instead of doing yet another calculation), and in case
> something goes sideways, it'll give us another bisection point.
> 
> Lastly, adding a cleanup patch on top to capture apic->lapic_timer in a local
> variable would make this code easier to read (I don't want to say "easy" to
> read, but at least easier :-D).

Yes, it's better.

> 
> All in all, I ended up with the below.  I'll post a v6 tomorrow (I've got the
> tweaks made locally) after testing.  Thanks much!
>
> static void advance_periodic_target_expiration(struct kvm_lapic *apic)
> {
> 	struct kvm_timer *ktimer = &apic->lapic_timer;
> 	ktime_t now = ktime_get();
> 	u64 tscl = rdtsc();
> 	ktime_t delta;
> 
> 	/*
> 	 * Use kernel time as the time source for both the hrtimer deadline and
> 	 * TSC-based deadline so that they stay synchronized.  Computing each
> 	 * deadline independently will cause the two deadlines to drift apart
> 	 * over time as differences in the periods accumulate, e.g. due to
> 	 * differences in the underlying clocks or numerical approximation errors.
> 	 */
> 	ktimer->target_expiration = ktime_add_ns(ktimer->target_expiration,
> 						 ktimer->period);
> 
> 	/*
> 	 * If the new expiration is in the past, e.g. because userspace stopped
> 	 * running the VM for an extended duration, then force the expiration
> 	 * to "now" and don't try to play catch-up with the missed events.  KVM
> 	 * will only deliver a single interrupt regardless of how many events
> 	 * are pending, i.e. restarting the timer with an expiration in the
> 	 * past will do nothing more than waste host cycles, and can even lead
> 	 * to a hard lockup in extreme cases.
> 	 */

This comment is a significant improvement over v5...

> 	if (ktime_before(ktimer->target_expiration, now))
> 		ktimer->target_expiration = now;
> 
> 	/*
> 	 * Note, ensuring the expiration isn't in the past also prevents delta
> 	 * from going negative, which could cause the TSC deadline to become
> 	 * excessively large due to it an unsigned value.
> 	 */
> 	delta = ktime_sub(ktimer->target_expiration, now);
> 	ktimer->target_expiration = kvm_read_l1_tsc(apic->vcpu, tscl) +
> 				    nsec_to_cycles(apic->vcpu, delta);
> }

I really appreciate your patient corrections and advice. They have helped me
learn how to submit a good patch and I will also actively learn and abide by
these rules.

Regards,
fuqiang
