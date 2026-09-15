#ifndef STC89C52_MIN_H
#define STC89C52_MIN_H

/*
 * Minimal SDCC-native STC89C52 helpers for the first migration spike.
 *
 * This is intentionally not a full REGX52.H compatibility layer. Add only the
 * SFR/sbit definitions that an active migrated example actually needs.
 */

#include <stdint.h>

__sfr __at(0x87) PCON;
__sfr __at(0x88) TCON;
__sfr __at(0x89) TMOD;
__sfr __at(0x8C) TH0;
__sfr __at(0x8A) TL0;
__sfr __at(0x8D) TH1;
__sfr __at(0x8B) TL1;
__sfr __at(0x98) SCON;
__sfr __at(0x99) SBUF;
__sfr __at(0xA8) IE;

__sbit __at(0xA0) STC89C52_P20;
__sbit __at(0xB1) STC89C52_P31;
__sbit __at(0x88) IT0;
__sbit __at(0x89) IE0;
__sbit __at(0x8A) IT1;
__sbit __at(0x8B) IE1;
__sbit __at(0x8C) TR0;
__sbit __at(0x8D) TF0;
__sbit __at(0x8E) TR1;
__sbit __at(0x8F) TF1;
__sbit __at(0x98) RI;
__sbit __at(0x99) TI;
__sbit __at(0xAB) ET1;

#endif
