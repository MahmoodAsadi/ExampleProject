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
 * released) to 9 (fully pulled). Force/strength/amplitude values run 0-8, where 0 always
 * means "effect disabled". Every factory function clamps range-constrained parameters to
 * values accepted by the firmware before generating the payload, so out-of-range input
 * produces the nearest supported effect instead of being rejected.
 */
class INDEPENDENTINPUTMANAGER_API FDualSenseTriggerEffectGenerator
{
public:

    /** Fully disengage the trigger and return the stop to the neutral position. Always succeeds. */
    static void Off(uint8 Out[11]);

    /**
     * Official "Feedback" effect (opcode 0x21). The trigger resists movement beyond Position
     * with a constant force. Formerly (mis-)implemented as "Resistance" using the deprecated
     * Simple_Feedback opcode (0x01).
     * @param Position  Zone the resistance begins at. 0-9.
     * @param Strength  Resistance force. 0-8 (0 disables the effect).
     */
    static bool Feedback(uint8 Out[11], uint8 Position, uint8 Strength);

    /**
     * Official "Weapon" effect (opcode 0x25). Resistance builds from StartPosition to
     * EndPosition, then releases entirely, like pulling the trigger of a gun.
     * @param StartPosition  2-7.
     * @param EndPosition    StartPosition+1 to 8.
     * @param Strength       0-8 (0 disables the effect).
     */
    static bool Weapon(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 Strength);

    /**
     * Official "Vibration" effect (opcode 0x26). The trigger vibrates at Frequency once
     * pulled beyond Position.
     * @param Position   Zone the vibration begins at. 0-9.
     * @param Amplitude  Strength of the vibration. 0-8 (0 disables the effect).
     * @param Frequency  Frequency in Hz (0 disables the effect).
     */
    static bool Vibration(uint8 Out[11], uint8 Position, uint8 Amplitude, uint8 Frequency);

    /**
     * Unofficial "Bow" effect (opcode 0x22, not guaranteed to survive future firmware).
     * Resembles Weapon, but with an added spring-like snap-back force.
     * @param StartPosition  0-8.
     * @param EndPosition    StartPosition+1 to 9.
     * @param Strength       0-8.
     * @param SnapForce      Force of the snap-back. 0-8.
     */
    static bool Bow(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 Strength, uint8 SnapForce);

    /**
     * Unofficial "Galloping" effect (opcode 0x23, not guaranteed to survive future firmware).
     * Rhythmic two-beat cycling between two sub-positions; only clearly perceptible at low
     * frequencies.
     * @param StartPosition  0-8.
     * @param EndPosition    StartPosition+1 to 9.
     * @param FirstFoot      Position of the first "foot" in the cycle. 0-6.
     * @param SecondFoot     Position of the second "foot" in the cycle. FirstFoot+1 to 7.
     * @param Frequency      Frequency in Hz (0 disables the effect).
     */
    static bool Galloping(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 FirstFoot, uint8 SecondFoot, uint8 Frequency);

    /**
     * Unofficial "Machine" effect (opcode 0x27, not guaranteed to survive future firmware).
     * Resembles Vibration, but oscillates between two amplitudes.
     * @param StartPosition  0-8.
     * @param EndPosition    StartPosition+1 to 9.
     * @param AmplitudeA     Primary vibration strength. 0-7.
     * @param AmplitudeB     Secondary vibration strength. 0-7.
     * @param Frequency      Frequency in Hz (0 disables the effect).
     * @param Period         Period of the oscillation between AmplitudeA/B, in tenths of a second.
     */
    static bool Machine(uint8 Out[11], uint8 StartPosition, uint8 EndPosition, uint8 AmplitudeA, uint8 AmplitudeB, uint8 Frequency, uint8 Period);

private:

    static void Clear(uint8 Out[11]);
};
