
#include "Halo2HD.h"
#include "Config/Config.h"
#include <Trainer.h>

#pragma comment(linker, "/trainer_flag:enable_extra_ram")

#pragma section(".trainer", nopage)
#pragma const_seg(push, ".trainer")
#pragma data_seg(push, ".trainer")

TRAINER_GAME_INFO SupportedGameList[] =
{
#ifdef H2_1_0
    {
        0x4D530064,
        0xFFFFFFFF,
        0x00000003
    }
#elif H2_1_5
    {
        0x4D530064,
        0xFFFFFFFF,
        0x00000803
    }
#endif
};

TRAINER_OPTION_INFO TrainerOptions[] =
{
    {
        L"Enable 1080i Support",
        L"Enables 1080i resolution support, the console video settings must have 1080i enable for this to take effect.",
        (bool*)&Cfg_Enable1080iSupport.ValueAsUInt
    }
};

extern "C" TRAINER_INFO TrainerInfo =
{
    L"Halo 2 HD",
    L"Grimdoomer",
    L"v1.2",

    _countof(SupportedGameList),
    SupportedGameList,

    _countof(TrainerOptions),
    TrainerOptions
};

#pragma data_seg(pop)
#pragma const_seg(pop)