
#pragma once

#include <beaengine/BeaEngine.h>

namespace ChickenHook {

/**
 *
 * @return -1 on failure, otherwise next possible instruction offset
 */
    static int nextInstOff(void *fun, size_t hookLen) {
        if (fun == nullptr) {
            // TODO error log
            return -1;
        }

        DISASM infos;
        int len, i = 0, offset = 0;

        (void) memset(&infos, 0, sizeof(DISASM));
        infos.EIP = (UIntPtr) fun;

        while ((infos.Error == 0) && (offset < hookLen)) {
            len = Disasm(&infos);
            if (infos.Instruction.BranchType != 0) {
                log("Branch found... abort");
                return -1;
            }

            if (infos.Error != UNKNOWN_OPCODE) {
                log("Instruciton %s : <%d>", infos.CompleteInstr, infos.Instruction.BranchType);
                infos.EIP += len;
                offset += len;
                i++;
            }
        }
        return offset;
    }
}