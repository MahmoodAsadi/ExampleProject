// Fill out your copyright notice in the Description page of Project Settings.

#include "DualSenseWindows.h"

#if PLATFORM_WINDOWS

#include "IndependentInputManager.h"
#include "DualSenseTriggerEffectGenerator.h"

#include <setupapi.h>
#include <hidsdi.h>
#include "Misc/Crc.h"


FDualSenseWindows::FDualSenseWindows()
	: DeviceHandle(INVALID_HANDLE_VALUE)
	, Effects{}
	, State{}
{
}

FDualSenseWindows::~FDualSenseWindows()
{
	Close();
}

bool FDualSenseWindows::ReadInputReport()
{
    if (DeviceHandle == INVALID_HANDLE_VALUE)
        return false;

    uint8 Report[64];
    DWORD BytesRead = 0;
    BOOL Result = ReadFile(DeviceHandle, Report, sizeof(Report), &BytesRead, nullptr);

    if (!Result || BytesRead != sizeof(Report))
        return false;

    // USB reports begin with ReportID (0x01)
    if (Report[0] != 0x01)
        return false;

    FMemory::Memzero(State);
    FMemory::Memcpy(&State, Report + 1, FMath::Min<int32>(sizeof(FDS5State), BytesRead - 1));
    return true;
}

bool FDualSenseWindows::Open(uint16 VendorId, uint16 ProductId, const FString& SDLSerial)
{
    Close();

    if (SDLSerial.IsEmpty())
    {
        UE_LOG(LogIndependentInput, Warning, TEXT("DualSense SDL serial is empty; cannot match HID device."));
        return false;
    }

    GUID HidGuid;
    HidD_GetHidGuid(&HidGuid);

    HDEVINFO DeviceInfoSet = SetupDiGetClassDevs(
        &HidGuid,
        nullptr,
        nullptr,
        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

    if (DeviceInfoSet == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    SP_DEVICE_INTERFACE_DATA InterfaceData{};
    InterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

    DWORD DeviceIndex = 0;

    while (SetupDiEnumDeviceInterfaces(DeviceInfoSet, nullptr, &HidGuid, DeviceIndex++, &InterfaceData))
    {
        DWORD RequiredSize = 0;

        SetupDiGetDeviceInterfaceDetail(
            DeviceInfoSet,
            &InterfaceData,
            nullptr,
            0,
            &RequiredSize,
            nullptr);

        if (RequiredSize == 0)
            continue;

        TArray<uint8> Buffer;
        Buffer.SetNumZeroed(RequiredSize);

        PSP_DEVICE_INTERFACE_DETAIL_DATA Detail =
            reinterpret_cast<PSP_DEVICE_INTERFACE_DETAIL_DATA>(Buffer.GetData());

        Detail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

        if (!SetupDiGetDeviceInterfaceDetail(
            DeviceInfoSet,
            &InterfaceData,
            Detail,
            RequiredSize,
            nullptr,
            nullptr))
        {
            continue;
        }

        HANDLE Handle = CreateFile(
            Detail->DevicePath,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if (Handle == INVALID_HANDLE_VALUE)
        {
            continue;
        }

        HIDD_ATTRIBUTES Attributes{};
        Attributes.Size = sizeof(HIDD_ATTRIBUTES);

        if (!HidD_GetAttributes(Handle, &Attributes))
        {
            CloseHandle(Handle);
            continue;
        }

        // Ignore devices that aren't the requested VID/PID.
        if (Attributes.VendorID != VendorId ||
            Attributes.ProductID != ProductId)
        {
            CloseHandle(Handle);
            continue;
        }


        PHIDP_PREPARSED_DATA PreparsedData = nullptr;
        HIDP_CAPS Caps{};

        if (!HidD_GetPreparsedData(Handle, &PreparsedData))
        {
            CloseHandle(Handle);
            continue;
        }

        if (HidP_GetCaps(PreparsedData, &Caps) != HIDP_STATUS_SUCCESS)
        {
            HidD_FreePreparsedData(PreparsedData);
            CloseHandle(Handle);
            continue;
        }

        HidD_FreePreparsedData(PreparsedData);


        uint8 Report[64] = {};
        Report[0] = 0x09; // DualSense serial feature report

        if (!HidD_GetFeature(Handle, Report, sizeof(Report)))
        {
            UE_LOG(LogIndependentInput, Verbose, TEXT("DualSense feature serial unavailable for HID candidate. GetLastError=%lu"), GetLastError());
            CloseHandle(Handle);
            continue;
        }

        const FString FeatureSerial = FString::Printf(
            TEXT("%02x-%02x-%02x-%02x-%02x-%02x"),
            Report[6], 
            Report[5], 
            Report[4],
            Report[3], 
            Report[2], 
            Report[1]);

        if (!FeatureSerial.Equals(SDLSerial, ESearchCase::IgnoreCase))
        {
            UE_LOG(LogIndependentInput, Verbose, TEXT("Skipping DualSense HID candidate because serial does not match SDL device."));
            CloseHandle(Handle);
            continue;
        }

        InputReportLength = Caps.InputReportByteLength;
        OutputReportLength = Caps.OutputReportByteLength;
        FeatureReportLength = Caps.FeatureReportByteLength;

        if (OutputReportLength >= 78)
        {
            ConnectionType = EDeviceConnectionType::Wireless;
        }
        else
        {
            ConnectionType = EDeviceConnectionType::Wired;
        }

        BluetoothSequence = 0;

        UE_LOG(LogIndependentInput, Log,
            TEXT("Opened DualSense HID device. Serial=%s Connection=%s"),
            *SDLSerial, ConnectionType == EDeviceConnectionType::Wireless ? TEXT("Bluetooth") : TEXT("USB"));


        // Print capabilities so we know what we found.
        LogDeviceCapabilities(Handle, Detail->DevicePath);

        UE_LOG(LogIndependentInput, Log, TEXT("Opened DualSense HID device matching SDL serial: %s"), *SDLSerial);

        // Keep this handle.
        DeviceHandle = Handle;

        SetupDiDestroyDeviceInfoList(DeviceInfoSet);
        ResetEffects();

        return true;
    }

    SetupDiDestroyDeviceInfoList(DeviceInfoSet);
    UE_LOG(LogIndependentInput, Warning, TEXT("No DualSense HID device matched SDL serial: %s"), *SDLSerial);

    return false;
}

void FDualSenseWindows::Close()
{
	if (DeviceHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(DeviceHandle);
		DeviceHandle = INVALID_HANDLE_VALUE;
	}
}

bool FDualSenseWindows::IsOpen() const
{
	return DeviceHandle != INVALID_HANDLE_VALUE;
}

void FDualSenseWindows::ResetEffects()
{
    Effects = FDS5EffectsState{};
}

bool FDualSenseWindows::SetAdaptiveTriggerResistance(EDualSenseTrigger Trigger, uint8 Position, uint8 Strength)
{
    uint8 Effect[11] = {};
    if (!FDualSenseTriggerEffectGenerator::Feedback(Effect, Position, Strength))
        return false;

    return ApplyTriggerEffect(Trigger, Effect);
}

bool FDualSenseWindows::SetAdaptiveTriggerWeapon(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength)
{
    uint8 Effect[11] = {};
    if (!FDualSenseTriggerEffectGenerator::Weapon(Effect, StartPosition, EndPosition, Strength))
        return false;

    return ApplyTriggerEffect(Trigger, Effect);
}

bool FDualSenseWindows::SetAdaptiveTriggerVibration(EDualSenseTrigger Trigger, uint8 Position, uint8 Amplitude, uint8 Frequency)
{
    uint8 Effect[11] = {};
    if (!FDualSenseTriggerEffectGenerator::Vibration(Effect, Position, Amplitude, Frequency))
        return false;

    return ApplyTriggerEffect(Trigger, Effect);
}

bool FDualSenseWindows::SetAdaptiveTriggerBow(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce)
{
    uint8 Effect[11] = {};
    if (!FDualSenseTriggerEffectGenerator::Bow(Effect, StartPosition, EndPosition, Strength, SnapForce))
        return false;

    return ApplyTriggerEffect(Trigger, Effect);
}

bool FDualSenseWindows::SetAdaptiveTriggerGalloping(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency)
{
    uint8 Effect[11] = {};
    if (!FDualSenseTriggerEffectGenerator::Galloping(Effect, StartPosition, EndPosition, FirstFoot, SecondFoot, Frequency))
        return false;

    return ApplyTriggerEffect(Trigger, Effect);
}

bool FDualSenseWindows::SetAdaptiveTriggerMachine(EDualSenseTrigger Trigger, uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period)
{
    uint8 Effect[11] = {};
    if (!FDualSenseTriggerEffectGenerator::Machine(Effect, StartPosition, EndPosition, AmplitudeA, AmplitudeB, Frequency, Period))
        return false;

    return ApplyTriggerEffect(Trigger, Effect);
}

bool FDualSenseWindows::ClearAdaptiveTriggerEffect(EDualSenseTrigger Trigger)
{
    uint8 Effect[11] = {};
    FDualSenseTriggerEffectGenerator::Off(Effect);
    return ApplyTriggerEffect(Trigger, Effect);
}

void FDualSenseWindows::SetTriggerEffectsEnable(bool bEnable)
{
    if (bEnable)
    {
        Effects.EnableBits1 |= 0x08;
        Effects.EnableBits1 |= 0x04;
    }
    else
    {
        Effects.EnableBits1 &= ~0x08;
        Effects.EnableBits1 &= ~0x04;
    }
}

void FDualSenseWindows::LogDeviceCapabilities(HANDLE Handle, const TCHAR* DevicePath)
{
    HIDD_ATTRIBUTES Attributes{};
    Attributes.Size = sizeof(HIDD_ATTRIBUTES);

    if (!HidD_GetAttributes(Handle, &Attributes))
    {
        return;
    }

    PHIDP_PREPARSED_DATA PreparsedData = nullptr;

    if (!HidD_GetPreparsedData(Handle, &PreparsedData))
    {
        return;
    }

    HIDP_CAPS Caps{};

    if (HidP_GetCaps(PreparsedData, &Caps) != HIDP_STATUS_SUCCESS)
    {
        HidD_FreePreparsedData(PreparsedData);
        return;
    }

    WCHAR SerialBuffer[256]{};
    WCHAR ManufacturerBuffer[256]{};
    WCHAR ProductBuffer[256]{};

    HidD_GetSerialNumberString(
        Handle,
        SerialBuffer,
        sizeof(SerialBuffer));

    HidD_GetManufacturerString(
        Handle,
        ManufacturerBuffer,
        sizeof(ManufacturerBuffer));

    HidD_GetProductString(
        Handle,
        ProductBuffer,
        sizeof(ProductBuffer));

    UE_LOG(LogIndependentInput, Log, TEXT("-------------------------------------------"));
    UE_LOG(LogIndependentInput, Log, TEXT("Device Path : %s"), DevicePath);
    UE_LOG(LogIndependentInput, Log, TEXT("Vendor      : %04X"), Attributes.VendorID);
    UE_LOG(LogIndependentInput, Log, TEXT("Product     : %04X"), Attributes.ProductID);
    UE_LOG(LogIndependentInput, Log, TEXT("Version     : %d"), Attributes.VersionNumber);

    UE_LOG(LogIndependentInput, Log, TEXT("Manufacturer: %s"), ManufacturerBuffer);
    UE_LOG(LogIndependentInput, Log, TEXT("Product Name: %s"), ProductBuffer);
    UE_LOG(LogIndependentInput, Log, TEXT("Serial      : %s"), SerialBuffer);

    UE_LOG(LogIndependentInput, Log, TEXT("Usage Page  : 0x%04X"), Caps.UsagePage);
    UE_LOG(LogIndependentInput, Log, TEXT("Usage       : 0x%04X"), Caps.Usage);

    UE_LOG(LogIndependentInput, Log, TEXT("InputReport : %d"), Caps.InputReportByteLength);
    UE_LOG(LogIndependentInput, Log, TEXT("OutputReport: %d"), Caps.OutputReportByteLength);
    UE_LOG(LogIndependentInput, Log, TEXT("FeatureRpt  : %d"), Caps.FeatureReportByteLength);

    UE_LOG(LogIndependentInput, Log, TEXT("-------------------------------------------"));

    HidD_FreePreparsedData(PreparsedData);
}

bool FDualSenseWindows::ReadState(FDS5State& OutState)
{
    if (DeviceHandle == INVALID_HANDLE_VALUE)
        return false;

    uint8 Report[64];
    DWORD Read = 0;
    BOOL Result = ReadFile(DeviceHandle, Report, sizeof(Report), &Read, nullptr);

    if (!Result || Read < 55)
        return false;

    if (Report[0] != 0x01)
        return false;

    FMemory::Memcpy(&OutState, Report + 1, sizeof(FDS5State));
    return true;
}

bool FDualSenseWindows::ApplyTriggerEffect(EDualSenseTrigger Trigger, const uint8 Effect[11])
{
    uint8* Buffer = GetTriggerBuffer(Trigger);
    FMemory::Memcpy(Buffer, Effect, 11);

    return WriteEffects();
}

bool FDualSenseWindows::WriteEffects()
{
    if (DeviceHandle == INVALID_HANDLE_VALUE)
        return false;

    switch (ConnectionType)
    {
        case EDeviceConnectionType::Wired:
        {
            // USB effects report
            uint8 Report[48] = {};

            // USB Report ID
            Report[0] = 0x02;

            FMemory::Memcpy(Report + 1, &Effects, sizeof(FDS5EffectsState));

            DWORD Written = 0;
            BOOL Result = WriteFile(DeviceHandle, Report, sizeof(Report), &Written, nullptr);

            if (!Result)
            {
                UE_LOG(LogIndependentInput, Warning, TEXT("USB WriteFile failed (%d)"), GetLastError());
                return false;
            }

            return Written == sizeof(Report);
        }

        case EDeviceConnectionType::Wireless:
        {
            // Bluetooth effects report
            uint8 Report[78] = {};

            // Bluetooth Report ID
            Report[0] = 0x31;

            Report[1] = 0x00; // Tag / sequence
            Report[2] = 0x10; // Magic value

            FMemory::Memcpy(Report + 3, &Effects, sizeof(FDS5EffectsState));

            // Bluetooth CRC includes HIDP output header 0xA2.
            uint8 CrcHeader = 0xA2;
            uint32 CRC = FCrc::MemCrc32(&CrcHeader, 1);
            CRC = FCrc::MemCrc32(Report, sizeof(Report) - sizeof(uint32), CRC);

            Report[74] = static_cast<uint8>((CRC >> 0) & 0xFF);
            Report[75] = static_cast<uint8>((CRC >> 8) & 0xFF);
            Report[76] = static_cast<uint8>((CRC >> 16) & 0xFF);
            Report[77] = static_cast<uint8>((CRC >> 24) & 0xFF);

            DWORD Written = 0;
            BOOL Result = WriteFile(DeviceHandle, Report, sizeof(Report), &Written, nullptr);

            if (!Result)
            {
                UE_LOG(LogIndependentInput, Warning, TEXT("Bluetooth WriteFile failed (%d)"), GetLastError());
                return false;
            }

            return Written == sizeof(Report);
        }

        case EDeviceConnectionType::Unknown:
        default:
            return false;
    }
}

void FDualSenseWindows::ResetTriggerEffect(EDualSenseTrigger Trigger)
{
    uint8* Buffer = GetTriggerBuffer(Trigger);

    FMemory::Memzero(Buffer, 11);
}

uint8* FDualSenseWindows::GetTriggerBuffer(EDualSenseTrigger Trigger)
{
    return Trigger == EDualSenseTrigger::Left
        ? Effects.LeftTriggerEffect
        : Effects.RightTriggerEffect;
}

#endif // PLATFORM_WINDOWS