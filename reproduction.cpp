#include <iostream>
#include <string>
#include <thread>
#include <wasmedge/wasmedge.h>

#define MultiThreadInvocation

WasmEdge_Result mul(void* Data, const WasmEdge_CallingFrameContext* CallFrameCxt, const WasmEdge_Value* In, WasmEdge_Value* Out)
{
    int32_t Val = WasmEdge_ValueGetI32(In[0]);
    Out[0] = WasmEdge_ValueGenI32(Val * Val);
    return WasmEdge_Result_Success;
}

static void invocation(WasmEdge_VMContext* VMCxt)
{
    WasmEdge_String FuncName = WasmEdge_StringCreateByCString("func");
    WasmEdge_Value Params[1], Returns[1];
    Params[0] = WasmEdge_ValueGenI32(10);
    WasmEdge_Result Res = WasmEdge_VMRunWasmFromFile(VMCxt, "func.so", FuncName, Params, 1, Returns, 1);
    if (WasmEdge_ResultOK(Res)) {
        std::cout << "Get result: " << WasmEdge_ValueGetI32(Returns[0]) << std::endl;
    } else {
        printf("Error message: %s\n", WasmEdge_ResultGetMessage(Res));
        exit(-1);
    }
    WasmEdge_StringDelete(FuncName);
}

int main()
{
    /* Create the configure context and add the WASI support. */
    WasmEdge_ConfigureContext* ConfCxt = WasmEdge_ConfigureCreate();
    WasmEdge_ConfigureAddProposal(ConfCxt, WasmEdge_Proposal_Threads);
    WasmEdge_VMContext* VMCxt = WasmEdge_VMCreate(ConfCxt, NULL);
    WasmEdge_ModuleInstanceContext* server = NULL;

    /* Register the host module */
    {
        WasmEdge_String HostName = WasmEdge_StringCreateByCString("server");
        server = WasmEdge_ModuleInstanceCreate(HostName);
        WasmEdge_StringDelete(HostName);

        WasmEdge_Limit MemLimit = { .HasMax = true, .Shared = false, .Min = 1, .Max = 2 };
        WasmEdge_MemoryTypeContext* HostMType = WasmEdge_MemoryTypeCreate(MemLimit);
        WasmEdge_MemoryInstanceContext* HostMemory = WasmEdge_MemoryInstanceCreate(HostMType);
        WasmEdge_MemoryTypeDelete(HostMType);
        WasmEdge_String MemoryName = WasmEdge_StringCreateByCString("memory");
        WasmEdge_ModuleInstanceAddMemory(server, MemoryName, HostMemory);
        WasmEdge_StringDelete(MemoryName);

        enum WasmEdge_ValType ParamList[1] = { WasmEdge_ValType_I32 };
        enum WasmEdge_ValType ReturnList[1] = { WasmEdge_ValType_I32 };
        WasmEdge_FunctionTypeContext* HostFType = WasmEdge_FunctionTypeCreate(ParamList, 1, ReturnList, 1);
        WasmEdge_FunctionInstanceContext* HostFunc = WasmEdge_FunctionInstanceCreate(HostFType, mul, NULL, 0);
        WasmEdge_FunctionTypeDelete(HostFType);
        WasmEdge_String FuncName = WasmEdge_StringCreateByCString("mul");
        WasmEdge_ModuleInstanceAddFunction(server, FuncName, HostFunc);
        WasmEdge_StringDelete(FuncName);

        WasmEdge_Result Res = WasmEdge_VMRegisterModuleFromImport(VMCxt, server);
        if (!WasmEdge_ResultOK(Res)) {
            printf("Server module registration failed: %s\n", WasmEdge_ResultGetMessage(Res));
            exit(-1);
        }
    }

#ifdef MultiThreadInvocation
    /* multi-thread invocation */
    std::thread(invocation, VMCxt).join();
#else
    /* invocation in series */
    invocation(VMCxt);
#endif

    /* Resources deallocations. */
    WasmEdge_ModuleInstanceDelete(server);
    WasmEdge_VMDelete(VMCxt);
    WasmEdge_ConfigureDelete(ConfCxt);
    return 0;
}
