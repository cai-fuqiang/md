chen.yu@easystack.cn

Hi, Marcelo

We notice in the email you sent
 [PATCH] KVM: x86: fix hardlockup due to LAPIC hrtimer period drift
at [1].

We have also encountered a similar issue and currently have no clue how to
resolve it. After reviewing the patch you sent, we still have some other
questions we would like to ask.

> Timer interrupt handlers get to execute a few microseconds after
> their deadline, while the exact period is added to the hrtimer
> expiration time. With large uptimes, this causes the hrtimer 
> expiration time to become much smaller than the actual
> timer interrupt base:

I have also noticed that there is an issue with the way target_expiration is
advanced. However, what I can’t figure out is under what circumstances such a
large delta could occur, causing hrtimer_interrupt to loop repeatedly and
eventually result in a hard lockup.

We make the following assumptions: when hrtimer_interrupt is executed this time,
target_expiration_0 = now_0 - n * period. Suppose that during this round of
execution, target_expiration_0 > now_0 after completing n + 1 iterations.

  now_1 = now_0 + delay * (n + 1)
  target_expiration_1 = target_expiration_0 + (n + 1) * period

So,
  (now_1 - target_expiration_1) - (now_0 - target_expiration_0)
  = (now_1 - now_0) - (target_expiration_1 - target_expiration_0)
  = (n + 1) * (delay - period)

Only when delay is greater than period will the gap between target_expiration
and now keep increasing, which in turn causes n to become larger and leads to
prolonged execution of timer_interrupt, potentially resulting in a hard lockup.

However, in our actual tests, the value of min_timer_period_us is 200, i.e.
200us, which makes such a large delay unlikely. Especially when now >>
target_expiration, it will continuously execute apic_timer_fn without triggering
an interrupt, and such delay should be very small, thereby narrowing the gap
between now and target_expiration.

THX

fuqiang

[1]. https://lore.kernel.org/kvm/YgahsSubOgFtyorl@fuller.cnet/
