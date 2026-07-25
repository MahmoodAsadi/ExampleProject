// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"

#include "IndependentInputManagerTypes.h"
#include "IndependentInputManagerSettings.generated.h"

/**
 * 
 */
UCLASS(Config = IndependentInputManager, DefaultConfig, meta = (DisplayName = "Independent Input Manager"))
class INDEPENDENTINPUTMANAGER_API UIndependentInputManagerSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	virtual FName GetCategoryName() const override
	{
		return TEXT("Plugins");
	}

	static const UIndependentInputManagerSettings* Get()
	{
		return GetDefault<UIndependentInputManagerSettings>();
	}

	static UIndependentInputManagerSettings* GetMutable()
	{
		return GetMutableDefault<UIndependentInputManagerSettings>();
	}

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager")
	static const UIndependentInputManagerSettings* GetIndependentInputSettings() { return Get(); }

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager")
	static UIndependentInputManagerSettings* GetMutableIndependentInputSettings() { return GetMutable(); }

	bool GetUseDeviceNameAsHardwareDeviceIdentifier() const { return bUseDeviceNameAsHardwareDeviceIdentifier; }
	void DevicePluggedIn(const FJoystickDeviceInfo& InDeviceInfo);
	void DeviceUnplugged(const FJoystickDeviceInfo& InDeviceInfo);
	void UpdatePluggedDeviceInfo(const FJoystickDeviceInfo& InDeviceInfo);

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager")
	bool GetIgnoreXInputDevices() const { return bIgnoreXInputDevices; }

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager")
	bool HasDeviceKeyMapping(const FJoystickDeviceIdentifier& DeviceIdentifier) const { return DevicesKeyMapping.Contains(DeviceIdentifier); }

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager")
	TArray<FJoystickDeviceKeyMapping> GetDevicesKeyMappings() const;

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager", meta = (DisplayName = "FindDeviceKeyMappings"))
	bool K2_FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier, FJoystickDeviceKeyMapping& OutDeviceKeyMapping);

	FJoystickDeviceKeyMapping* FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier);
	const FJoystickDeviceKeyMapping* FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier) const;
	void AddOrUpdateDeviceKeyMapping(const FJoystickDeviceIdentifier& DeviceIdentifier, const FJoystickDeviceKeyMapping& DeviceKeyMapping);
	void GenerateDevicesRuntimeKeys();
	bool GetIgnoreVirtualDevices() const { return bIgnoreVirtualDevices; }
	TMap<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping> GetDevicesKeyMapping() const { return DevicesKeyMapping; }

protected:

	void GenerateRuntimeKeysForDeviceMapping(FJoystickDeviceKeyMapping& DeviceKeyMapping);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Connected Devices")
	TArray<FJoystickDeviceInfo> ConnectedDevices;

	/** Keep Windows XInput devices visible for diagnostics, but let Unreal's XInput interface own their gameplay input. */
	UPROPERTY(Config, EditAnywhere, Category = "Device Filtering")
	bool bIgnoreXInputDevices = true;

	/** Completely exclude SDL virtual devices from discovery, profiles, delegates, and gameplay input. */
	UPROPERTY(Config, EditAnywhere, Category = "Device Filtering")
	bool bIgnoreVirtualDevices = false;

	UPROPERTY(Config, EditAnywhere, Category = "Device Profile")
	TMap<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping> DevicesKeyMapping;

	// Use the device name as the hardware device identifier. Useful if you need specific hardware identification (ie. for icons).
	UPROPERTY(Config, EditAnywhere, Category = "Device Profile")
	bool bUseDeviceNameAsHardwareDeviceIdentifier = true;

};
