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
	bool HasDeviceKeyMapping(const FJoystickDeviceIdentifier& DeviceIdentifier) const { return DevicesKeyMapping.Contains(DeviceIdentifier); }

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager")
	TArray<FJoystickDeviceKeyMapping> GetDevicesKeyMappings() const;

	UFUNCTION(BlueprintPure, Category = "Independent Input Manager", meta = (DisplayName = "FindDeviceKeyMappings"))
	bool K2_FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier, FJoystickDeviceKeyMapping& OutDeviceKeyMapping);

	bool GetSensorEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier, EDeviceSensorType Sensor) const;

	/**
	 * Enables or disables a sensor in the profile identified by DeviceIdentifier.
	 * Connected devices using the profile are reconnected to rebuild their
	 * runtime state when the setting changes.
	 */
	bool SetSensorEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, EDeviceSensorType Sensor, bool bEnable);

	/**
	 * Enables or disables a sensor in every profile that contains that sensor.
	 * Connected devices using an updated profile are reconnected to rebuild
	 * their runtime state.
	 */
	void SetSensorEnableForAllDevices(EDeviceSensorType Sensor, bool bEnable);

	/** Returns whether the device profile reports rumble support. */
	bool GetRumbleSupported(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	/** Returns whether rumble is enabled in the device profile. */
	bool GetRumbleEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	/** Returns whether the device profile reports trigger-rumble support. */
	bool GetTriggerRumbleSupported(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	/** Returns whether trigger rumble is enabled in the device profile. */
	bool GetTriggerRumbleEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	/** Returns whether the device profile reports adaptive-trigger support. */
	bool GetAdaptiveTriggerEffectsSupported(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	/** Returns whether adaptive-trigger effects are enabled in the device profile. */
	bool GetAdaptiveTriggerEffectsEnabled(const FJoystickDeviceIdentifier& DeviceIdentifier) const;

	/**
	 * Enables or disables rumble in the identified device profile. Connected
	 * devices using the profile are reconnected when the setting changes.
	 */
	bool SetRumbleEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, bool bEnable);

	/**
	 * Enables or disables trigger rumble in the identified device profile.
	 * Connected devices using the profile are reconnected when the setting changes.
	 */
	bool SetTriggerRumbleEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, bool bEnable);

	/**
	 * Enables or disables adaptive-trigger effects in the identified device
	 * profile. Active effects are cleared before connected devices using the
	 * profile are reconnected.
	 */
	bool SetAdaptiveTriggerEffectsEnable(const FJoystickDeviceIdentifier& DeviceIdentifier, bool bEnable);

	/** Enables or disables rumble for every device profile. */
	void SetRumbleEnableForAllDevices(bool bEnable);

	/** Enables or disables trigger rumble for every device profile. */
	void SetTriggerRumbleEnableForAllDevices(bool bEnable);

	/** Enables or disables adaptive-trigger effects for every device profile. */
	void SetAdaptiveTriggerEffectsEnableForAllDevices(bool bEnable);

	FJoystickDeviceKeyMapping* FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier);
	const FJoystickDeviceKeyMapping* FindDeviceKeyMappings(const FJoystickDeviceIdentifier& DeviceIdentifier) const;
	void AddOrUpdateDeviceKeyMapping(const FJoystickDeviceIdentifier& DeviceIdentifier, const FJoystickDeviceKeyMapping& DeviceKeyMapping);
	void GenerateDevicesRuntimeKeys();
	bool GetIgnoreXInputDevices() const { return bIgnoreXInputDevices; }
	bool GetIgnoreSteamInputDevices() const { return bIgnoreSteamInputDevices; }
	bool GetIgnoreVirtualDevices() const { return bIgnoreVirtualDevices; }
	bool GetForceDevicesForSingleUser() const { return bForceAllDevicesForSingleUser; }
	TMap<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping> GetDevicesKeyMapping() const { return DevicesKeyMapping; }

protected:

	void GenerateRuntimeKeysForDeviceMapping(FJoystickDeviceKeyMapping& DeviceKeyMapping);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Connected Devices")
	TArray<FJoystickDeviceInfo> ConnectedDevices;

	/**
	 * Keeps Windows XInput devices visible for diagnostics while allowing
	 * Unreal's XInput interface to own their gameplay input. To let Independent
	 * Input Manager own these devices instead, disable this option and Unreal's
	 * XInput Device plugin to avoid duplicate input.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Device Filtering")
	bool bIgnoreXInputDevices = true;

	/**
	 * Keeps Valve input devices visible for diagnostics while allowing Unreal's
	 * Steam Controller interface to own their gameplay input. Valve controllers
	 * have not yet been tested with Independent Input Manager. To let this plugin
	 * own them, disable this option and Unreal's Steam Controller plugin to avoid
	 * duplicate input.
	 */
	UPROPERTY(Config, EditAnywhere, Category = "Device Filtering")
	bool bIgnoreSteamInputDevices = true;

	/** Completely exclude SDL virtual devices from discovery, profiles, delegates, and gameplay input. */
	UPROPERTY(Config, EditAnywhere, Category = "Device Filtering")
	bool bIgnoreVirtualDevices = false;

	/** Maps every plugin-owned input device to Unreal's primary platform user. */
	UPROPERTY(Config, EditAnywhere, Category = "Device Profile")
	bool bForceAllDevicesForSingleUser = false;

	UPROPERTY(Config, VisibleAnywhere, Category = "Device Profile")
	TMap<FJoystickDeviceIdentifier, FJoystickDeviceKeyMapping> DevicesKeyMapping;

	// Use the device name as the hardware device identifier. Useful if you need specific hardware identification (ie. for icons).
	UPROPERTY(Config, EditAnywhere, Category = "Device Profile")
	bool bUseDeviceNameAsHardwareDeviceIdentifier = true;

};
