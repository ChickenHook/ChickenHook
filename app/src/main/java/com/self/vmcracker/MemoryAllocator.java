package com.self.vmcracker;

public class MemoryAllocator {

    public static Object[] getObjectArray() {
        Object[] objects = new Object[1];
        objects[0] = new Object();
        return objects;
    }
}
