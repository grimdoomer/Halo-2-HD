
#pragma once
#include "Xbox/Xbox.h"

enum ConfigValueType : int
{
    CfgValType_Int = 0,         // 32-bit signed integer
    CfgValType_Float,           // 32-bit signed float
    CfgValType_String,          // Value is a pointer to a null terminated ascii string
    CfgValType_Bool,            // Value is a 32-bit integer with value 0 or 1, config file value must be either "false" or "true"
};

struct ConfigFileValue
{
    ULONG NameCrc;
    ConfigValueType Type;

    union
    {
        ULONG ValueAsUInt;
        float ValueAsFloat;
        void* ValueAsPtr;
    };

    template<typename T> T GetValue()
    {
        return *(T*)&this->ValueAsUInt;
    }

    template<typename T> void SetValue(T value)
    {
        this->ValueAsUInt = *(unsigned int*)&value;
    }
};

// Video options:
extern ConfigFileValue Cfg_Enable1080iSupport;
extern ConfigFileValue Cfg_Enable720pSupport;

// Rendering options:
extern ConfigFileValue Cfg_DisableAnamorphicScaling;
extern ConfigFileValue Cfg_DisableAtmosphericFog;

// Gameplay options:
extern ConfigFileValue Cfg_FieldOfView;
extern ConfigFileValue Cfg_SplitScreenFavor;
extern ConfigFileValue Cfg_DisableHud;

// Misc options:
extern ConfigFileValue Cfg_DebugMode;

// Hdd options:
extern ConfigFileValue Cfg_SetHddSpeed;
extern ConfigFileValue Cfg_HddSpeed;

// Overclocking options:
extern ConfigFileValue Cfg_OverclockGPU;
extern ConfigFileValue Cfg_GPUOverclockStep;
extern ConfigFileValue Cfg_OverrideFanSpeed;
extern ConfigFileValue Cfg_FanSpeedPercent;

void Cfg_ParseConfigFile();