
#pragma once
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
        char newCurrentByte = 0; // needed for sub opcodes parsing
        int currentOff = 0;
        while (currentOff < hookLen) {
            signed char currentByte = 0;
            currentByte = ((uint8_t *) fun)[currentOff] & 0xff;
            log("Found current byte %hx at <%p>", currentByte, currentOff);
            if (currentByte >= 0x50 && currentByte <= 0x59) { // default registers
                currentOff++;
                continue;
            }
            switch (currentByte) {
                case '\xe8':
                    currentOff += 5;
                    return -1; // function calls not supported yet :(
                    break;
                case '\x83':
                    currentOff += 3;
                    break;
                case '\x89':
                    currentOff += 1;
                    newCurrentByte = ((char *) fun)[currentOff];
                    switch (newCurrentByte) {
                        case 0x0c:
                        case 0x4e:
                        case 0x46:
                        case 0x4f:
                        case 0x77:
                            currentOff += 2;
                            continue;
                        case 0x84:
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff];
                            if (newCurrentByte == 0x24) {
                                currentOff += 5;
                                continue;
                            }
                            log("Unsupported opcode: 0x89 0x84 %hx", newCurrentByte);
                            return -1;
                    }
                    currentOff += 1;
                    if (newCurrentByte == 0x24) {
                        currentOff += 2;
                        continue;
                    } else {
                        continue;
                    }
                    break;
                default:
                    log("Unsupported opcode: %hx", currentByte);
                    return -1;
            }
        }
        return currentOff;
    }
}