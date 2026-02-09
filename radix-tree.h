/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Userspace stub for radix-tree.h
 * Provides radix_tree_node_rcu_free for xarray.c
 * At this point struct xa_node (from xarray.h) is already defined.
 */
#ifndef _RADIX_TREE_H
#define _RADIX_TREE_H

#include <linux/types.h>

static inline void radix_tree_node_rcu_free(struct rcu_head *head)
{
	struct xa_node *node = container_of(head, struct xa_node, rcu_head);
	free(node);
}

#endif /* _RADIX_TREE_H */
