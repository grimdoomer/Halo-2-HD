
#include "Config.h"


// Video options:
ConfigFileValue Cfg_Enable1080iSupport = { 0x2c4a6bc1, CfgValType_Bool, (unsigned int)false };
ConfigFileValue Cfg_Enable720pSupport = { 0xf6e9fe44, CfgValType_Bool, (unsigned int)true };

// Rendering options:
ConfigFileValue Cfg_DisableAnamorphicScaling = { 0x6539f288, CfgValType_Bool, (unsigned int)true };
ConfigFileValue Cfg_DisableAtmosphericFog = { 0xb12e139d, CfgValType_Bool, (unsigned int)false };

// Gameplay options:
ConfigFileValue Cfg_FieldOfView = { 0x0136b5eb, CfgValType_Float, 70.0f };
ConfigFileValue Cfg_SplitScreenFavor = { 0x2ded9bf1, CfgValType_Int, 2 };
ConfigFileValue Cfg_DisableHud = { 0xf81deed7, CfgValType_Bool, (unsigned int)false };

// Misc options:
ConfigFileValue Cfg_DebugMode = { 0x8714b510, CfgValType_Bool, (unsigned int)false };

// Hdd options:
ConfigFileValue Cfg_SetHddSpeed = { 0x0e7b8d29, CfgValType_Bool, (unsigned int)false };
ConfigFileValue Cfg_HddSpeed = { 0x7b030564, CfgValType_Int, 3 };

// Overclocking options:
ConfigFileValue Cfg_OverclockGPU = { 0xcb24708a, CfgValType_Bool, (unsigned int)false };
ConfigFileValue Cfg_GPUOverclockStep = { 0xa7450bb4, CfgValType_Int, 28 };
ConfigFileValue Cfg_OverrideFanSpeed = { 0xe8d9c3fb, CfgValType_Bool, (unsigned int)false };
ConfigFileValue Cfg_FanSpeedPercent = { 0xc9bd6bcf, CfgValType_Int, 100 };

ConfigFileValue* Cfg_ConfigFileOptionTable[] =
{
    // Video options:
    &Cfg_Enable1080iSupport,
    &Cfg_Enable720pSupport,

    // Rendering options:
    &Cfg_DisableAnamorphicScaling,
    &Cfg_DisableAtmosphericFog,

    // Gameplay options:
    &Cfg_FieldOfView,
    &Cfg_SplitScreenFavor,
    &Cfg_DisableHud,

    // Misc options:
    &Cfg_DebugMode,

    // Hdd options:
    &Cfg_SetHddSpeed,
    &Cfg_HddSpeed,

    // Overclocking options:
    &Cfg_OverclockGPU,
    &Cfg_GPUOverclockStep,
    &Cfg_OverrideFanSpeed,
    &Cfg_FanSpeedPercent,
};

const int Cfg_ConfigFileOptionTableCount = sizeof(Cfg_ConfigFileOptionTable) / sizeof(ConfigFileValue*);
