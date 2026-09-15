#include <8052.h>
#include <stdint.h>

// 6x 74HC595 daisy-chain probe (no multiplexing)
// - P3.7: DS/SER (SI)
// - P3.6: SHCP   (SCK)
// - P3.5: STCP   (RCK)
//
// Shows "012345" but automatically toggles:
// - byte order (normal vs reversed)
// - segment polarity (direct vs inverted)
// every ~2 seconds, so you can visually pick the correct mode.

#ifndef FOSC_HZ
#define FOSC_HZ 11059200UL
#endif

#define SI  P3_7
#define SCK P3_6
#define RCK P3_5

#define TICKS_PER_MS (FOSC_HZ / 12UL / 1000UL)
#define TIMER0_RELOAD (65536UL - TICKS_PER_MS)
#define TH0_RELOAD ((uint8_t)((TIMER0_RELOAD >> 8) & 0xFF))
#define TL0_RELOAD ((uint8_t)(TIMER0_RELOAD & 0xFF))

static const uint8_t __code seg_cc[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static volatile uint16_t ms = 0;
static volatile uint8_t mode = 0; // 0..3

static void shift595_send_byte(uint8_t value) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    SI = (value & 0x80) ? 1 : 0;
    value <<= 1;
    SCK = 0;
    SCK = 1;
  }
}

static void shift595_latch_6bytes(const uint8_t bytes[6], uint8_t reverse, uint8_t invert) {
  uint8_t i;

  // shift in while latch is low, then pulse high once at the end
  RCK = 0;

  if (!reverse) {
    for (i = 0; i < 6; i++) {
      uint8_t v = bytes[i];
      if (invert) v = (uint8_t)~v;
      shift595_send_byte(v);
    }
  } else {
    for (i = 0; i < 6; i++) {
      uint8_t v = bytes[5 - i];
      if (invert) v = (uint8_t)~v;
      shift595_send_byte(v);
    }
  }

  RCK = 1;
  RCK = 0;
}

static void timer0_init(void) {
  TMOD = (TMOD & 0xF0) | 0x01;
  TR0 = 0;
  TF0 = 0;
  TH0 = TH0_RELOAD;
  TL0 = TL0_RELOAD;
  ET0 = 1;
  EA = 1;
  TR0 = 1;
}

void timer0_isr(void) __interrupt(1) {
  TH0 = TH0_RELOAD;
  TL0 = TL0_RELOAD;
  ms++;
  if (ms >= 2000) { // change mode every ~2s
    ms = 0;
    mode++;
    if (mode >= 4) mode = 0;
  }
}

void main(void) {
  static const uint8_t digits[6] = {0, 1, 2, 3, 4, 5};
  uint8_t bytes[6];

  SI = 0;
  SCK = 1;
  RCK = 0;

  timer0_init();

  while (1) {
    uint8_t i;
    uint8_t reverse = (mode & 0x01) ? 1 : 0;
    uint8_t invert = (mode & 0x02) ? 1 : 0;

    for (i = 0; i < 6; i++) {
      bytes[i] = seg_cc[digits[i]];
    }

    shift595_latch_6bytes(bytes, reverse, invert);
  }
}

