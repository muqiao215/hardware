# Release Notes

Release status: `v2.0.0-soft-validated`

## Monorepo Note

This repository is now the single git home for three subprojects:

- STM32F103 bare-metal lab
- MCS-51 / STC89C52 migration track
- Raspberry Pi High-Precision AD/DA Board software toolkit

This does not mean one unified build system.

It means:

- one repository
- one top-level README
- one aggregate no-hardware gate
- separate build/runtime boundaries per subproject

## What This Version Is

This is the first repository version that is ready to hand off as a structured bare-metal STM32F103 lab baseline on Raspberry Pi.

It is no longer only a migration snapshot from Keil projects. It is now a repeatable engineering baseline with:

- unified project layout
- unified build flow
- unified no-hardware quality gate
- reusable shared modules
- per-example verification notes
- validation bundles for later board-side testing

## What You Can Do With It

- Build every migrated example with GNU Arm Embedded + CMake + Ninja
- Generate `.elf`, `.hex`, `.bin`, and `.map` for every example
- Run host-side tests for extracted pure logic modules
- Package each example into a validation bundle that another person can flash and verify on real hardware
- Browse imported `STC89C52` source material in the same repository

## What This Version Does Not Claim

- It does not claim ST-Link probe success in the current Raspberry Pi environment
- It does not claim flash success
- It does not claim real-board runtime behavior has been observed
- It does not claim every legacy compressed course example was reproduced byte-for-byte
- It does not claim `STC89C52` has been migrated into the same CLI build workflow yet

## Why Hardware Is Still Pending

The current environment still reports:

```text
Found 0 stlink programmers
```

So the project can be treated as:

- software-side migration complete for v2
- hardware-side execution package prepared
- final board verification still waiting for physical connection

This version should be read as:

- software closure: complete
- hardware closure: pending execution on a real bench

## Recommended Next Step

On a machine or bench with a connected board and ST-Link:

1. Run `scripts/probe-stlink.sh`
2. Generate or unpack a validation bundle
3. Run `scripts/flash.sh <example>`
4. Follow `REALBOARD_VALIDATION_CHECKLIST.md`
5. Verify wiring, expected behavior, and serial output against the bundled documentation
