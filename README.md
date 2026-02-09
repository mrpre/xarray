# Userspace XArray

A port of the Linux kernel's XArray (`lib/xarray.c`) to userspace.
No kernel source files were modified. All kernel-specific dependencies are
satisfied through stub headers and a compatibility layer in `linux/types.h`.

## Build

```bash
gcc -o test_xarray test_xarray.c xarray.c bitmap.c -I ./ -lpthread
```

## File inventory

### Unmodified kernel source files

These are copied verbatim from the kernel tree and **not modified**:

| File | Kernel path | Description |
|---|---|---|
| `xarray.c` | `lib/xarray.c` | XArray implementation |
| `linux/xarray.h` | `include/linux/xarray.h` | XArray header |
| `linux/bitmap.h` | `include/linux/bitmap.h` | Bitmap operations header |
| `linux/list.h` | `include/linux/list.h` | Linked list header |

### Compatibility layer (has real content)

| File | Description |
|---|---|
| `linux/types.h` | **Core compat header.** Provides all kernel types, macros, and inline functions needed by xarray. Every stub header includes this. See details below. |
| `radix-tree.h` | Provides `radix_tree_node_rcu_free()` — frees an `xa_node` via `container_of` + `free()`. |
| `bitmap.c` | Implements `__bitmap_set()` and `__bitmap_clear()` (called by `bitmap.h` inline wrappers when arguments are not compile-time constants). |

### Stub headers (include-guard + `#include <linux/types.h>` only)

These exist solely to satisfy `#include` directives in the kernel headers.
They contain **no logic** — all definitions come from `linux/types.h`.

| Stub header | Required by |
|---|---|
| `linux/compiler.h` | `linux/xarray.h` |
| `linux/bug.h` | `linux/xarray.h` |
| `linux/err.h` | `linux/xarray.h` |
| `linux/gfp.h` | `linux/xarray.h` |
| `linux/kconfig.h` | `linux/xarray.h` |
| `linux/limits.h` | `linux/xarray.h`, `linux/bitmap.h` |
| `linux/lockdep.h` | `linux/xarray.h` |
| `linux/rcupdate.h` | `linux/xarray.h` |
| `linux/spinlock.h` | `linux/xarray.h` |
| `linux/sched/mm.h` | `linux/xarray.h` |
| `linux/export.h` | `xarray.c` |
| `linux/errno.h` | `linux/bitmap.h` |
| `linux/string.h` | `linux/bitmap.h` |
| `linux/align.h` | `linux/bitmap.h` |
| `linux/bitops.h` | `linux/bitmap.h` |
| `linux/cleanup.h` | `linux/bitmap.h` |
| `linux/find.h` | `linux/bitmap.h` |
| `linux/bitmap-str.h` | `linux/bitmap.h` |
| `linux/container_of.h` | `linux/list.h` |
| `linux/stddef.h` | `linux/list.h` |
| `linux/poison.h` | `linux/list.h` |
| `linux/const.h` | `linux/list.h` |
| `asm/barrier.h` | `linux/list.h` |

## What `linux/types.h` provides

All kernel-specific concepts are mapped to userspace equivalents or
stubbed out in this single file. Summary:

### Kernel feature → Userspace mapping

| Kernel feature | Userspace replacement |
|---|---|
| `spinlock_t`, `spin_lock()` etc. | `pthread_mutex_t`, `pthread_mutex_lock()` etc. |
| `spin_lock_irq()`, `spin_lock_bh()` | Same as `spin_lock()` (no IRQ/BH distinction in userspace) |
| `kmem_cache_alloc_lru()` | `calloc(1, sizeof(struct xa_node))` |
| `radix_tree_node_rcu_free()` | `free()` via `container_of` (in `radix-tree.h`) |
| `BUG()` | `abort()` |
| `pr_info()`, `pr_cont()` | `printf()` |

### Kernel features defined as no-op / empty

| Kernel feature | Defined as | Reason |
|---|---|---|
| `__force`, `__rcu`, `__bitwise` | empty | Sparse annotation attributes, not needed in userspace |
| `__must_hold(x)` | empty | Lock annotation, not needed |
| `__preserve_most` | empty | Calling convention hint, GCC userspace doesn't support it |
| `EXPORT_SYMBOL()`, `EXPORT_SYMBOL_GPL()` | empty | Kernel module symbol export |
| `rcu_read_lock()`, `rcu_read_unlock()` | `do {} while (0)` | No RCU in userspace |
| `rcu_dereference_check(p, c)` | `(p)` | Just return the pointer directly |
| `rcu_dereference_protected(p, c)` | `(p)` | Just return the pointer directly |
| `rcu_dereference_raw(p)` | `(p)` | Just return the pointer directly |
| `rcu_assign_pointer(p, v)` | `(p) = (v)` | Plain assignment |
| `RCU_INIT_POINTER(p, v)` | `(p) = (v)` | Plain assignment |
| `call_rcu(head, func)` | `(func)(head)` | Call the callback immediately (no deferred free) |
| `lockdep_is_held(lock)` | `(1)` | Always "held" — no lock debugging |
| `might_alloc(gfp)` | empty | Sleep-ability check, not needed |
| `gfpflags_allow_blocking(gfp)` | `(1)` | Always allow "blocking" |
| `GFP_KERNEL`, `GFP_NOWAIT`, `__GFP_ACCOUNT` | `0` | GFP flags have no meaning in userspace |
| `DEFINE_FREE(name, type, fn)` | empty | Kernel cleanup attribute, not needed |
| `READ_ONCE(x)` | `(x)` | No need for volatile semantics in single-threaded use |
| `WRITE_ONCE(x, val)` | `(x) = (val)` | Plain assignment |
| `smp_store_release()`, `smp_load_acquire()` | plain read/write | No memory ordering needed |
| `WARN_ON(x)`, `WARN_ON_ONCE(x)` | `(x)` | Evaluates condition but does not print |
| `CONFIG_LIST_HARDENED` | not defined | Disabled via `#ifdef` in `list.h` |
| `CONFIG_DEBUG_LIST` | not defined | Disabled via `#ifdef` in `list.h` |

### Kernel features re-implemented for userspace

| Kernel feature | Implementation |
|---|---|
| `container_of()` | GCC statement expression with `offsetof` |
| `test_bit()`, `__set_bit()`, `__clear_bit()` | Inline bit manipulation on `unsigned long` arrays |
| `__test_and_set_bit()`, `__test_and_clear_bit()` | Non-atomic test-and-modify |
| `find_next_bit()`, `find_first_bit()` | Word-at-a-time scan with `__builtin_ctzl` |
| `find_next_zero_bit()`, `find_first_zero_bit()` | Same approach on inverted words |
| `__ffs()` | `__builtin_ctzl()` |
| `hweight_long()` | `__builtin_popcountl()` |
| `__bitmap_set()`, `__bitmap_clear()` | Implemented in `bitmap.c` |
| `for_each_set_bit()` | Loop macro using `find_first_bit` / `find_next_bit` |

### Config options

| Config | Value | Effect |
|---|---|---|
| `CONFIG_XARRAY_MULTI` | `1` | Enable multi-index entry support |
| `CONFIG_BASE_SMALL` | `0` | Use full-size nodes (`XA_CHUNK_SHIFT=6`, 64 slots per node) |

## Limitations

- **RCU is stubbed out.** All RCU read-side critical sections are no-ops and
  `call_rcu()` calls the free function immediately. This means the xarray is
  **not safe for lockless concurrent reads** as it would be in the kernel.
- **IRQ/BH locking is not distinguished.** All lock variants map to the same
  `pthread_mutex_lock()` call.
- **Memory allocation flags are ignored.** `GFP_KERNEL`, `GFP_NOWAIT`, etc.
  all map to a plain `calloc()`.
