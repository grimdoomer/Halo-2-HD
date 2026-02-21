
#include "Config.h"
#include "Utilities/Utilities.h"
#include "Blam/memory/crc.h"

// Forward declarations:
bool Cfg_ParseConfigLine(char* pNameString, char* pValueString);
bool Cfg_TokenizeNextLine(char** ppBuffer, char** ppNameString, char** ppValueString);
char* Cfg_SkipWhitespace(char* psBuffer);
bool Cfg_IsAlphaNumeric(char character);
char* Cfg_StrFind(char* pString, char find);

double _atof(const char* s);


// Video options:
ConfigFileValue Cfg_Enable1080iSupport = { 0x4b51a48c, CfgValType_Bool, (ULONG)false };
ConfigFileValue Cfg_Enable720pSupport = { 0x3f060ff9, CfgValType_Bool, (ULONG)true };

// Rendering options:
ConfigFileValue Cfg_DisableAnamorphicScaling = { 0xc6f838a8, CfgValType_Bool, (ULONG)true };
ConfigFileValue Cfg_DisableAtmosphericFog = { 0xf02ad616, CfgValType_Bool, (ULONG)false };

// Gameplay options:
ConfigFileValue Cfg_FieldOfView = { 0x6ab10407, CfgValType_Float, 70.0f };
ConfigFileValue Cfg_SplitScreenFavor = { 0xc156d0a4, CfgValType_Int, 2 };
ConfigFileValue Cfg_DisableHud = { 0x1b9786a1, CfgValType_Bool, (ULONG)false };

// Misc options:
ConfigFileValue Cfg_DebugMode = { 0x611da1be, CfgValType_Bool, (ULONG)false };

// Hdd options:
ConfigFileValue Cfg_SetHddSpeed = { 0x65fc3cc5, CfgValType_Bool, (ULONG)false };
ConfigFileValue Cfg_HddSpeed = { 0x1e21da0d, CfgValType_Int, 3 };

// Overclocking options:
ConfigFileValue Cfg_OverclockGPU = { 0xb0f1b6e5, CfgValType_Bool, (ULONG)false };
ConfigFileValue Cfg_GPUOverclockStep = { 0xdc90cddb, CfgValType_Int, 28 };
ConfigFileValue Cfg_OverrideFanSpeed = { 0x40f0721b, CfgValType_Bool, (ULONG)false };
ConfigFileValue Cfg_FanSpeedPercent = { 0xc6c92d4d, CfgValType_Int, 100 };

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


void Cfg_ParseConfigFile()
{
    ULONG BytesRead = 0;
    char* pBufferPtr;
    char* psNameString;
    char* psValueString;

    // Open the config file for reading.
    HANDLE hConfigFile = CreateFileA("D:\\hd_config.ini", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hConfigFile == INVALID_HANDLE_VALUE)
        return;

    // Get the size of the config file.
    ULONG configFileSize = GetFileSize(hConfigFile, nullptr);

    // Allocate a buffer to hold the config file data.
    char* pCfgFileBuffer = (char*)malloc(configFileSize + 1);
    if (pCfgFileBuffer == nullptr)
        goto Cleanup;

    // Null terminate the buffer.
    pCfgFileBuffer[configFileSize] = 0;

    // Read the config file data.
    if (ReadFile(hConfigFile, pCfgFileBuffer, configFileSize, &BytesRead, nullptr) == FALSE)
        goto Cleanup;

    pBufferPtr = pCfgFileBuffer;

    do
    {
        // Tokenize the next line.
        if (Cfg_TokenizeNextLine(&pBufferPtr, &psNameString, &psValueString) == false)
            goto Cleanup;

        // Check if the line contained valid data or was empty.
        if (psValueString == nullptr)
            continue;

        // Parse the tokens.
        if (Cfg_ParseConfigLine(psNameString, psValueString) == false)
            goto Cleanup;
    }
    while (pBufferPtr != nullptr);

Cleanup:

    // Free string buffer.
    if (pCfgFileBuffer != nullptr)
        free(pCfgFileBuffer);

    // Close the config file.
    if (hConfigFile != INVALID_HANDLE_VALUE)
        CloseHandle(hConfigFile);
}

//
// Parses the tokens for a single line in the config file
//
bool Cfg_ParseConfigLine(char* pNameString, char* pValueString)
{
    int checksum = 0;

    // Calculate the checksum of the config value name.
    int length = lstrlenA(pNameString);
    crc_checksum_buffer(&checksum, (unsigned char*)pNameString, length);

#ifdef CFG_DEBUG_ENABLED
    DbgPrint("Cfg: Crc=0x%08x Name=%s Value=%s\n", checksum, pNameString, pValueString);
#endif

    // Search the config file value table for an entry with matching crc.
    for (int i = 0; i < Cfg_ConfigFileOptionTableCount; i++)
    {
        // Check if the name crc matches.
        if (Cfg_ConfigFileOptionTable[i]->NameCrc != checksum)
            continue;

        // Check the config value type and parse accordingly.
        switch (Cfg_ConfigFileOptionTable[i]->Type)
        {
        case CfgValType_Int:
        {
            Cfg_ConfigFileOptionTable[i]->ValueAsUInt = (ULONG)atol(pValueString);
            return true;
        }
        case CfgValType_Float:
        {
            Cfg_ConfigFileOptionTable[i]->ValueAsFloat = (float)_atof(pValueString);
            return true;
        }
        case CfgValType_String:
        {
            // Create a new buffer and copy the string contents to it.
            int valueLength = lstrlenA(pValueString);
            char* pValueCopy = (char*)malloc(valueLength);
            if (pValueCopy == nullptr)
                return false;

            lstrcpyA(pValueCopy, pValueString);
            Cfg_ConfigFileOptionTable[i]->ValueAsPtr = pValueCopy;
            return true;
        }
        case CfgValType_Bool:
        {
            // Check the value for true/false.
            if (*(ULONG*)pValueString == 'eurt')
                Cfg_ConfigFileOptionTable[i]->ValueAsUInt = 1;
            else if (*(ULONG*)pValueString == 'slaf')
                Cfg_ConfigFileOptionTable[i]->ValueAsUInt = 0;
            else
                return false;

            return true;
        }
        }
    }

    // Config option with matching crc not found, return success as we don't want to fail out in case this
    // is an old version of the patch running against a new config file or something.
    return true;
}

//
// Tokenizes the next line of the config file. Returns true if successful and false if a parsing error occured.
//
bool Cfg_TokenizeNextLine(char** ppBuffer, char** ppNameString, char** ppValueString)
{
    // Initialize out parameters.
    *ppNameString = nullptr;
    *ppValueString = nullptr;

    // Skip any leading whitespace.
    char* pPtr = *ppBuffer;
    if ((pPtr = Cfg_SkipWhitespace(pPtr)) == nullptr)
        return false;

    // Check if this is a comment line.
    if (*pPtr == ';' || *pPtr == '\r' || *pPtr == '\n')
    {
        // Skip to the end of the line.
        if ((pPtr = Cfg_StrFind(pPtr, '\n')) == nullptr)
            return false;

        // Save the next line pointer.
        *ppBuffer = ++pPtr;
        return true;
    }

    // Save the name pointer.
    *ppNameString = pPtr;

    // Loop until we reach the next non-alpha numeric character.
    while (Cfg_IsAlphaNumeric(*pPtr) == true)
        pPtr++;

    // Find the string split character.
    char* pTempPtr = pPtr;
    if ((pPtr = Cfg_StrFind(pPtr, '=')) == nullptr)
        return false;

    // Null terminate the name string.
    *pTempPtr = 0;
    pPtr++;

    // Skip any whitespace characters.
    if ((pPtr = Cfg_SkipWhitespace(pPtr)) == nullptr)
        return false;

    // Save the value pointer.
    *ppValueString = pPtr;

    // At this point we consider the string valid, if the newline character isn't found it could mean
    // this is the last line in the config file, in which case we don't want to fail out.

    // Find the end of the line.
    *ppBuffer = Cfg_StrFind(pPtr, '\n');
    if (*ppBuffer != 0)
    {
        // Null terminate the value string.
        *(*ppBuffer) = 0;
        (*ppBuffer)++;
    }

    return true;
}

//
// Skips leading whitespace characters and returns the address of the first valid character or NULL if null terminator was reached
//
char* Cfg_SkipWhitespace(char* psBuffer)
{
    while (*psBuffer != 0)
    {
        // If the next character isn't whitespace break out.
        if (*psBuffer != ' ' && *psBuffer != '\t')
            break;

        psBuffer++;
    }

    return *psBuffer != 0 ? psBuffer : nullptr;
}

//
// Returns true if the character is alpha numeric, false otherwise
//
bool Cfg_IsAlphaNumeric(char character)
{
    // Check if the character is numeric.
    if (character >= '0' && character <= '9')
        return true;

    // Check for upper/lowercase.
    if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z'))
        return true;

    return false;
}

char* Cfg_StrFind(char* pString, char find)
{
    // Loop and search for the specified character.
    while (*pString != find)
    {
        // Check if this is the character we're looking for.
        if (*pString == 0 || *pString == '\n')
            return nullptr;

        pString++;
    }

    // Return a pointer to the found character.
    return pString;
}

int __cdecl isdigit(int c)
{
    return (unsigned)c - '0' < 10;
}

// See: https://github.com/GaloisInc/minlibc/blob/master/atof.c
double _atof(const char* s)
{
    // This function stolen from either Rolf Neugebauer or Andrew Tolmach. 
    // Probably Rolf.
    double a = 0.0;
    int e = 0;
    int c;
    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a * 10.0 + (c - '0');
    }
    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a * 10.0 + (c - '0');
            e = e - 1;
        }
    }
    if (c == 'e' || c == 'E') {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
            c = *s++;
        else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (isdigit(c)) {
            i = i * 10 + (c - '0');
            c = *s++;
        }
        e += i * sign;
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    return a;
}