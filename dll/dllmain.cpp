#include <qqdef.h>

std::shared_ptr<spdlog::sinks::msvc_sink_mt> sink(nullptr);
std::shared_ptr<spdlog::logger> logger(nullptr);

//      如果groupUin = 0 则为私聊
void __cdecl MyCheckVideoMsg(int a, unsigned long senderUin, unsigned long unknown, unsigned long groupUin, unsigned long * msg)
{
    spdlog::info("MyCheckVideoMsg begin");
    if(CheckPtrVaild())
    {
        wchar_t * nickname = NULL;
        wchar_t * text = NULL;
    //        获取消息时间
    //    __int64 timestamp = GetMsgTime(msg);
        GetNickname(&nickname, senderUin);
        GetMsgAbstract(&text, msg);

        spdlog::info("senderUin: {0:x}, groupUin: {1:x}, unknown: {2:x}", senderUin, groupUin, unknown);
    }

    CheckVideoMsg(a, senderUin, unknown, groupUin, msg);
}


BOOL SetHook(LPVOID pTarget, LPVOID pDest, LPVOID pOld)
{
    MH_STATUS ret = MH_OK;

    spdlog::info("SetHook begin, pTarget: {}", pTarget);
    ret = MH_CreateHook(pTarget, pDest, reinterpret_cast<LPVOID*>(pOld));
    if (ret != MH_OK)
    {
        spdlog::error("MH_CreateHook failed, MH_STATUS: {}", MH_StatusToString(ret));
        return FALSE;
    }

    if ((ret = MH_EnableHook((LPVOID)pTarget)) != MH_OK)
    {
        spdlog::error("MH_CreateHook failed, MH_STATUS: {}", MH_StatusToString(ret));
        return FALSE;
    }

    return TRUE;
}

BOOL UnHook(LPVOID pTarget)
{
    if (MH_DisableHook(pTarget) != MH_OK)
        return FALSE;

    if(MH_Uninitialize() != MH_OK)
        return FALSE;

    return TRUE;
}

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID lpReserved)
{
    switch (reason)
    {
    case DLL_PROCESS_ATTACH:
    {
        // 设置日志默认输出
        sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
        logger = std::make_shared<spdlog::logger>("MsgIPC", sink);
        spdlog::set_default_logger(logger);

        // 初始化minhook并hook
        MH_Initialize();
        InitQQPtr();

        if(SetHook((LPVOID)MsgHookTarget, &MyCheckVideoMsg, &CheckVideoMsg))
            spdlog::info("SetHook OK!");
    }
        break;
    case DLL_PROCESS_DETACH:
    {
        if(UnHook((LPVOID)MsgHookTarget))
            spdlog::info("UnHook OK!");

        MH_Uninitialize();
    }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}