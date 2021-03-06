/*-
 * Copyright (c) 2002, Jeffrey Roberson <jeff@freebsd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice unmodified, this list of conditions, and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 *
 */

#ifndef _SYS_UMTX_H_
#define	_SYS_UMTX_H_

#include <sys/_umtx.h>

#define USYNC_PROCESS_SHARED	0x0001	/* Process shared sync objs */

#define	UMUTEX_UNOWNED		0x0
#define	UMUTEX_CONTESTED	0x80000000U

#define	UMUTEX_PRIO_INHERIT	0x0004	/* Priority inherited mutex */
#define	UMUTEX_PRIO_PROTECT	0x0008	/* Priority protect mutex */

/* urwlock flags */
#define URWLOCK_PREFER_READER	0x0002

#define URWLOCK_WRITE_OWNER	0x80000000U
#define URWLOCK_WRITE_WAITERS	0x40000000U
#define URWLOCK_READ_WAITERS	0x20000000U
#define URWLOCK_MAX_READERS	0x1fffffffU
#define URWLOCK_READER_COUNT(c)	((c) & URWLOCK_MAX_READERS)

/* _usem flags */
#define SEM_NAMED	0x0002

/* op code for _umtx_op */
#define	UMTX_OP_RESERVED0	0
#define	UMTX_OP_RESERVED1	1
#define	UMTX_OP_WAIT		2
#define	UMTX_OP_WAKE		3
#define	UMTX_OP_MUTEX_TRYLOCK	4
#define	UMTX_OP_MUTEX_LOCK	5
#define	UMTX_OP_MUTEX_UNLOCK	6
#define	UMTX_OP_SET_CEILING	7
#define	UMTX_OP_CV_WAIT		8
#define	UMTX_OP_CV_SIGNAL	9
#define	UMTX_OP_CV_BROADCAST	10
#define	UMTX_OP_WAIT_UINT	11
#define	UMTX_OP_RW_RDLOCK	12
#define	UMTX_OP_RW_WRLOCK	13
#define	UMTX_OP_RW_UNLOCK	14
#define	UMTX_OP_WAIT_UINT_PRIVATE	15
#define	UMTX_OP_WAKE_PRIVATE	16
#define	UMTX_OP_MUTEX_WAIT	17
#define	UMTX_OP_MUTEX_WAKE	18	/* deprecated */
#define	UMTX_OP_SEM_WAIT	19
#define	UMTX_OP_SEM_WAKE	20
#define	UMTX_OP_NWAKE_PRIVATE   21
#define	UMTX_OP_MUTEX_WAKE2	22
#define	UMTX_OP_MAX		23

/* Flags for UMTX_OP_CV_WAIT */
#define	CVWAIT_CHECK_UNPARKING	0x01
#define	CVWAIT_ABSTIME		0x02
#define	CVWAIT_CLOCKID		0x04

#define	UMTX_ABSTIME		0x01

#define	UMTX_CHECK_UNPARKING	CVWAIT_CHECK_UNPARKING

#ifndef _KERNEL

int _umtx_op(void *obj, int op, u_long val, void *uaddr, void *uaddr2);

#else

/*
 * The umtx_key structure is used by both the Linux futex code and the
 * umtx implementation to map userland addresses to unique keys.
 */

enum {
	TYPE_SIMPLE_WAIT,
	TYPE_CV,
	TYPE_SEM,
	TYPE_SIMPLE_LOCK,
	TYPE_NORMAL_UMUTEX,
	TYPE_PI_UMUTEX,
	TYPE_PP_UMUTEX,
	TYPE_RWLOCK,
	TYPE_FUTEX
};

/* Key to represent a unique userland synchronous object */
struct umtx_key {
	int	hash;
	int	type;
	int	shared;
	union {
		struct {
			struct vm_object *object;
			uintptr_t	offset;
		} shared;
		struct {
			struct vmspace	*vs;
			uintptr_t	addr;
		} private;
		struct {
			void		*a;
			uintptr_t	b;
		} both;
	} info;
};

#define THREAD_SHARE		0
#define PROCESS_SHARE		1
#define AUTO_SHARE		2

struct thread;

static inline int
umtx_key_match(const struct umtx_key *k1, const struct umtx_key *k2)
{
	return (k1->type == k2->type &&
		k1->info.both.a == k2->info.both.a &&
	        k1->info.both.b == k2->info.both.b);
}

int umtx_copyin_timeout(const void *, struct timespec *);
int umtx_key_get(void *, int, int, struct umtx_key *);
void umtx_key_release(struct umtx_key *);
struct umtx_q *umtxq_alloc(void);
void umtxq_free(struct umtx_q *);
int kern_umtx_wake(struct thread *, void *, int, int);
void umtx_pi_adjust(struct thread *, u_char);
void umtx_thread_init(struct thread *);
void umtx_thread_fini(struct thread *);
void umtx_thread_alloc(struct thread *);
void umtx_thread_exit(struct thread *);
#endif /* !_KERNEL */
#endif /* !_SYS_UMTX_H_ */
