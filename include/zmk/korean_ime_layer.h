#pragma once

#include <stdint.h>

#define KOREAN_IME_MARKER 0xD5

enum korean_ime_state {
    KOREAN_IME_STATE_NONE = 0x00,
    KOREAN_IME_STATE_ENGLISH = 0x01,
    KOREAN_IME_STATE_HANGUL = 0x02,
};
