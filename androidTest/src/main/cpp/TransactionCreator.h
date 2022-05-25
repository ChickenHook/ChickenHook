//
// Created by sascharoth on 24.05.22.
//

#ifndef CHICKENHOOK_TRANSACTIONCREATOR_H
#define CHICKENHOOK_TRANSACTIONCREATOR_H

#include <string>
#include <android/log.h>
#include <dlfcn.h>
#include <vector>
#include "chickenHook/hooking.h"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <cstdlib>
#include <sys/ioctl.h>
#include <map>

typedef unsigned int binder_size_t;

#define BINDER_WRITE_READ -1070571007

struct binder_object_header {
    __u32 type;
};
struct flat_binder_object {
    struct binder_object_header hdr;
    __u32 flags;
    union {
        uint64_t binder;
        __u32 handle;
    };
    uint64_t cookie;
};


struct binder_transaction_data {
    union {
        __u32 handle;
        uint64_t ptr;
    } target;
    uint64_t cookie;
    __u32 code;
    __u32 flags;
    pid_t sender_pid;
    uid_t sender_euid;
    uint64_t data_size;
    uint64_t offsets_size;
    union {
        struct {
            uint64_t buffer;
            uint64_t offsets;
        } ptr;
        __u8 buf[8];
    } data;
};

struct binder_transaction_data_secctx {
    struct binder_transaction_data transaction_data;
    uint64_t secctx;
};
struct binder_transaction_data_sg {
    struct binder_transaction_data transaction_data;
    uint64_t buffers_size;
};
struct binder_ptr_cookie {
    uint64_t ptr;
    uint64_t cookie;
};
struct binder_handle_cookie {
    __u32 handle;
    uint64_t cookie;
} __packed;
struct binder_pri_desc {
    __s32 priority;
    __u32 desc;
};


struct binder_write_read {
    uint64_t write_size;
    uint64_t write_consumed;
    uint64_t write_buffer;
    uint64_t read_size;
    uint64_t read_consumed;
    uint64_t read_buffer;
};




enum binder_driver_command_protocol {
    BC_TRANSACTION = _IOW('c', 0, struct binder_transaction_data),
    BC_REPLY = _IOW('c', 1, struct binder_transaction_data),
    BC_ACQUIRE_RESULT = _IOW('c', 2, __s32),
    BC_FREE_BUFFER = _IOW('c', 3, uint64_t),
    BC_INCREFS = _IOW('c', 4, __u32),
    BC_ACQUIRE = _IOW('c', 5, __u32),
    BC_RELEASE = _IOW('c', 6, __u32),
    BC_DECREFS = _IOW('c', 7, __u32),
    BC_INCREFS_DONE = _IOW('c', 8, struct binder_ptr_cookie),
    BC_ACQUIRE_DONE = _IOW('c', 9, struct binder_ptr_cookie),
    BC_ATTEMPT_ACQUIRE = _IOW('c', 10, struct binder_pri_desc),
    BC_REGISTER_LOOPER = _IO('c', 11),
    BC_ENTER_LOOPER = _IO('c', 12),
    BC_EXIT_LOOPER = _IO('c', 13),
    BC_REQUEST_DEATH_NOTIFICATION = _IOW('c', 14, struct binder_handle_cookie),
    BC_CLEAR_DEATH_NOTIFICATION = _IOW('c', 15, struct binder_handle_cookie),
    BC_DEAD_BINDER_DONE = _IOW('c', 16, uint64_t),
    BC_TRANSACTION_SG = _IOW('c', 17, struct binder_transaction_data_sg),
    BC_REPLY_SG = _IOW('c', 18, struct binder_transaction_data_sg),
};





struct binder_pri_ptr_cookie {
    __s32 priority;
    uint64_t ptr;
    uint64_t cookie;
};


enum binder_driver_return_protocol {
    BR_ERROR = _IOR('r', 0, __s32),
    BR_OK = _IO('r', 1),
    BR_TRANSACTION_SEC_CTX = _IOR('r', 2, struct binder_transaction_data_secctx),
    BR_TRANSACTION = _IOR('r', 2, struct binder_transaction_data),
    BR_REPLY = _IOR('r', 3, struct binder_transaction_data),
    BR_ACQUIRE_RESULT = _IOR('r', 4, __s32),
    BR_DEAD_REPLY = _IO('r', 5),
    BR_TRANSACTION_COMPLETE = _IO('r', 6),
    BR_INCREFS = _IOR('r', 7, struct binder_ptr_cookie),
    BR_ACQUIRE = _IOR('r', 8, struct binder_ptr_cookie),
    BR_RELEASE = _IOR('r', 9, struct binder_ptr_cookie),
    BR_DECREFS = _IOR('r', 10, struct binder_ptr_cookie),
    BR_ATTEMPT_ACQUIRE = _IOR('r', 11, struct binder_pri_ptr_cookie),
    BR_NOOP = _IO('r', 12),
    BR_SPAWN_LOOPER = _IO('r', 13),
    BR_FINISHED = _IO('r', 14),
    BR_DEAD_BINDER = _IOR('r', 15, uint64_t),
    BR_CLEAR_DEATH_NOTIFICATION_DONE = _IOR('r', 16, uint64_t),
    BR_FAILED_REPLY = _IO('r', 17),
    BR_FROZEN_REPLY = _IO('r', 18),
    BR_ONEWAY_SPAM_SUSPECT = _IO('r', 19),
};


class TransactionCreator {
public:
    static int BuildBwr(uint64_t targetPtr, int code, uint8_t *data, uint32_t data_size, binder_size_t *offsets, uint32_t offsets_size,
                        binder_write_read &bwr);

};


#endif //CHICKENHOOK_TRANSACTIONCREATOR_H
