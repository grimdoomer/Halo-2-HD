
#include "Halo2HD.h"
#include "Config/Config.h"
#include <Trainer.h>

#pragma comment(linker, "/trainer_flag:enable_extra_ram_xbe_header")

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

ENUM_OPTION_INFO SplitScreenEnumOptions[] =
{
    {
        L"Horizontal Split",
        L"Screen is split horizontally.",
        1
    },
    {
        L"Vertical Split",
        L"Screen is split vertically.",
        2
    },
    ENUM_LIST_END()
};

TRAINER_OPTION_INFO TrainerOptions[] =
{
    {
        L"Enable 1080i Support",
        L"Enables 1080i resolution support, the console video settings must have 1080i enable for this to take effect.",
        _type_bool,
        &Cfg_Enable1080iSupport.ValueAsUInt
    },
    {
        L"Enable 720p Support",
        L"Enables 720p resolution support, the console video settings must have 720p enable for this to take effect.",
        _type_bool,
        &Cfg_Enable720pSupport.ValueAsUInt
    },
    {
        L"Disable Anamorphic Scaling",
        L"Disables anamorphic scaling, if you're unsure what this does leave it enabled. Should only be used in 480p mode with your TV set to \"stretch\" mode.",
        _type_bool,
        &Cfg_DisableAnamorphicScaling.ValueAsUInt
    },
    {
        L"Field of View",
        L"Field of view adjustment, default is 70 degrees.",
        _type_float,
        &Cfg_FieldOfView.ValueAsFloat
    },
    {
        L"Split Screen Mode",
        L"Determines how the screen will be divided when playing with 2 players, only used when widescreen is enabled.",
        _type_enum,
        &Cfg_SplitScreenFavor.ValueAsUInt,
        SplitScreenEnumOptions
    },
    {
        L"Disable HUD",
        L"Disables the HUD.",
        _type_bool,
        &Cfg_DisableHud.ValueAsUInt
    },
    {
        L"Display Performance Counters",
        L"Displays performance counters on the screen including FPS and GPU clock speed.",
        _type_bool,
        &Cfg_DebugMode.ValueAsUInt
    },
    {
        L"Overclock GPU",
        L"WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n"
        L"\n"
        L"Before enabling this option please read the Halo 2 HD wiki page on overclocking the GPU and the risks it carries.",
        _type_bool,
        &Cfg_OverclockGPU.ValueAsUInt
    },
    {
        .Name = L"GPU Clock Step",
        .Description = L"Controls the step value for how the GPU clock frequency is calculated. Please read the Halo 2 HD wiki page on overclocking the GPU to understand how to set this value.",
        .Type = (unsigned int)_type_integer | _flag_use_min_max,
        .Value = &Cfg_GPUOverclockStep.ValueAsUInt,
        .MinMaxInteger = { 28, 0xFFFFFFFF }
    },
    {
        L"Fan Speed Override",
        L"Used to set a custom fan speed while playing the game. Only takes effect when GPU overclocking is enabled.",
        _type_bool,
        &Cfg_OverrideFanSpeed.ValueAsUInt
    },
    {
        .Name = L"Fan Speed Value",
        .Description = L"Fan speed used when fan speed override is enabled. Value is a percentage between [10, 100].",
        .Type = (unsigned int)_type_integer | _flag_use_min_max,
        .Value = &Cfg_FanSpeedPercent.ValueAsUInt,
        .MinMaxInteger = { 10, 100 }
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