On Fri, Nov 07, 2025, fuqiang wang wrote:
> > =================================
> > Fix both issues in a single patch
> > =================================
> >
> > In versions v2 and v3, I split these two issues into two separate patches
> > for fixing. However, this caused patch 2 to revert some of the changes made
> > by patch 1.
> 
> FWIW, my initial reaction was that I liked splitting this into two patches better,
> but after digging through all the angles of this for a few hours, I agree that it's
> better to skip the "don't let the delta go negative" patch, because that patch
> really only addresses a symptom that shouldn't happen in the first place.

Yes, the reason why I issued the “don’t let the delta go negative” patch
separately in v1 was that I hadn’t yet identified the root cause of the
hardlockup at that time.

> 
> > In patch 4, I attempted to merge the two patches into one and tried to
> > describe both issues in the commit message, but I did not do it well. In
> > this version, I have included more details in the commit message and the
> > cover letter.
