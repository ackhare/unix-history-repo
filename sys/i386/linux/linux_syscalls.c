/*
 * System call names.
 *
 * DO NOT EDIT-- this file is automatically generated.
 * $FreeBSD$
 * created from FreeBSD: head/sys/i386/linux/syscalls.master 283451 2015-05-24 16:59:25Z dchagin 
 */

const char *linux_syscallnames[] = {
#define	nosys	linux_nosys
	"#0",			/* 0 = setup */
	"linux_exit",			/* 1 = linux_exit */
	"linux_fork",			/* 2 = linux_fork */
	"read",			/* 3 = read */
	"write",			/* 4 = write */
	"linux_open",			/* 5 = linux_open */
	"close",			/* 6 = close */
	"linux_waitpid",			/* 7 = linux_waitpid */
	"linux_creat",			/* 8 = linux_creat */
	"linux_link",			/* 9 = linux_link */
	"linux_unlink",			/* 10 = linux_unlink */
	"linux_execve",			/* 11 = linux_execve */
	"linux_chdir",			/* 12 = linux_chdir */
	"linux_time",			/* 13 = linux_time */
	"linux_mknod",			/* 14 = linux_mknod */
	"linux_chmod",			/* 15 = linux_chmod */
	"linux_lchown16",			/* 16 = linux_lchown16 */
	"#17",			/* 17 = break */
	"linux_stat",			/* 18 = linux_stat */
	"linux_lseek",			/* 19 = linux_lseek */
	"linux_getpid",			/* 20 = linux_getpid */
	"linux_mount",			/* 21 = linux_mount */
	"linux_oldumount",			/* 22 = linux_oldumount */
	"linux_setuid16",			/* 23 = linux_setuid16 */
	"linux_getuid16",			/* 24 = linux_getuid16 */
	"linux_stime",			/* 25 = linux_stime */
	"linux_ptrace",			/* 26 = linux_ptrace */
	"linux_alarm",			/* 27 = linux_alarm */
	"linux_fstat",			/* 28 = linux_fstat */
	"linux_pause",			/* 29 = linux_pause */
	"linux_utime",			/* 30 = linux_utime */
	"#31",			/* 31 = stty */
	"#32",			/* 32 = gtty */
	"linux_access",			/* 33 = linux_access */
	"linux_nice",			/* 34 = linux_nice */
	"#35",			/* 35 = ftime */
	"sync",			/* 36 = sync */
	"linux_kill",			/* 37 = linux_kill */
	"linux_rename",			/* 38 = linux_rename */
	"linux_mkdir",			/* 39 = linux_mkdir */
	"linux_rmdir",			/* 40 = linux_rmdir */
	"dup",			/* 41 = dup */
	"linux_pipe",			/* 42 = linux_pipe */
	"linux_times",			/* 43 = linux_times */
	"#44",			/* 44 = prof */
	"linux_brk",			/* 45 = linux_brk */
	"linux_setgid16",			/* 46 = linux_setgid16 */
	"linux_getgid16",			/* 47 = linux_getgid16 */
	"linux_signal",			/* 48 = linux_signal */
	"linux_geteuid16",			/* 49 = linux_geteuid16 */
	"linux_getegid16",			/* 50 = linux_getegid16 */
	"acct",			/* 51 = acct */
	"linux_umount",			/* 52 = linux_umount */
	"#53",			/* 53 = lock */
	"linux_ioctl",			/* 54 = linux_ioctl */
	"linux_fcntl",			/* 55 = linux_fcntl */
	"#56",			/* 56 = mpx */
	"setpgid",			/* 57 = setpgid */
	"#58",			/* 58 = ulimit */
	"linux_olduname",			/* 59 = linux_olduname */
	"umask",			/* 60 = umask */
	"chroot",			/* 61 = chroot */
	"linux_ustat",			/* 62 = linux_ustat */
	"dup2",			/* 63 = dup2 */
	"linux_getppid",			/* 64 = linux_getppid */
	"getpgrp",			/* 65 = getpgrp */
	"setsid",			/* 66 = setsid */
	"linux_sigaction",			/* 67 = linux_sigaction */
	"linux_sgetmask",			/* 68 = linux_sgetmask */
	"linux_ssetmask",			/* 69 = linux_ssetmask */
	"linux_setreuid16",			/* 70 = linux_setreuid16 */
	"linux_setregid16",			/* 71 = linux_setregid16 */
	"linux_sigsuspend",			/* 72 = linux_sigsuspend */
	"linux_sigpending",			/* 73 = linux_sigpending */
	"linux_sethostname",			/* 74 = linux_sethostname */
	"linux_setrlimit",			/* 75 = linux_setrlimit */
	"linux_old_getrlimit",			/* 76 = linux_old_getrlimit */
	"getrusage",			/* 77 = getrusage */
	"gettimeofday",			/* 78 = gettimeofday */
	"settimeofday",			/* 79 = settimeofday */
	"linux_getgroups16",			/* 80 = linux_getgroups16 */
	"linux_setgroups16",			/* 81 = linux_setgroups16 */
	"linux_old_select",			/* 82 = linux_old_select */
	"linux_symlink",			/* 83 = linux_symlink */
	"linux_lstat",			/* 84 = linux_lstat */
	"linux_readlink",			/* 85 = linux_readlink */
	"linux_uselib",			/* 86 = linux_uselib */
	"swapon",			/* 87 = swapon */
	"linux_reboot",			/* 88 = linux_reboot */
	"linux_readdir",			/* 89 = linux_readdir */
	"linux_mmap",			/* 90 = linux_mmap */
	"munmap",			/* 91 = munmap */
	"linux_truncate",			/* 92 = linux_truncate */
	"linux_ftruncate",			/* 93 = linux_ftruncate */
	"fchmod",			/* 94 = fchmod */
	"fchown",			/* 95 = fchown */
	"linux_getpriority",			/* 96 = linux_getpriority */
	"setpriority",			/* 97 = setpriority */
	"#98",			/* 98 = profil */
	"linux_statfs",			/* 99 = linux_statfs */
	"linux_fstatfs",			/* 100 = linux_fstatfs */
	"linux_ioperm",			/* 101 = linux_ioperm */
	"linux_socketcall",			/* 102 = linux_socketcall */
	"linux_syslog",			/* 103 = linux_syslog */
	"linux_setitimer",			/* 104 = linux_setitimer */
	"linux_getitimer",			/* 105 = linux_getitimer */
	"linux_newstat",			/* 106 = linux_newstat */
	"linux_newlstat",			/* 107 = linux_newlstat */
	"linux_newfstat",			/* 108 = linux_newfstat */
	"linux_uname",			/* 109 = linux_uname */
	"linux_iopl",			/* 110 = linux_iopl */
	"linux_vhangup",			/* 111 = linux_vhangup */
	"#112",			/* 112 = idle */
	"linux_vm86old",			/* 113 = linux_vm86old */
	"linux_wait4",			/* 114 = linux_wait4 */
	"linux_swapoff",			/* 115 = linux_swapoff */
	"linux_sysinfo",			/* 116 = linux_sysinfo */
	"linux_ipc",			/* 117 = linux_ipc */
	"fsync",			/* 118 = fsync */
	"linux_sigreturn",			/* 119 = linux_sigreturn */
	"linux_clone",			/* 120 = linux_clone */
	"linux_setdomainname",			/* 121 = linux_setdomainname */
	"linux_newuname",			/* 122 = linux_newuname */
	"linux_modify_ldt",			/* 123 = linux_modify_ldt */
	"linux_adjtimex",			/* 124 = linux_adjtimex */
	"linux_mprotect",			/* 125 = linux_mprotect */
	"linux_sigprocmask",			/* 126 = linux_sigprocmask */
	"linux_create_module",			/* 127 = linux_create_module */
	"linux_init_module",			/* 128 = linux_init_module */
	"linux_delete_module",			/* 129 = linux_delete_module */
	"linux_get_kernel_syms",			/* 130 = linux_get_kernel_syms */
	"linux_quotactl",			/* 131 = linux_quotactl */
	"getpgid",			/* 132 = getpgid */
	"fchdir",			/* 133 = fchdir */
	"linux_bdflush",			/* 134 = linux_bdflush */
	"linux_sysfs",			/* 135 = linux_sysfs */
	"linux_personality",			/* 136 = linux_personality */
	"#137",			/* 137 = afs_syscall */
	"linux_setfsuid16",			/* 138 = linux_setfsuid16 */
	"linux_setfsgid16",			/* 139 = linux_setfsgid16 */
	"linux_llseek",			/* 140 = linux_llseek */
	"linux_getdents",			/* 141 = linux_getdents */
	"linux_select",			/* 142 = linux_select */
	"flock",			/* 143 = flock */
	"linux_msync",			/* 144 = linux_msync */
	"readv",			/* 145 = readv */
	"writev",			/* 146 = writev */
	"linux_getsid",			/* 147 = linux_getsid */
	"linux_fdatasync",			/* 148 = linux_fdatasync */
	"linux_sysctl",			/* 149 = linux_sysctl */
	"mlock",			/* 150 = mlock */
	"munlock",			/* 151 = munlock */
	"mlockall",			/* 152 = mlockall */
	"munlockall",			/* 153 = munlockall */
	"linux_sched_setparam",			/* 154 = linux_sched_setparam */
	"linux_sched_getparam",			/* 155 = linux_sched_getparam */
	"linux_sched_setscheduler",			/* 156 = linux_sched_setscheduler */
	"linux_sched_getscheduler",			/* 157 = linux_sched_getscheduler */
	"sched_yield",			/* 158 = sched_yield */
	"linux_sched_get_priority_max",			/* 159 = linux_sched_get_priority_max */
	"linux_sched_get_priority_min",			/* 160 = linux_sched_get_priority_min */
	"linux_sched_rr_get_interval",			/* 161 = linux_sched_rr_get_interval */
	"linux_nanosleep",			/* 162 = linux_nanosleep */
	"linux_mremap",			/* 163 = linux_mremap */
	"linux_setresuid16",			/* 164 = linux_setresuid16 */
	"linux_getresuid16",			/* 165 = linux_getresuid16 */
	"linux_vm86",			/* 166 = linux_vm86 */
	"linux_query_module",			/* 167 = linux_query_module */
	"poll",			/* 168 = poll */
	"linux_nfsservctl",			/* 169 = linux_nfsservctl */
	"linux_setresgid16",			/* 170 = linux_setresgid16 */
	"linux_getresgid16",			/* 171 = linux_getresgid16 */
	"linux_prctl",			/* 172 = linux_prctl */
	"linux_rt_sigreturn",			/* 173 = linux_rt_sigreturn */
	"linux_rt_sigaction",			/* 174 = linux_rt_sigaction */
	"linux_rt_sigprocmask",			/* 175 = linux_rt_sigprocmask */
	"linux_rt_sigpending",			/* 176 = linux_rt_sigpending */
	"linux_rt_sigtimedwait",			/* 177 = linux_rt_sigtimedwait */
	"linux_rt_sigqueueinfo",			/* 178 = linux_rt_sigqueueinfo */
	"linux_rt_sigsuspend",			/* 179 = linux_rt_sigsuspend */
	"linux_pread",			/* 180 = linux_pread */
	"linux_pwrite",			/* 181 = linux_pwrite */
	"linux_chown16",			/* 182 = linux_chown16 */
	"linux_getcwd",			/* 183 = linux_getcwd */
	"linux_capget",			/* 184 = linux_capget */
	"linux_capset",			/* 185 = linux_capset */
	"linux_sigaltstack",			/* 186 = linux_sigaltstack */
	"linux_sendfile",			/* 187 = linux_sendfile */
	"#188",			/* 188 = getpmsg */
	"#189",			/* 189 = putpmsg */
	"linux_vfork",			/* 190 = linux_vfork */
	"linux_getrlimit",			/* 191 = linux_getrlimit */
	"linux_mmap2",			/* 192 = linux_mmap2 */
	"linux_truncate64",			/* 193 = linux_truncate64 */
	"linux_ftruncate64",			/* 194 = linux_ftruncate64 */
	"linux_stat64",			/* 195 = linux_stat64 */
	"linux_lstat64",			/* 196 = linux_lstat64 */
	"linux_fstat64",			/* 197 = linux_fstat64 */
	"linux_lchown",			/* 198 = linux_lchown */
	"linux_getuid",			/* 199 = linux_getuid */
	"linux_getgid",			/* 200 = linux_getgid */
	"geteuid",			/* 201 = geteuid */
	"getegid",			/* 202 = getegid */
	"setreuid",			/* 203 = setreuid */
	"setregid",			/* 204 = setregid */
	"linux_getgroups",			/* 205 = linux_getgroups */
	"linux_setgroups",			/* 206 = linux_setgroups */
	"fchown",			/* 207 = fchown */
	"setresuid",			/* 208 = setresuid */
	"getresuid",			/* 209 = getresuid */
	"setresgid",			/* 210 = setresgid */
	"getresgid",			/* 211 = getresgid */
	"linux_chown",			/* 212 = linux_chown */
	"setuid",			/* 213 = setuid */
	"setgid",			/* 214 = setgid */
	"linux_setfsuid",			/* 215 = linux_setfsuid */
	"linux_setfsgid",			/* 216 = linux_setfsgid */
	"linux_pivot_root",			/* 217 = linux_pivot_root */
	"linux_mincore",			/* 218 = linux_mincore */
	"madvise",			/* 219 = madvise */
	"linux_getdents64",			/* 220 = linux_getdents64 */
	"linux_fcntl64",			/* 221 = linux_fcntl64 */
	"#222",			/* 222 =  */
	"#223",			/* 223 =  */
	"linux_gettid",			/* 224 = linux_gettid */
	"#225",			/* 225 = linux_readahead */
	"linux_setxattr",			/* 226 = linux_setxattr */
	"linux_lsetxattr",			/* 227 = linux_lsetxattr */
	"linux_fsetxattr",			/* 228 = linux_fsetxattr */
	"linux_getxattr",			/* 229 = linux_getxattr */
	"linux_lgetxattr",			/* 230 = linux_lgetxattr */
	"linux_fgetxattr",			/* 231 = linux_fgetxattr */
	"linux_listxattr",			/* 232 = linux_listxattr */
	"linux_llistxattr",			/* 233 = linux_llistxattr */
	"linux_flistxattr",			/* 234 = linux_flistxattr */
	"linux_removexattr",			/* 235 = linux_removexattr */
	"linux_lremovexattr",			/* 236 = linux_lremovexattr */
	"linux_fremovexattr",			/* 237 = linux_fremovexattr */
	"linux_tkill",			/* 238 = linux_tkill */
	"#239",			/* 239 = linux_sendfile64 */
	"linux_sys_futex",			/* 240 = linux_sys_futex */
	"linux_sched_setaffinity",			/* 241 = linux_sched_setaffinity */
	"linux_sched_getaffinity",			/* 242 = linux_sched_getaffinity */
	"linux_set_thread_area",			/* 243 = linux_set_thread_area */
	"linux_get_thread_area",			/* 244 = linux_get_thread_area */
	"#245",			/* 245 = linux_io_setup */
	"#246",			/* 246 = linux_io_destroy */
	"#247",			/* 247 = linux_io_getevents */
	"#248",			/* 248 = linux_io_submit */
	"#249",			/* 249 = linux_io_cancel */
	"linux_fadvise64",			/* 250 = linux_fadvise64 */
	"#251",			/* 251 =  */
	"linux_exit_group",			/* 252 = linux_exit_group */
	"linux_lookup_dcookie",			/* 253 = linux_lookup_dcookie */
	"linux_epoll_create",			/* 254 = linux_epoll_create */
	"linux_epoll_ctl",			/* 255 = linux_epoll_ctl */
	"linux_epoll_wait",			/* 256 = linux_epoll_wait */
	"linux_remap_file_pages",			/* 257 = linux_remap_file_pages */
	"linux_set_tid_address",			/* 258 = linux_set_tid_address */
	"linux_timer_create",			/* 259 = linux_timer_create */
	"linux_timer_settime",			/* 260 = linux_timer_settime */
	"linux_timer_gettime",			/* 261 = linux_timer_gettime */
	"linux_timer_getoverrun",			/* 262 = linux_timer_getoverrun */
	"linux_timer_delete",			/* 263 = linux_timer_delete */
	"linux_clock_settime",			/* 264 = linux_clock_settime */
	"linux_clock_gettime",			/* 265 = linux_clock_gettime */
	"linux_clock_getres",			/* 266 = linux_clock_getres */
	"linux_clock_nanosleep",			/* 267 = linux_clock_nanosleep */
	"linux_statfs64",			/* 268 = linux_statfs64 */
	"linux_fstatfs64",			/* 269 = linux_fstatfs64 */
	"linux_tgkill",			/* 270 = linux_tgkill */
	"linux_utimes",			/* 271 = linux_utimes */
	"linux_fadvise64_64",			/* 272 = linux_fadvise64_64 */
	"#273",			/* 273 = vserver */
	"linux_mbind",			/* 274 = linux_mbind */
	"linux_get_mempolicy",			/* 275 = linux_get_mempolicy */
	"linux_set_mempolicy",			/* 276 = linux_set_mempolicy */
	"linux_mq_open",			/* 277 = linux_mq_open */
	"linux_mq_unlink",			/* 278 = linux_mq_unlink */
	"linux_mq_timedsend",			/* 279 = linux_mq_timedsend */
	"linux_mq_timedreceive",			/* 280 = linux_mq_timedreceive */
	"linux_mq_notify",			/* 281 = linux_mq_notify */
	"linux_mq_getsetattr",			/* 282 = linux_mq_getsetattr */
	"linux_kexec_load",			/* 283 = linux_kexec_load */
	"linux_waitid",			/* 284 = linux_waitid */
	"#285",			/* 285 =  */
	"linux_add_key",			/* 286 = linux_add_key */
	"linux_request_key",			/* 287 = linux_request_key */
	"linux_keyctl",			/* 288 = linux_keyctl */
	"linux_ioprio_set",			/* 289 = linux_ioprio_set */
	"linux_ioprio_get",			/* 290 = linux_ioprio_get */
	"linux_inotify_init",			/* 291 = linux_inotify_init */
	"linux_inotify_add_watch",			/* 292 = linux_inotify_add_watch */
	"linux_inotify_rm_watch",			/* 293 = linux_inotify_rm_watch */
	"linux_migrate_pages",			/* 294 = linux_migrate_pages */
	"linux_openat",			/* 295 = linux_openat */
	"linux_mkdirat",			/* 296 = linux_mkdirat */
	"linux_mknodat",			/* 297 = linux_mknodat */
	"linux_fchownat",			/* 298 = linux_fchownat */
	"linux_futimesat",			/* 299 = linux_futimesat */
	"linux_fstatat64",			/* 300 = linux_fstatat64 */
	"linux_unlinkat",			/* 301 = linux_unlinkat */
	"linux_renameat",			/* 302 = linux_renameat */
	"linux_linkat",			/* 303 = linux_linkat */
	"linux_symlinkat",			/* 304 = linux_symlinkat */
	"linux_readlinkat",			/* 305 = linux_readlinkat */
	"linux_fchmodat",			/* 306 = linux_fchmodat */
	"linux_faccessat",			/* 307 = linux_faccessat */
	"linux_pselect6",			/* 308 = linux_pselect6 */
	"linux_ppoll",			/* 309 = linux_ppoll */
	"linux_unshare",			/* 310 = linux_unshare */
	"linux_set_robust_list",			/* 311 = linux_set_robust_list */
	"linux_get_robust_list",			/* 312 = linux_get_robust_list */
	"linux_splice",			/* 313 = linux_splice */
	"linux_sync_file_range",			/* 314 = linux_sync_file_range */
	"linux_tee",			/* 315 = linux_tee */
	"linux_vmsplice",			/* 316 = linux_vmsplice */
	"linux_move_pages",			/* 317 = linux_move_pages */
	"linux_getcpu",			/* 318 = linux_getcpu */
	"linux_epoll_pwait",			/* 319 = linux_epoll_pwait */
	"linux_utimensat",			/* 320 = linux_utimensat */
	"linux_signalfd",			/* 321 = linux_signalfd */
	"linux_timerfd_create",			/* 322 = linux_timerfd_create */
	"linux_eventfd",			/* 323 = linux_eventfd */
	"linux_fallocate",			/* 324 = linux_fallocate */
	"linux_timerfd_settime",			/* 325 = linux_timerfd_settime */
	"linux_timerfd_gettime",			/* 326 = linux_timerfd_gettime */
	"linux_signalfd4",			/* 327 = linux_signalfd4 */
	"linux_eventfd2",			/* 328 = linux_eventfd2 */
	"linux_epoll_create1",			/* 329 = linux_epoll_create1 */
	"linux_dup3",			/* 330 = linux_dup3 */
	"linux_pipe2",			/* 331 = linux_pipe2 */
	"linux_inotify_init1",			/* 332 = linux_inotify_init1 */
	"linux_preadv",			/* 333 = linux_preadv */
	"linux_pwritev",			/* 334 = linux_pwritev */
	"linux_rt_tsigqueueinfo",			/* 335 = linux_rt_tsigqueueinfo */
	"linux_perf_event_open",			/* 336 = linux_perf_event_open */
	"linux_recvmmsg",			/* 337 = linux_recvmmsg */
	"linux_fanotify_init",			/* 338 = linux_fanotify_init */
	"linux_fanotify_mark",			/* 339 = linux_fanotify_mark */
	"linux_prlimit64",			/* 340 = linux_prlimit64 */
	"linux_name_to_handle_at",			/* 341 = linux_name_to_handle_at */
	"linux_open_by_handle_at",			/* 342 = linux_open_by_handle_at */
	"linux_clock_adjtime",			/* 343 = linux_clock_adjtime */
	"linux_syncfs",			/* 344 = linux_syncfs */
	"linux_sendmmsg",			/* 345 = linux_sendmmsg */
	"linux_setns",			/* 346 = linux_setns */
	"linux_process_vm_readv",			/* 347 = linux_process_vm_readv */
	"linux_process_vm_writev",			/* 348 = linux_process_vm_writev */
};
