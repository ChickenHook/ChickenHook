//
// Created by sascharoth on 24.05.22.
//

#define WRITE_BUFFER 512
#define READ_BUFFER 512

#include "TransactionCreator.h"

typedef int32_t status_t;


template<class T>
uint8_t *writeAligned(uint8_t *dst, T val) {

    memcpy(dst, &val, sizeof(val));

    return dst + sizeof(val);
}


uint8_t *writeInt32(uint8_t *dst, int32_t val) {
    return writeAligned(dst, val);
}


uint8_t *writeBinderTransaction(uint8_t *dst, binder_transaction_data &btd) {
    return writeAligned(dst, btd);
}

int
TransactionCreator::BuildBwr(uint64_t targetPtr, int code, uint8_t *data, uint32_t data_size, binder_size_t *offsets, uint32_t offsets_size,
                             binder_write_read &bwr) {
    uint8_t *ReadBuffer = (uint8_t *) malloc(READ_BUFFER);
    uint8_t *WriteBuffer = (uint8_t *) malloc(WRITE_BUFFER);
    bwr.write_buffer = (uint64_t)WriteBuffer;
    bwr.read_buffer = (uint64_t)WriteBuffer;
    bwr.read_consumed = 0;
    bwr.write_consumed = 0;

    uint8_t *WriteBufferPos = WriteBuffer;

    WriteBufferPos = writeInt32(WriteBufferPos, BC_TRANSACTION);
    binder_transaction_data btd{};
    btd.data.ptr.buffer = (uint64_t) data;
    btd.data.ptr.offsets = (uint64_t) offsets;
    btd.data_size = data_size;
    btd.offsets_size = offsets_size;
    btd.flags = 17;
    btd.code = code;
    btd.cookie = 0;
    btd.target.ptr = targetPtr;
    btd.target.handle = targetPtr;
    btd.sender_euid = 0;
    btd.sender_pid = 0;
    WriteBufferPos = writeBinderTransaction(WriteBufferPos, btd);

    bwr.read_size = READ_BUFFER;
    bwr.write_size = WriteBufferPos - WriteBuffer;
    __android_log_print(ANDROID_LOG_DEBUG, "write_size", "write_size <%llx, %p>", bwr.write_size, (void *)btd.target.ptr);
    return 0;
}
