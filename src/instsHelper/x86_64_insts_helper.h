
#pragma once

#include <beaengine/BeaEngine.h>


namespace ChickenHook {

    static void printInstruction(char *inst) {

        DISASM infos;
        int len, i = 0, offset = 0;

        (void) memset(&infos, 0, sizeof(DISASM));

        infos.EIP = (UIntPtr) inst;
        log("printInstruction %s : <%d> : <%x> at <%p>", infos.CompleteInstr,
            infos.Instruction.BranchType, offset, infos.EIP);
    }

/**
 *
 * @return -1 on failure, otherwise next possible instruction offset
 */
    static int nextInstOff(void *fun, size_t hookLen, char *target) {
        if (fun == nullptr) {
            // TODO error log
            return -1;
        }

        DISASM infos;
        int len, i = 0, offset = 0;

        (void) memset(&infos, 0, sizeof(DISASM));

        infos.EIP = (UIntPtr) fun;
        volatile bool branchFound = false;
        while (((infos.Error == 0) && ((offset < hookLen) || branchFound))) {
            len = Disasm(&infos);


            if (infos.Error == 0) {
                log("Instruction %s : <%d> : <%x> at <%p>", infos.CompleteInstr,
                    infos.Instruction.BranchType, offset, infos.EIP);


                if (infos.Instruction.BranchType == RetType && branchFound && (offset > hookLen)) {
                    log("Branch found... ret found!");
                    return offset + len;
                }
                if (infos.Instruction.BranchType == CallType) {
                    int32_t rel_addr = 0;

                    switch (*((char *) (uint64_t) infos.EIP)) {
                        case '\xe8':
                            // calculate new addr
                            rel_addr = (char *) infos.Instruction.AddrValue - (target + offset) - 5;
                            // inject into memory
                            memcpy((char *) infos.EIP + 1, &rel_addr, 4);
                            break;
                        default:
                            log("Unsupported branch found... abort");
                            //infos.Instruction.AddrValue
                            return -1;
                    }
                } else if (infos.Instruction.BranchType !=
                           0) { // all other branches are not supported yet
                    log("Branch to addr <%d> found... seatching for ret",
                        infos.Instruction.AddrValue);

                    branchFound = true;
                }


                infos.EIP += len;
                offset += len;
                i++;
            } else {
                log("Unable to read instruction, abort. (Error %d)", infos.Error);
                return -1;
            }
            log("Iteration %d : %d : %d :% d", infos.Error, (offset < hookLen), branchFound,
                hookLen);
        }
        return offset;
    }
}