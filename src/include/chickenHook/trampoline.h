
#pragma once

#include <stdio.h>

#include <vector>
#include <setjmp.h>
#include <mutex>

#ifdef __aarch64__
#define CODE_SIZE 64
#elif __i386__
#define CODE_SIZE 128
#elif __x86_64__
#define CODE_SIZE 128
#else
#error "UNSUPPORTED"
#endif

/**
 * Represents a trampoline that will forward the controlflow from the function to be hooked into our code.
 *
 * This class contains all neccessary functions to work with a trampoline.
 */
class Trampoline {
public:
    /**
     * Creates a new Trampoline
     * @param addr the function the trampoline will be isntalled
     * @param callback the function that will be called instead
     */
    Trampoline(void *addr, void *callback);

    /**
     * Used for call by reference calls
     */
    Trampoline();


    /**
     * Installs the trampoline at the given address
     * @return true on success
     */
    bool install();

    bool reinstall();

    /**
     * Returns the hook function
     * @return the address of the function to be called instead of the original function
     */
    void *getHook();

    /**
     * Copies the original bytecode into the original function
     * @return
     */
    bool copyOriginal();

    /**
     * @return the address of the function where the trampoline was / will be installed
     */
    void *getOriginal();

    /**
     * Locks the trampoline.
     * Other requests to copy or reinstall the trampoline must wait till trampoline is unlocked.
     */
    void lock();

    /**
    * Locks the trampoline.
    * Other requests to copy or reinstall the trampoline must wait till trampoline is unlocked.
    */
    void unlock();

private:
    void *_original_addr;
    void *_hook_addr;
    std::vector<uint8_t> _original_code;
    std::mutex *_trampoline_lock{};

};