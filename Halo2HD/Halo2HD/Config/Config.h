
#pragma once
#include <Utilities/ConfigFile.h>

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

extern ConfigFileValue* Cfg_ConfigFileOptionTable[];
extern const int Cfg_ConfigFileOptionTableCount;