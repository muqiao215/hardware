#include <8052.h>
#include <stdint.h>

// 6x 74HC595 daisy-chain stopwatch (HHMMSS) with start/stop button.
//
// 74HC595 wiring (verified):
// - P3.7: DS/SER (SI)
// - P3.6: SHCP   (SCK)
// - P3.5: STCP   (RCK)
//
// Button wiring (requested):
// - P2.7: BTN (active-low assumed)
//
// Counts up from 00:00:00. Press button to toggle start/stop.
// Segment codes assume "1 = ON" (common-cathode style), matching your board.

#ifndef FOSC_HZ
#define FOSC_HZ 11059200UL
#endif

#ifndef DIGIT_REVERSE
// Your board showed "rev" order as correct.
#define DIGIT_REVERSE 1
#endif

#ifndef BUTTON_ACTIVE_LOW
#define BUTTON_ACTIVE_LOW 1
#endif

#ifndef DEBOUNCE_MS
#define DEBOUNCE_MS 30u
#endif

#define SI  P3_7
#define SCK P3_6
#define RCK P3_5

#define BTN P2_7

// Timer0 reload for ~1ms tick (mode 1, 16-bit)
#define TICKS_PER_MS (FOSC_HZ / 12UL / 1000UL)
#define TIMER0_RELOAD (65536UL - TICKS_PER_MS)
#define TH0_RELOAD ((uint8_t)((TIMER0_RELOAD >> 8) & 0xFF))
#define TL0_RELOAD ((uint8_t)(TIMER0_RELOAD & 0xFF))

static const uint8_t __code seg_cc[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

static volatile uint16_t ms_in_sec = 0;
static volatile uint8_t tick_1s = 0;

static uint8_t running = 0;
static uint32_t seconds = 0;

static uint8_t btn_is_active(void) {
#if BUTTON_ACTIVE_LOW
  return (BTN == 0) ? 1 : 0;
#else
  return (BTN == 1) ? 1 : 0;
#endif
}

static void shift595_send_byte(uint8_t value) {
  uint8_t i;
  for (i = 0; i < 8; i++) {
    SI = (value & 0x80) ? 1 : 0;
    value <<= 1;
    SCK = 0;
    SCK = 1;
  }
}

static void shift595_latch_6bytes(const uint8_t bytes[6]) {
  uint8_t i;

  RCK = 0;

#if DIGIT_REVERSE
  for (i = 0; i < 6; i++) {
    shift595_send_byte(bytes[5 - i]);
  }
#else
  for (i = 0; i < 6; i++) {
    shift595_send_byte(bytes[i]);
  }
#endif

  RCK = 1;
  RCK = 0;
}

static void render_hhmmss(uint32_t total_seconds, uint8_t out_bytes[6]) {
  uint32_t hh32 = total_seconds / 3600u;
  uint8_t hh = (hh32 > 99u) ? 99u : (uint8_t)hh32;
  uint8_t mm = (uint8_t)((total_seconds / 60u) % 60u);
  uint8_t ss = (uint8_t)(total_seconds % 60u);

  out_bytes[0] = seg_cc[hh / 10u];
  out_bytes[1] = seg_cc[hh % 10u];
  out_bytes[2] = seg_cc[mm / 10u];
  out_bytes[3] = seg_cc[mm % 10u];
  out_bytes[4] = seg_cc[ss / 10u];
  out_bytes[5] = seg_cc[ss % 10u];
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

  ms_in_sec++;
  if (ms_in_sec >= 1000u) {
    ms_in_sec = 0;
    tick_1s = 1;
  }
}

void main(void) {
  uint8_t bytes[6];

  uint8_t btn_stable = 0;
  uint8_t btn_last_sample = 0;
  uint16_t btn_debounce = 0;

  // idle defaults
  SI = 0;
  SCK = 1;
  RCK = 0;

  btn_last_sample = btn_is_active();
  btn_stable = btn_last_sample;

  render_hhmmss(seconds, bytes);
  shift595_latch_6bytes(bytes);

  timer0_init();

  while (1) {
    // Debounce (sampled by tight loop; time base uses ms_in_sec decrement behavior)
    // We approximate elapsed ms by observing ms_in_sec changes: not perfect but stable enough
    // for a simple start/stop button.
    {
      static uint16_t last_ms_in_sec = 0;
      uint16_t current = ms_in_sec;
      uint16_t elapsed;

      if (current >= last_ms_in_sec) {
        elapsed = (uint16_t)(current - last_ms_in_sec);
      } else {
        elapsed = (uint16_t)(1000u - last_ms_in_sec + current);
      }
      last_ms_in_sec = current;

      if (elapsed != 0) {
        uint8_t sample = btn_is_active();
        if (sample == btn_last_sample) {
          if (btn_debounce < (uint16_t)DEBOUNCE_MS) {
            btn_debounce = (uint16_t)(btn_debounce + elapsed);
            if (btn_debounce >= (uint16_t)DEBOUNCE_MS) {
              // stable transition confirmed
              if (btn_stable != sample) {
                btn_stable = sample;
                if (btn_stable) {
                  // Press behavior:
                  // - If running: stop and reset to 00:00:00
                  // - If stopped: start from 00:00:00
                  seconds = 0;
                  running = running ? 0 : 1;
                }
              }
            }
          }
        } else {
          btn_last_sample = sample;
          btn_debounce = 0;
        }
      }
    }

    if (tick_1s) {
      tick_1s = 0;

      if (running) {
        if (seconds < (uint32_t)99u * 3600u + 59u * 60u + 59u) {
          seconds++;
        } else {
          running = 0;
        }
      }

      render_hhmmss(seconds, bytes);
      shift595_latch_6bytes(bytes);
    }
  }
}
