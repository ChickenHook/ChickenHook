package org.chickenhook.demoapp

import android.os.Binder
import android.os.IBinder
import android.os.Parcel
import android.os.RemoteException
import android.util.Log
import java.lang.Exception

enum class CODE {
    STRING_CMD
}

class MyBinder : Binder() {
    override fun onTransact(code: Int, data: Parcel, reply: Parcel?, flags: Int): Boolean {
        Log.d("MyBinder", "Received onTransact <${CODE.values()[code]}, ${data.dataSize()}>", Exception())
        // research: clearGatheredViolations
        /*
        syscall 0x0000007f6cec281c
art::ConditionVariable::WaitHoldingLocks(art::Thread*) 0x0000007c6308ccd0
openjdkjvmti::ThreadUtil::SuspendCheck(art::Thread*) 0x0000007c46d218cc
bool openjdkjvmti::JvmtiMonitor::Wait<openjdkjvmti::JvmtiMonitor::Wait(art::Thread*)::'lambda'(std::__1::unique_lock<std::__1::mutex>&)>(art::Thread*, openjdkjvmti::JvmtiMonitor::Wait(art::Thread*)::'lambda'(std::__1::unique_lock<std::__1::mutex>&)) 0x0000007c46ce91b4
openjdkjvmti::MonitorUtil::RawMonitorWait(_jvmtiEnv*, _jrawMonitorID*, long) 0x0000007c46ce859c
debugMonitorWait 0x0000007c4ba73964
enqueueCommand 0x0000007c4ba63cbc
event_callback 0x0000007c4ba62924
cbBreakpoint 0x0000007c4ba5f134
openjdkjvmti::JvmtiMethodTraceListener::DexPcMoved(art::Thread*, art::Handle<art::mirror::Object>, art::ArtMethod*, unsigned int) 0x0000007c46c91f30
art::instrumentation::Instrumentation::DexPcMovedEventImpl(art::Thread*, art::ObjPtr<art::mirror::Object>, art::ArtMethod*, unsigned int) const 0x0000007c631d39d0
art::interpreter::InstructionHandler<false, false, (art::Instruction::Format)26>::DoDexPcMoveEvent(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame const&, unsigned int, art::instrumentation::Instrumentation const*, art::JValue*) 0x0000007c63036d90
void art::interpreter::ExecuteSwitchImplCpp<false, false>(art::interpreter::SwitchImplContext*) 0x0000007c6302e150
ExecuteSwitchImplAsm 0x0000007c63024bdc
art::interpreter::ExecuteSwitch(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame&, art::JValue, bool) 0x0000007c631e16c4
art::interpreter::Execute(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame&, art::JValue, bool, bool) 0x0000007c631d988c
art::interpreter::ArtInterpreterToInterpreterBridge(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame*, art::JValue*) 0x0000007c631e1258
bool art::interpreter::DoCall<false, false>(art::ArtMethod*, art::Thread*, art::ShadowFrame&, art::Instruction const*, unsigned short, art::JValue*) 0x0000007c631e1ccc
void art::interpreter::ExecuteSwitchImplCpp<false, false>(art::interpreter::SwitchImplContext*) 0x0000007c6302d9e0
ExecuteSwitchImplAsm 0x0000007c63024bdc
art::interpreter::ExecuteSwitch(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame&, art::JValue, bool) 0x0000007c631e16c4
art::interpreter::Execute(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame&, art::JValue, bool, bool) 0x0000007c631d988c
art::interpreter::ArtInterpreterToInterpreterBridge(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame*, art::JValue*) 0x0000007c631e1258
bool art::interpreter::DoCall<true, false>(art::ArtMethod*, art::Thread*, art::ShadowFrame&, art::Instruction const*, unsigned short, art::JValue*) 0x0000007c631e2eb4
void art::interpreter::ExecuteSwitchImplCpp<false, false>(art::interpreter::SwitchImplContext*) 0x0000007c6302ca9c
ExecuteSwitchImplAsm 0x0000007c63024bdc
art::interpreter::ExecuteSwitch(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame&, art::JValue, bool) 0x0000007c631e16c4
art::interpreter::Execute(art::Thread*, art::CodeItemDataAccessor const&, art::ShadowFrame&, art::JValue, bool, bool) 0x0000007c631d988c
artQuickToInterpreterBridge 0x0000007c635464b8
art_quick_to_interpreter_bridge 0x0000007c6302237c
art_quick_invoke_stub 0x0000007c63018968
art::ArtMethod::Invoke(art::Thread*, unsigned int*, unsigned int, art::JValue*, char const*) 0x0000007c630851f4
art::JValue art::InvokeVirtualOrInterfaceWithVarArgs<art::ArtMethod*>(art::ScopedObjectAccessAlreadyRunnable const&, _jobject*, art::ArtMethod*, std::__va_list) 0x0000007c63428d98
art::JValue art::InvokeVirtualOrInterfaceWithVarArgs<_jmethodID*>(art::ScopedObjectAccessAlreadyRunnable const&, _jobject*, _jmethodID*, std::__va_list) 0x0000007c63428f38
art::JNI<true>::CallBooleanMethodV(_JNIEnv*, _jobject*, _jmethodID*, std::__va_list) 0x0000007c632d3074
art::(anonymous namespace)::CheckJNI::CallMethodV(char const*, _JNIEnv*, _jobject*, _jclass*, _jmethodID*, std::__va_list, art::Primitive::Type, art::InvokeType) 0x0000007c63248c0c
art::(anonymous namespace)::CheckJNI::CallBooleanMethodV(_JNIEnv*, _jobject*, _jmethodID*, std::__va_list) (.llvm.779442404939547968) 0x0000007c632367cc
_JNIEnv::CallBooleanMethod(_jobject*, _jmethodID*, ...) 0x0000007f58bb22d4
JavaBBinder::onTransact(unsigned int, android::Parcel const&, android::Parcel*, unsigned int) 0x0000007f58c63548
android::BBinder::transact(unsigned int, android::Parcel const&, android::Parcel*, unsigned int) 0x0000007f6eadc844
android::IPCThreadState::executeCommand(int) 0x0000007f6eae6320
android::IPCThreadState::getAndExecuteCommand() 0x0000007f6eae5e54
android::IPCThreadState::joinThreadPool(bool) 0x0000007f6eae66d0
android::PoolThread::threadLoop() 0x0000007f6eb0ceb8
android::Thread::_threadLoop(void*) 0x0000007c444e4240
android::AndroidRuntime::javaThreadShell(void*) 0x0000007f58bbb048
thread_data_t::trampoline(thread_data_t const*) 0x0000007c444e3ad0
__pthread_start(void*) 0x0000007f6cf29adc
__start_thread 0x0000007f6cec7c0c
         */
        return super.onTransact(code, data, reply, flags)
    }
}

class MyBinderProxy(val binder: IBinder) {

    init {
        Log.d("MyBinder", "Init proxy <$binder>")
    }
    fun sendString(str:String) {
        val data = Parcel.obtain()
//        data.writeString(str)
        //data.writeInt(-1)
        data.writeStrongBinder(binder)
        val res = Parcel.obtain()
        doTransact(CODE.STRING_CMD.ordinal, data, res, 0)
        // handle res
    }

    fun doTransact(code: Int, data: Parcel, reply: Parcel?, flags: Int): Boolean {
        Log.d("MyBinder", "Trigger doTransact <${CODE.values()[code]}, ${data.dataSize()}>")
        return binder.transact(code, data, reply, flags)
    }
}