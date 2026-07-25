#pragma once

#include "IInputDeviceModule.h"

DECLARE_LOG_CATEGORY_EXTERN(LogIndependentInput, Log, All);

class FIndependentInputDevice;

class FIndependentInputManagerModule : public IInputDeviceModule
{
public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    virtual TSharedPtr<IInputDevice> CreateInputDevice(const TSharedRef<FGenericApplicationMessageHandler>& InMessageHandler) override;

    void* GetSDLDllHandle() const { return SDLDllHandle; }

private:
    void* SDLDllHandle = nullptr;
    TSharedPtr<FIndependentInputDevice> InputDevice;

};
