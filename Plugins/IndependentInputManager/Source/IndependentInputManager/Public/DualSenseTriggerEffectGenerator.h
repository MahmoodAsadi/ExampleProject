// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * Builds the 11-byte adaptive-trigger effect payloads sent to a DualSense controller.
 *
 * This is a byte-for-byte implementation of Sony's official on-wire encoding, as reverse
 * engineered by Nielk1: https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db
 *
 * The trigger's physical pull range is divided into 10 discrete zones, numbered 0 (fully
 * released) to 9 (fully pulled). Supported force, strength, amplitude, and position ranges
 * vary by effect and are documented on each function. Every factory function clamps
 * range-constrained parameters before generating the payload, so out-of-range input produces
 * the nearest supported effect instead of being rejected. A zero value generates Off only
 * where the corresponding parameter comment states that it disables the effect.
 */
class INDEPENDENTINPUTMANAGER_API FDualSenseTriggerEffectGenerator
{
public:

    /**
     * Fully disengage the trigger and return the stop to the neutral position. Always succeeds.
     * @param Out  Receives the 11-byte adaptive-trigger effect payload.
     */
    static void Off(uint8 Out[11]);

    /**
     * Official "Feedback" effect (opcode 0x21). The trigger resists movement beyond Position
     * with a constant force. Formerly (mis-)implemented as "Resistance" using the deprecated
     * Simple_Feedback opcode (0x01).
     * @param Out       Receives the 11-byte adaptive-trigger effect payload.
     * @param Position  Zone where resistance begins. Clamped to 0-9.
     * @param Strength  Resistance force. Clamped to 0-8; 0 disables the effect.
     */
    static bool Feedback(uint8 Out[11], uint8 Position, uint8 Strength);

    /**
     * Official "Weapon" effect (opcode 0x25). Resistance builds from StartPosition to
     * EndPosition, then releases entirely, like pulling the trigger of a gun.
     * @param Out            Receives the 11-byte adaptive-trigger effect payload.
     * @param StartPosition  Starting zone. Clamped to 2-7.
     * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 8.
     * @param Strength       Resistance force. Clamped to 0-8; 0 disables the effect.
     */
    static bool Weapon(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 Strength);

    /**
     * Official "Vibration" effect (opcode 0x26). The trigger vibrates at Frequency once
     * pulled beyond Position.
     * @param Out        Receives the 11-byte adaptive-trigger effect payload.
     * @param Position   Zone where vibration begins. Clamped to 0-9.
     * @param Amplitude  Strength of the vibration. Clamped to 0-8; 0 disables the effect.
     * @param Frequency  Frequency in Hz. Full uint8 range; 0 disables the effect.
     */
    static bool Vibration(uint8 Out[11], uint8 Position, uint8 Amplitude, uint8 Frequency);

    /**
     * Unofficial "Bow" effect (opcode 0x22, not guaranteed to survive future firmware).
     * Resembles Weapon, but with an added spring-like snap-back force.
     * @param Out            Receives the 11-byte adaptive-trigger effect payload.
     * @param StartPosition  Starting zone. Clamped to the effective range 0-7.
     * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 8.
     * @param Strength       Resistance force. Clamped to 0-8; 0 disables the effect.
     * @param SnapForce      Snap-back force. Clamped to 0-8; 0 disables the effect.
     */
    static bool Bow(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce);

    /**
     * Unofficial "Galloping" effect (opcode 0x23, not guaranteed to survive future firmware).
     * Rhythmic two-beat cycling between two sub-positions; only clearly perceptible at low
     * frequencies.
     * @param Out            Receives the 11-byte adaptive-trigger effect payload.
     * @param StartPosition  Starting zone. Clamped to 0-8.
     * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 9.
     * @param FirstFoot      First cycle position. Clamped to 0-6.
     * @param SecondFoot     Second cycle position. Clamped to FirstFoot+1 through 7.
     * @param Frequency      Frequency in Hz. Full uint8 range; 0 disables the effect.
     */
    static bool Galloping(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency);

    /**
     * Unofficial "Machine" effect (opcode 0x27, not guaranteed to survive future firmware).
     * Resembles Vibration, but oscillates between two amplitudes.
     * @param Out            Receives the 11-byte adaptive-trigger effect payload.
     * @param StartPosition  Starting zone. Clamped to 0-8.
     * @param EndPosition    Ending zone. Clamped to StartPosition+1 through 9.
     * @param AmplitudeA     Primary cycling strength. Clamped to 0-7.
     * @param AmplitudeB     Secondary cycling strength. Clamped to 0-7.
     * @param Frequency      Frequency in Hz. Full uint8 range; 0 disables the effect.
     * @param Period         Oscillation period in tenths of a second. Full uint8 range.
     */
    static bool Machine(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period);

private:

    static void Clear(uint8 Out[11]);
};
