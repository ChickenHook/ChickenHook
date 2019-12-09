
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
            if (currentByte >= '\x50' && currentByte <= '\x59') { // default registers
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
                case '\x40':
                    currentOff += 1;
                    newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                    switch (newCurrentByte) {
                        case '\x88':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                            switch (newCurrentByte) {
                                case '\xf0':
                                    currentOff += 1;
                                    continue;
                                    break;
                                default:
                                    log("Unsupported sub opcode 0x40 0x88 <%hx>", newCurrentByte);
                                    return -1;
                                    break;
                            }
                            break;
                        case '\x51':
                        case '\x52':
                        case '\x53':
                        case '\x54':
                        case '\x55':
                        case '\x56':
                        case '\x57':
                        case '\x58':
                        case '\x59':
                            currentOff += 1;
                            continue;
                            break;
                        default:
                            log("Unsupported sub opcode 0x40 <%hx>", newCurrentByte);
                            return -1;
                            break;
                    }
                    break;
                case '\x41':
                    currentOff += 1;
                    newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                    switch (newCurrentByte) {
                        case '\x89':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                            switch (newCurrentByte) {
                                case '\xce':
                                    currentOff += 1;
                                    continue;
                                    break;
                                default:
                                    log("Unsupported sub opcode 0x40 0x88 <%hx>", newCurrentByte);
                                    return -1;
                                    break;
                            }
                            break;
                        case '\x51':
                        case '\x52':
                        case '\x53':
                        case '\x54':
                        case '\x55':
                        case '\x56':
                        case '\x57':
                        case '\x58':
                        case '\x59':
                            currentOff += 1;
                            continue;
                            break;
                        default:
                            log("Unsupported sub opcode 0x40 <%hx>", newCurrentByte);
                            return -1;
                            break;
                    }
                case '\x48':
                    currentOff += 1;
                    newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                    switch (newCurrentByte) {
                        case '\x81':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                            switch (newCurrentByte) {
                                case '\xec':
                                    currentOff += 5;
                                    break;
                                default:
                                    log("Unsupported sub opcode 0x48 0x81 <%hx>", newCurrentByte);
                                    return -1;
                                    break;
                            }
                            break;
                        case '\x89':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                            switch (newCurrentByte) {
                                case '\xe5':
                                    currentOff += 1;
                                    continue;
                                    break;
                                case '\xc7':
                                    currentOff += 1;
                                    continue;
                                    break;
                                case '\x7d':
                                    currentOff += 2;
                                    continue;
                                    break;
                                default:
                                    log("Unsupported sub opcode 0x48 0x89 <%hx>", newCurrentByte);
                                    return -1;
                                    break;
                            }


                            break;
                        case '\x83':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                            switch (newCurrentByte) {
                                case '\xec':
                                    currentOff += 2;
                                    continue;
                                    break;
                                default:
                                    log("Unsupported sub opcode 0x48 0x83 <%hx>", newCurrentByte);
                                    return -1;
                                    break;


                            }
                            break;
                        default:
                            log("Unsupported sub opcode 0x48 <%hx>", newCurrentByte);
                            return -1;
                            break;
                    }

                    break;
                case '\x64':
                    currentOff += 1;
                    newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                    switch (newCurrentByte) {
                        case '\x48':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                            switch (newCurrentByte) {
                                case '\x8b':
                                    currentOff += 1;
                                    newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                                    switch (newCurrentByte) {
                                        case '\x04':
                                            currentOff += 1;
                                            newCurrentByte = ((char *) fun)[currentOff] & 0xff;
                                            switch (newCurrentByte) {
                                                case '\x25':
                                                    currentOff += 4;
                                                    continue;
                                                    break;
                                                default:
                                                    log("Unsupported sub opcode 0x64 0x48 0x8b 0x04  <%hx>",
                                                        newCurrentByte);
                                                    return -1;
                                                    break;

                                            }
                                            continue;
                                            break;
                                        default:
                                            log("Unsupported sub opcode 0x64 0x48 0x8b <%hx>",
                                                newCurrentByte);
                                            return -1;
                                            break;

                                    }
                                    continue;
                                    break;
                                default:
                                    log("Unsupported sub opcode 0x64 0x48 <%hx>", newCurrentByte);
                                    return -1;
                                    break;

                            }
                            continue;
                            break;
                        default:
                            log("Unsupported sub opcode 0x64 <%hx>", newCurrentByte);
                            return -1;
                            break;
                    }
                    break;
                case '\x89':
                    currentOff += 1;
                    newCurrentByte = ((char *) fun)[currentOff];
                    switch (newCurrentByte) {
                        case '\x0c':
                        case '\x4e':
                        case '\x46':
                        case '\x4f':
                        case '\x77':
                            currentOff += 2;
                            continue;
                        case '\x84':
                            currentOff += 1;
                            newCurrentByte = ((char *) fun)[currentOff];
                            if (newCurrentByte == '\x24') {
                                currentOff += 5;
                                continue;
                            }
                            log("Unsupported opcode: 0x89 0x84 %hx", newCurrentByte);
                            return -1;
                    }
                    currentOff += 1;
                    if (newCurrentByte == '\x24') {
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

        return
                currentOff;
    }
}