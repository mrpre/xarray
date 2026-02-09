/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

/*
 * Userspace compatibility header for kernel types.
 * Provides all fundamental types and macros needed by xarray.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <errno.h>
#include <pthread.h>

/*
 * The kernel uses 'errno' as a local variable name in some places
 * (e.g., __xa_insert). System <errno.h> defines it as a macro.
 * Undefine it so kernel code can use it as a variable name.
 */
#undef errno

/* ======================== Basic integer types ======================== */
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

/* ======================== Bit widths ======================== */
#if __SIZEOF_LONG__ == 8
#define BITS_PER_LONG       64
#define __BITS_PER_LONG     64
#else
#define BITS_PER_LONG       32
#define __BITS_PER_LONG     32
#endif

#define BITS_PER_BYTE       8
#define BITS_PER_TYPE(type) (sizeof(type) * BITS_PER_BYTE)
#define BITS_TO_LONGS(nr)   (((nr) + BITS_PER_LONG - 1) / BITS_PER_LONG)
#define BIT_WORD(nr)        ((nr) / BITS_PER_LONG)
#define BIT(nr)             (1UL << (nr))
#define DECLARE_BITMAP(name, bits) unsigned long name[BITS_TO_LONGS(bits)]

/* ======================== Compiler attributes ======================== */
#undef __always_inline
#define __always_inline     inline __attribute__((always_inline))
#define __force
#define __rcu
#define __bitwise
#define __pure              __attribute__((pure))
#define __cold              __attribute__((cold))
#define __preserve_most
#define __must_check        __attribute__((warn_unused_result))
#define __must_hold(x)
#define __list_valid_slowpath

#define likely(x)           __builtin_expect(!!(x), 1)
#define unlikely(x)         __builtin_expect(!!(x), 0)

/* ======================== Alignment / bit macros ======================== */
#define ALIGN(x, a)         (((x) + ((a) - 1)) & ~((a) - 1))
#define IS_ALIGNED(x, a)    (((x) & ((a) - 1)) == 0)
#define GENMASK(h, l)       (((~0UL) << (l)) & (~0UL >> (BITS_PER_LONG - 1 - (h))))

#ifndef max
#define max(a, b)           ((a) > (b) ? (a) : (b))
#endif
#ifndef min
#define min(a, b)           ((a) < (b) ? (a) : (b))
#endif

/* small_const_nbits(n) - true if n is known at compile-time and <= BITS_PER_LONG */
#define small_const_nbits(nbits) \
	(__builtin_constant_p(nbits) && (nbits) <= BITS_PER_LONG && (nbits) > 0)

/* ======================== Error numbers ======================== */
/* Re-define commonly used error numbers (system errno.h was #undef'd above) */
#ifndef ENOMEM
#define ENOMEM  12
#endif
#ifndef EBUSY
#define EBUSY   16
#endif
#ifndef EINVAL
#define EINVAL  22
#endif
#define MAX_ERRNO   4095

/* ======================== GFP types and flags ======================== */
typedef unsigned int gfp_t;
#define GFP_KERNEL      0
#define GFP_NOWAIT      0
#define __GFP_ACCOUNT   0
#define __GFP_BITS_SHIFT 26

#define gfpflags_allow_blocking(gfp) (1)
#define might_alloc(gfp)

/* ======================== Spinlock (pthread_mutex) ======================== */
typedef pthread_mutex_t spinlock_t;

#define __SPIN_LOCK_UNLOCKED(name)  PTHREAD_MUTEX_INITIALIZER
#define spin_lock_init(lock)        pthread_mutex_init(lock, NULL)
#define spin_lock(lock)             pthread_mutex_lock(lock)
#define spin_unlock(lock)           pthread_mutex_unlock(lock)
#define spin_trylock(lock)          (!pthread_mutex_trylock(lock))
#define spin_lock_bh(lock)          pthread_mutex_lock(lock)
#define spin_unlock_bh(lock)        pthread_mutex_unlock(lock)
#define spin_lock_irq(lock)         pthread_mutex_lock(lock)
#define spin_unlock_irq(lock)       pthread_mutex_unlock(lock)
#define spin_lock_irqsave(lock, flags) \
	do { (void)(flags); pthread_mutex_lock(lock); } while (0)
#define spin_unlock_irqrestore(lock, flags) \
	do { (void)(flags); pthread_mutex_unlock(lock); } while (0)
#define spin_lock_nested(lock, subclass)         pthread_mutex_lock(lock)
#define spin_lock_bh_nested(lock, subclass)      pthread_mutex_lock(lock)
#define spin_lock_irq_nested(lock, subclass)     pthread_mutex_lock(lock)
#define spin_lock_irqsave_nested(lock, flags, subclass) \
	do { (void)(flags); pthread_mutex_lock(lock); } while (0)

/* ======================== RCU stubs ======================== */
struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *);
};

#define rcu_read_lock()                 do {} while (0)
#define rcu_read_unlock()               do {} while (0)
#define rcu_dereference_check(p, c)     (p)
#define rcu_dereference_protected(p, c) (p)
#define rcu_dereference_raw(p)          (p)
#define rcu_assign_pointer(p, v)        ((p) = (v))
#define RCU_INIT_POINTER(p, v)          ((p) = (v))
#define call_rcu(head, func)            (func)(head)

/* ======================== List types ======================== */
struct list_head {
	struct list_head *next, *prev;
};

struct hlist_head {
	struct hlist_node *first;
};

struct hlist_node {
	struct hlist_node *next, **pprev;
};

/* ======================== Lockdep stubs ======================== */
#define lockdep_is_held(lock)   (1)

/* ======================== Bug / Warn ======================== */
#define BUG()               abort()
#define BUG_ON(x)           do { if (x) abort(); } while (0)
#define WARN_ON(x)          (x)
#define WARN_ON_ONCE(x)     (x)
#define WARN_ONCE(x, ...)   (x)

/* ======================== Print ======================== */
#define pr_info(fmt, ...)   printf(fmt, ##__VA_ARGS__)
#define pr_cont(fmt, ...)   printf(fmt, ##__VA_ARGS__)

/* ======================== Export ======================== */
#define EXPORT_SYMBOL(sym)
#define EXPORT_SYMBOL_GPL(sym)

/* ======================== Config ======================== */
#define CONFIG_XARRAY_MULTI 1
#define CONFIG_BASE_SMALL   0
/* CONFIG_LIST_HARDENED - intentionally NOT defined */
/* CONFIG_DEBUG_LIST    - intentionally NOT defined */

/*
 * IS_ENABLED - userspace version.
 * Works when CONFIG_FOO is defined to 0 or 1.
 */
#define IS_ENABLED(option)  (!!(option))

/* ======================== container_of ======================== */
#define container_of(ptr, type, member) ({          \
	const typeof(((type *)0)->member) *__mptr = (ptr);  \
	(type *)((char *)__mptr - offsetof(type, member)); })

/* ======================== READ_ONCE / WRITE_ONCE ======================== */
#define READ_ONCE(x)              (x)
#define WRITE_ONCE(x, val)        ((x) = (val))
#define smp_store_release(p, v)   (*(p) = (v))
#define smp_load_acquire(p)       (*(p))

/* ======================== List poison ======================== */
#define LIST_POISON1  ((void *) 0x100)
#define LIST_POISON2  ((void *) 0x200)

/* ======================== Cleanup (DEFINE_FREE) ======================== */
#define DEFINE_FREE(name, type, free_fn)

/* ======================== Bitops ======================== */

static __always_inline unsigned long __ffs(unsigned long word)
{
	return __builtin_ctzl(word);
}

static __always_inline unsigned int hweight_long(unsigned long w)
{
	return __builtin_popcountl(w);
}

static __always_inline int test_bit(unsigned long nr, const volatile unsigned long *addr)
{
	return 1UL & (addr[BIT_WORD(nr)] >> (nr & (BITS_PER_LONG - 1)));
}

static __always_inline void __set_bit(unsigned long nr, volatile unsigned long *addr)
{
	addr[BIT_WORD(nr)] |= 1UL << (nr & (BITS_PER_LONG - 1));
}

static __always_inline void __clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	addr[BIT_WORD(nr)] &= ~(1UL << (nr & (BITS_PER_LONG - 1)));
}

static __always_inline int __test_and_set_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = 1UL << (nr & (BITS_PER_LONG - 1));
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;
	*p = old | mask;
	return (old & mask) != 0;
}

static __always_inline int __test_and_clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long mask = 1UL << (nr & (BITS_PER_LONG - 1));
	unsigned long *p = ((unsigned long *)addr) + BIT_WORD(nr);
	unsigned long old = *p;
	*p = old & ~mask;
	return (old & mask) != 0;
}

static __always_inline void __assign_bit(unsigned long nr, volatile unsigned long *addr, bool value)
{
	if (value)
		__set_bit(nr, addr);
	else
		__clear_bit(nr, addr);
}

/* ======================== Find bit operations ======================== */

static __always_inline unsigned long
find_next_bit(const unsigned long *addr, unsigned long size, unsigned long offset)
{
	unsigned long tmp;

	if (offset >= size)
		return size;

	/* Handle the first partial word */
	tmp = addr[BIT_WORD(offset)] & (~0UL << (offset & (BITS_PER_LONG - 1)));
	offset = (offset / BITS_PER_LONG) * BITS_PER_LONG;

	while (!tmp) {
		offset += BITS_PER_LONG;
		if (offset >= size)
			return size;
		tmp = addr[BIT_WORD(offset)];
	}
	return min(offset + __ffs(tmp), size);
}

static __always_inline unsigned long
find_first_bit(const unsigned long *addr, unsigned long size)
{
	return find_next_bit(addr, size, 0);
}

static __always_inline unsigned long
find_next_zero_bit(const unsigned long *addr, unsigned long size, unsigned long offset)
{
	unsigned long tmp;

	if (offset >= size)
		return size;

	tmp = ~addr[BIT_WORD(offset)] & (~0UL << (offset & (BITS_PER_LONG - 1)));
	offset = (offset / BITS_PER_LONG) * BITS_PER_LONG;

	while (!tmp) {
		offset += BITS_PER_LONG;
		if (offset >= size)
			return size;
		tmp = ~addr[BIT_WORD(offset)];
	}
	return min(offset + __ffs(tmp), size);
}

static __always_inline unsigned long
find_first_zero_bit(const unsigned long *addr, unsigned long size)
{
	return find_next_zero_bit(addr, size, 0);
}

#define for_each_set_bit(bit, addr, size) \
	for ((bit) = find_first_bit((addr), (size)); \
	     (bit) < (size); \
	     (bit) = find_next_bit((addr), (size), (bit) + 1))

/* ======================== Memory allocation (xa_node) ======================== */
/*
 * In the kernel, xa_node is allocated from a slab cache (radix_tree_node_cachep).
 * In userspace, we use calloc/free.
 * Note: these macros are expanded at the point of use, where struct xa_node
 * is already defined (after xarray.h inclusion).
 */
#define radix_tree_node_cachep  ((void *)0)
#define kmem_cache_alloc_lru(cachep, lru, gfp) ({			\
	struct xa_node *___n = calloc(1, sizeof(struct xa_node));	\
	if (___n)							\
		INIT_LIST_HEAD(&___n->private_list);			\
	___n;								\
})

/* radix_tree_node_rcu_free is defined as a static inline in radix-tree.h */

#endif /* _LINUX_TYPES_H */
