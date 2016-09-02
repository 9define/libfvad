/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This header file includes the inline functions in
// the fix point signal processing library.

#ifndef COMMON_AUDIO_SIGNAL_PROCESSING_INCLUDE_SPL_INL_H_
#define COMMON_AUDIO_SIGNAL_PROCESSING_INCLUDE_SPL_INL_H_

#include "rtc_base/compile_assert_c.h"

extern const int8_t kWebRtcSpl_CountLeadingZeros32_Table[64];

// Don't call this directly except in tests!
static __inline int WebRtcSpl_CountLeadingZeros32_NotBuiltin(uint32_t n) {
  // Normalize n by rounding up to the nearest number that is a sequence of 0
  // bits followed by a sequence of 1 bits. This number has the same number of
  // leading zeros as the original n. There are exactly 33 such values.
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;

  // Multiply the modified n with a constant selected (by exhaustive search)
  // such that each of the 33 possible values of n give a product whose 6 most
  // significant bits are unique. Then look up the answer in the table.
  return kWebRtcSpl_CountLeadingZeros32_Table[(n * 0x8c0b2891) >> 26];
}

// Don't call this directly except in tests!
static __inline int WebRtcSpl_CountLeadingZeros64_NotBuiltin(uint64_t n) {
  const int leading_zeros = n >> 32 == 0 ? 32 : 0;
  return leading_zeros + WebRtcSpl_CountLeadingZeros32_NotBuiltin(
                             (uint32_t)(n >> (32 - leading_zeros)));
}

// Returns the number of leading zero bits in the argument.
static __inline int WebRtcSpl_CountLeadingZeros32(uint32_t n) {
#ifdef __GNUC__
  RTC_COMPILE_ASSERT(sizeof(unsigned int) == sizeof(uint32_t));
  return n == 0 ? 32 : __builtin_clz(n);
#else
  return WebRtcSpl_CountLeadingZeros32_NotBuiltin(n);
#endif
}

// Returns the number of leading zero bits in the argument.
static __inline int WebRtcSpl_CountLeadingZeros64(uint64_t n) {
#ifdef __GNUC__
  RTC_COMPILE_ASSERT(sizeof(unsigned long long) == sizeof(uint64_t));  // NOLINT
  return n == 0 ? 64 : __builtin_clzll(n);
#else
  return WebRtcSpl_CountLeadingZeros64_NotBuiltin(n);
#endif
}


static __inline int16_t WebRtcSpl_GetSizeInBits(uint32_t n) {
  int16_t bits;

  if (0xFFFF0000 & n) {
    bits = 16;
  } else {
    bits = 0;
  }
  if (0x0000FF00 & (n >> bits)) bits += 8;
  if (0x000000F0 & (n >> bits)) bits += 4;
  if (0x0000000C & (n >> bits)) bits += 2;
  if (0x00000002 & (n >> bits)) bits += 1;
  if (0x00000001 & (n >> bits)) bits += 1;

  return bits;
}

static __inline int16_t WebRtcSpl_NormW32(int32_t a) {
  int16_t zeros;

  if (a == 0) {
    return 0;
  }
  else if (a < 0) {
    a = ~a;
  }

  if (!(0xFFFF8000 & a)) {
    zeros = 16;
  } else {
    zeros = 0;
  }
  if (!(0xFF800000 & (a << zeros))) zeros += 8;
  if (!(0xF8000000 & (a << zeros))) zeros += 4;
  if (!(0xE0000000 & (a << zeros))) zeros += 2;
  if (!(0xC0000000 & (a << zeros))) zeros += 1;

  return zeros;
}

static __inline int16_t WebRtcSpl_NormU32(uint32_t a) {
  int16_t zeros;

  if (a == 0) return 0;

  if (!(0xFFFF0000 & a)) {
    zeros = 16;
  } else {
    zeros = 0;
  }
  if (!(0xFF000000 & (a << zeros))) zeros += 8;
  if (!(0xF0000000 & (a << zeros))) zeros += 4;
  if (!(0xC0000000 & (a << zeros))) zeros += 2;
  if (!(0x80000000 & (a << zeros))) zeros += 1;

  return zeros;
}


#endif  // WEBRTC_SPL_SPL_INL_H_
