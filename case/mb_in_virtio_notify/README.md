## some commit
```
commit be1fea9bc286f64c6c995bb0d7145a0b738aeddb
Author: Vincenzo Maffione <v.maffione@gmail.com>
Date:   Sun Jan 31 11:29:05 2016 +0100

    virtio: read avail_idx from VQ only when necessary

    The virtqueue_pop() implementation needs to check if the avail ring
    contains some pending buffers. To perform this check, it is not
    always necessary to fetch the avail_idx in the VQ memory, which is
    expensive. This patch introduces a shadow variable tracking avail_idx
    and modifies virtio_queue_empty() to access avail_idx in physical
    memory only when necessary.

commit 97cd965c070152bc626c7507df9fb356bbe1cd81
Author: Paolo Bonzini <pbonzini@redhat.com>
Date:   Fri Jan 27 16:40:20 2017 +0100

    virtio: use VRingMemoryRegionCaches for avail and used rings
```

