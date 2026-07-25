# Independent Input Manager: Found Issues

This file is the backlog for confirmed, unresolved issues found while auditing the
`IndependentInputManager` plugin. Work through one checkbox at a time and keep the
issue ID in any related discussion or commit.

Last audit: 2026-07-25

Audit scope:

- Runtime and editor modules under `Plugins/IndependentInputManager/Source`
- Plugin descriptor, module build rules, and project/plugin configuration
- Existing UnrealBuildTool SARIF diagnostics
- The latest `Saved/Logs/ExampleProject.log`

Priority meanings:

- `P1`: critical data loss, crash, or primary functionality failure
- `P2`: important correctness or lifecycle defect
- `P3`: lower-risk defect, compatibility gap, or incomplete feature

## Tasks

### [x] IIM-001 [P2] Respect the non-primary input-device creation contract

Evidence:

- `FIndependentInputManagerModule::CreateInputDevice(..., FInputDeviceCreationParameters)`
  always creates an input device and initializes the engine subsystem, even when
  `bInitAsPrimaryDevice` is false.
- `FIndependentInputDevice` stores `bIsPrimaryDevice`, but never uses it.
- Unreal's default `IInputDeviceModule` contract returns `nullptr` for a
  non-primary request unless a module deliberately implements an observation-only
  device.

Impact:

- An advanced caller requesting a device that is not part of the game input
  system still gets a fully active game-input device.
- A second creation can replace the subsystem's current input-device pointer and
  interfere with the primary instance.

Suggested direction:

- Return `nullptr` when `bInitAsPrimaryDevice` is false unless a real
  observation-only mode is implemented.
- Route the primary overload through one initialization path.
- Remove `bIsPrimaryDevice` if it is no longer needed.

Acceptance criteria:

- A non-primary creation request does not initialize SDL, replace the subsystem's
  primary device, or emit game input.
- Primary creation continues to initialize exactly once.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputManager.cpp:48`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputDevice.cpp:19`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Public/IndependentInputDevice.h:18`

Resolution:

- The redundant two-parameter override was removed.
- The module now inherits Unreal's default behavior: non-primary requests return
  `nullptr`, while primary requests delegate to the one-parameter overload.
- The unused `bIsPrimaryDevice` state and constructor parameter were removed.
- The explicit implementation built successfully before this cleanup; rebuilding the
  inherited-overload version was blocked because Unreal Live Coding was active.

### [ ] IIM-002 [P2] Make device-filter settings authoritative and consistent

Evidence:

- An existing profile is accepted before `bIgnoreXInputDevices` is checked, so an
  Xbox profile created while filtering was disabled can continue to use the
  independent input path after filtering is enabled.
- `bIgnoreVirtualDevices` is checked only when the runtime input device receives
  the connection. The subsystem has already opened the device, generated and
  saved a profile, added it to connected-device maps, and later broadcasts it.

Impact:

- Changing the XInput filter can fail to prevent duplicate Xbox input.
- Ignored virtual devices can still create project configuration and appear in
  connection delegates/editor lists.
- The meaning of "ignore" differs between the two filters.

Suggested direction:

- Centralize filtering in one helper evaluated for every connection, including
  devices with existing profiles.
- Decide whether ignored devices should remain visible as metadata. If they
  should, separate discovery from profile generation and input registration.
- Treat `bUseIndependentInputAPI` as derived connection state instead of stale
  profile data, or recompute it on every connection.

Acceptance criteria:

- Enabling XInput filtering prevents independent input even when an Xbox profile
  already exists.
- Enabling virtual-device filtering does not generate profiles or input events
  for virtual devices.
- Connection delegates/editor visibility follow the documented policy.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:793`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputDevice.cpp:132`

### [ ] IIM-003 [P2] Support devices that do not report both VID and PID

Evidence:

- `FJoystickDeviceIdentifier::IsValid()` requires both `VendorId` and `ProductId`
  to be nonzero.
- `FJoystickDeviceInfo::IsValid()` delegates to that rule.
- Profile generation and runtime registration return early for an invalid device
  even after SDL opened it successfully.

Impact:

- Generic, unusual, and some virtual SDL devices that report an unknown VID or
  PID can be listed as connected but receive no profile and emit no input.

Suggested direction:

- Add a stable fallback profile identity, preferably based on the SDL joystick
  GUID, with a sanitized name only as a final fallback.
- Do not use the SDL instance ID for persisted profile identity because it changes
  across physical reconnects.
- Plan migration for existing VID/PID-keyed configuration.

Acceptance criteria:

- A joystick with a zero VID or PID can receive a stable profile and produce
  input.
- Reconnecting the same device reuses its profile.
- Existing VID/PID profiles continue to load.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Public/IndependentInputManagerTypes.h:298`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Public/IndependentInputManagerTypes.h:479`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:793`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputDevice.cpp:132`

### [ ] IIM-004 [P2] Guard touchpad paired-key creation when the subsystem is unavailable

Evidence:

- Individual custom keys call `GenerateRuntimeKeyIfNeeded()`, which safely checks
  whether `UIndependentInputSubsystem::Get()` returned a subsystem.
- Touchpad generation then unconditionally dereferences the same subsystem to
  create the paired 2D key.

Impact:

- Applying or generating a touchpad mapping during early startup, late shutdown,
  a commandlet, or failed subsystem initialization can dereference null.

Suggested direction:

- Guard `CreateDevicePairedKey()` with the same subsystem-validity check.
- Consider making the whole generation operation report whether registration was
  completed, so callers can defer it safely.

Acceptance criteria:

- Mapping generation with no input subsystem does not crash.
- Component and paired touchpad keys are registered once the subsystem becomes
  available.

Relevant location:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputManagerSettings.cpp:79`

### [ ] IIM-005 [P2] Regenerate custom runtime keys after a key is renamed

Evidence:

- `FIndependentInputKey::RuntimeKey` is persisted inside the config-backed device
  mapping.
- The mapping editor copies that runtime key into its temporary object.
- `CreateDeviceKey()` returns the existing runtime key when it is still valid,
  before deriving a key from the edited `KeyName`.

Impact:

- Renaming a custom key during the same editor session can leave the mapping bound
  to the old key until restart.
- Touchpad paired keys can be generated from a new display name while still
  referencing stale X/Y component keys.

Suggested direction:

- Treat `RuntimeKey` as transient derived state.
- Derive the expected key name every time and regenerate when the stored key does
  not match it.
- Clear or migrate already serialized `RuntimeKey` values in existing config.

Acceptance criteria:

- Renaming a custom key and saving immediately updates dispatched input to the new
  key.
- Touchpad 2D keys reference the newly generated X/Y keys.
- Restarting the editor preserves the authored name without relying on a
  serialized runtime cache.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Public/IndependentInputManagerTypes.h:578`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Public/IndependentInputManagerTypes.h:593`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:51`
- `Config/DefaultIndependentInputManager.ini:4`

### [ ] IIM-006 [P3] Validate custom key-name collisions and key types

Evidence:

- Custom key names are sanitized and combined with the profile ID without
  checking uniqueness.
- If a key already exists, `CreateDeviceKey()` reuses it without verifying its
  display name or whether it was registered as a button versus `Axis1D`.

Impact:

- Two controls can silently emit the same key.
- Reusing one custom name for a button and an axis makes whichever registration
  happened first determine the key type.

Suggested direction:

- Validate all derived key names before saving a profile.
- Reject or clearly report empty names, duplicates, and button/axis type
  conflicts.
- Keep validation deterministic across `TMap` iteration order.

Acceptance criteria:

- A conflicting profile cannot be saved silently.
- Valid profiles register every custom key with the intended key flags.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Public/IndependentInputManagerTypes.h:619`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:51`
- `Plugins/IndependentInputManager/Source/IndependentInputManagerEditor/Private/Slates/SDeviceKeyMapping.cpp:465`

### [ ] IIM-007 [P2] Do not write project default config from runtime device discovery

Evidence:

- Every `AddOrUpdateDeviceKeyMapping()` call invokes
  `TryUpdateDefaultConfigFile()`.
- First-time profile generation is part of the runtime subsystem and can execute
  in packaged builds, not only in the editor.

Impact:

- Connecting an unknown controller can attempt to modify the project's default
  configuration at runtime.
- Packaged installs may be read-only, causing warnings and losing the generated
  profile after exit; writable installs may be modified unexpectedly.

Suggested direction:

- Restrict project-default authoring to editor workflows.
- In non-editor builds, either keep generated fallback mappings in memory or save
  user-specific mappings to an explicit writable user-config/save location.
- Check and report persistence failures where persistence is expected.

Acceptance criteria:

- Runtime discovery never attempts to modify `DefaultIndependentInputManager.ini`.
- Editor-authored profiles still persist to project defaults.
- Any intended player-authored runtime profile persists in a writable user
  location.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputManagerSettings.cpp:63`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:1048`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:1101`

### [ ] IIM-008 [P3] Allow live preview selection among identical controllers

Evidence:

- Profiles are correctly shared by VID/PID.
- `FindDeviceInfoByIdentifier()` returns the first matching connected instance
  from a `TMap`.
- The mapping editor stores only that one instance ID for live input previews.

Impact:

- With two identical controllers, the editor cannot choose which physical
  instance drives the button/axis/touch/sensor preview.
- The chosen preview device is dependent on map iteration order.

Suggested direction:

- Keep profile selection keyed by stable identifier, but add a separate connected
  instance selector when multiple matching devices are present.
- Sort instance choices by SDL instance ID for deterministic display.

Acceptance criteria:

- Every connected instance sharing a profile can be selected for live preview.
- Profile edits still apply to all devices sharing that profile.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:150`
- `Plugins/IndependentInputManager/Source/IndependentInputManagerEditor/Private/Slates/SDeviceKeyMapping.cpp:123`

### [ ] IIM-009 [P3] Implement or explicitly reject joystick ball input

Evidence:

- Device information reports `NumberOfBalls`.
- `SDL_EVENT_JOYSTICK_BALL_MOTION` is consumed without producing state or input.
- Default mapping generation contains a TODO for ball keys.

Impact:

- Trackball-capable joysticks appear supported but their ball motion is silently
  discarded.

Suggested direction:

- Add ball mappings/state/events, or explicitly document ball input as
  unsupported and avoid presenting it as an actionable capability.

Acceptance criteria:

- Ball motion either produces documented Unreal input events or is clearly
  reported as unsupported without silent ambiguity.

Relevant locations:

- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputSubsystem.cpp:440`
- `Plugins/IndependentInputManager/Source/IndependentInputManager/Private/IndependentInputManagerSettings.cpp:172`

## Audit Notes

- Existing compiler SARIF files contain no diagnostics.
- The latest editor log completed controller removal and plugin shutdown without
  an Independent Input assertion or error.
- This was a read-only implementation audit. No plugin C++ or configuration was
  changed.

## Intentional or Previously Resolved Behaviors

Do not reopen these as tasks without new evidence or a changed requirement:

- SDL instance IDs, and therefore newly allocated Unreal device IDs, can increase
  after a physical disconnect/reconnect.
- The current 100 ms force-feedback submission behavior is intentional for this
  project.
- The synchronous LED fade during final device close is an accepted shutdown
  behavior.
- The editor selection fixes, mapping callback lifetime fixes, fast
  press/release transition queue, `FInputDeviceScope` hardware handle fix,
  temporary mapping UObject GC protection, and virtual-axis-button dispatch fix
  were already addressed.
