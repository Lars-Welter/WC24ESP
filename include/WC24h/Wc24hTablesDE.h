// Word clock tables of the WordClock24h firmware (layout 16x18, German).
//
// Copied from wordclock24h v3.1.5, make-tables/tables24h-de.{c,h}
// https://github.com/ukw100/wordclock24h
// Copyright (c) 2014-2026 Frank Meyer - frank(at)uclock.de
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// The table contents are unchanged from the original, only wrapped into the
// wc24h namespace.

#pragma once

#include <stdint.h>

namespace wc24h {

constexpr uint8_t WC_ROWS = 16;
constexpr uint8_t WC_COLUMNS = 18;
constexpr uint8_t DISPLAY_MODES_COUNT = 27;
constexpr uint8_t HOUR_COUNT = 24;
constexpr uint8_t MINUTE_COUNT = 60;

constexpr uint8_t MAX_HOUR_WORDS = 6;
constexpr uint8_t MAX_MINUTE_WORDS = 7;
constexpr uint8_t DISPLAY_MODE_TEXT_LEN = 32;

constexpr uint8_t WP_IF_HOUR_IS_0 = 0xFE;
constexpr uint8_t WP_IF_MINUTE_IS_0 = 0xFF;

constexpr uint8_t MDF_IT_IS_0 = 0x00;
constexpr uint8_t MDF_IT_IS_1 = 0x01;
constexpr uint8_t MDF_HOUR_OFFSET_0 = 0x00;
constexpr uint8_t MDF_HOUR_OFFSET_1 = 0x02;
constexpr uint8_t MDF_HOUR_OFFSET_2 = 0x04;

constexpr uint8_t ILLUMINATION_LEN_MASK = 0x1F;
constexpr uint8_t ILLUMINATION_FLAG_IT_IS = 0x80;
constexpr uint8_t ILLUMINATION_FLAG_AM = 0x40;
constexpr uint8_t ILLUMINATION_FLAG_PM = 0x20;

struct MINUTEDISPLAY {
    uint8_t flags;
    uint8_t word_idx[MAX_MINUTE_WORDS];
};

struct WORD_ILLUMINATION {
    uint8_t row;
    uint8_t col;
    uint8_t len;
};

struct DISPLAY_MODES {
    uint8_t hour_idx;
    uint8_t minute_idx;
    char description[DISPLAY_MODE_TEXT_LEN + 1];
};

enum HourMode
{
    HM_0,                                                       // 0 = Mode 0: "leer, fuer Temperaturanzeige"
    HM_1,                                                       // 1 = Mode 1: "MM - HH (12)"
    HM_2,                                                       // 2 = Mode 3: "MM - HH UHR (12) NACHTS"
    HM_3,                                                       // 3 = Mode 4: "MM - HH UHR (24)"
    HM_4,                                                       // 4 = Mode 5: "HH UHR (12) - MM"
    HM_5,                                                       // 5 = Mode 6: "HH UHR 24) - MM"
    HM_6,                                                       // 6 = Mode 7: "MITTERNACHT (0 UHR)"
    HOUR_MODES_COUNT                                            // Number of HourModes
};

enum MinuteMode
{
    MM_1,                                                       //  0 = Mode  1: "MM NACH"
    MM_2,                                                       //  1 = Mode  2: "MM MINUTEN NACH"
    MM_3,                                                       //  2 = Mode  3: "OSSI - MM MINUTEN NACH (VIERTEL NACH, HALB, VIERTEL VOR)"
    MM_4,                                                       //  3 = Mode  4: "OESI - MINUTEN NACH (VIERTEL NACH, HALB, DREIVIERTEL)"
    MM_5,                                                       //  4 = Mode  5: "RHEIN/ RUHR - MINUTEN NACH (VIERTEL, HALB, DREIVIERTEL)"
    MM_6,                                                       //  5 = Mode  6: "SCHWABEN - MM MINUTEN NACH (VIERTEL NACH, HALB, DREIVIERTEL)"
    MM_7,                                                       //  6 = Mode  7: "WESSI - MM MINUTEN NACH (VIERTEL, HALB, DREIVIERTEL)"
    MM_8,                                                       //  7 = Mode  8: "TIROL - VIERTEL NACH, aber DREIVIERTEL"
    MM_9,                                                       //  8 = Mode  9: "MM"
    MM_10,                                                      //  9 = Mode 10: "UND MM MINUTEN"
    MM_11,                                                      // 10 = Mode 11: "MM MINUTEN VOR"
    MM_12,                                                      // 11 = Mode 12: "JESTER"
    MM_13,                                                      // 11 = Mode 13: "OSSI - MM NACH (VIERTEL NACH, HALB, VIERTEL VOR)"
    MM_14,                                                      // 11 = Mode 14: "OESI - NACH (VIERTEL NACH, HALB, DREIVIERTEL)"
    MM_15,                                                      // 11 = Mode 15: "RHEIN/ RUHR - MM NACH (VIERTEL, HALB, DREIVIERTEL)"
    MM_16,                                                      // 11 = Mode 16: "SCHWABEN - MM NACH (VIERTEL NACH, HALB, DREIVIERTEL)"
    MM_17,                                                      // 11 = Mode 17: "WESSI - MM NACH (VIERTEL, HALB, DREIVIERTEL)"
    MM_18,                                                      // 11 = Mode 18: "TIROL - MM NACH (VIERTEL NACH, aber DREIVIERTEL)"
    MM_19,                                                      // 12 = Mode 19: "TEMPERATUR CC GRAD"
    MINUTE_MODES_COUNT                                          // Number of MinuteModes
};

enum WordPos
{
    WP_END_OF_WORDS,                                            //  0 = ""
    WP_ES,                                                      //  1 = "ES"
    WP_IST,                                                     //  3 = "IST"
    WP_VIERTEL_1,                                               //  5 = "VIERTEL"
    WP_EIN_1,                                                   //  6 = "EIN"
    WP_EINS_1,                                                  //  7 = "EINS"
    WP_IN,                                                      //  8 = "IN"
    WP_DREI_1,                                                  //  9 = "DREI"
    WP_EIN_2,                                                   // 10 = "EIN"
    WP_EINE_1,                                                  // 11 = "EINE"
    WP_EINER,                                                   // 12 = "EINER"
    WP_SECH_1,                                                  // 13 = "SECH"
    WP_SECHS_1,                                                 // 14 = "SECHS"
    WP_SIEB_1,                                                  // 15 = "SIEB"
    WP_SIEBEN_1,                                                // 16 = "SIEBEN"
    WP_ELF_1,                                                   // 17 = "ELF"
    WP_FUENF_1,                                                 // 18 = "FÜNF"
    WP_NEUN_1,                                                  // 19 = "NEUN"
    WP_VIER_1,                                                  // 20 = "VIER"
    WP_ACHT_1,                                                  // 21 = "ACHT"
    WP_NULL_1,                                                  // 22 = "NULL"
    WP_ZWEI_1,                                                  // 23 = "ZWEI"
    WP_ZWOELF_1,                                                // 24 = "ZWÖLF"
    WP_ZEHN_1,                                                  // 25 = "ZEHN"
    WP_UND_1,                                                   // 26 = "UND"
    WP_ZWANZIG_1,                                               // 27 = "ZWANZIG"
    WP_VIERZIG_1,                                               // 28 = "VIERZIG"
    WP_DREISSIG_1,                                              // 29 = "DREISSIG"
    WP_FUENFZIG_1,                                              // 30 = "FÜNFZIG"
    WP_UHR_1,                                                   // 31 = "UHR"
    WP_MINUTE_1,                                                // 32 = "MINUTE"
    WP_MINUTEN_1,                                               // 33 = "MINUTEN"
    WP_VOR_1,                                                   // 34 = "VOR"
    WP_UND_2,                                                   // 35 = "UND"
    WP_NACH_1,                                                  // 36 = "NACH"
    WP_EIN_3,                                                   // 37 = "EIN"
    WP_DREIVIERTEL,                                             // 38 = "DREIVIERTEL"
    WP_VIERTEL_2,                                               // 39 = "VIERTEL"
    WP_HALB,                                                    // 40 = "HALB"
    WP_SIEB_2,                                                  // 41 = "SIEB"
    WP_SIEBEN_2,                                                // 42 = "SIEBEN"
    WP_NEUN_2,                                                  // 43 = "NEUN"
    WP_NULL_2,                                                  // 44 = "NULL"
    WP_ZWEI_2,                                                  // 45 = "ZWEI"
    WP_EIN_4,                                                   // 46 = "EIN"
    WP_EINE_2,                                                  // 47 = "EINE"
    WP_FUENF_2,                                                 // 48 = "FÜNF"
    WP_SECH_2,                                                  // 49 = "SECH"
    WP_SECHS_2,                                                 // 50 = "SECHS"
    WP_NACH_2,                                                  // 51 = "NACH"
    WP_ACHT_2,                                                  // 52 = "ACHT"
    WP_VIER_2,                                                  // 53 = "VIER"
    WP_DREI_2,                                                  // 54 = "DREI"
    WP_EIN_5,                                                   // 55 = "EIN"
    WP_EINS_2,                                                  // 56 = "EINS"
    WP_UND_3,                                                   // 57 = "UND"
    WP_ELF_2,                                                   // 58 = "ELF"
    WP_ZEHN_2,                                                  // 59 = "ZEHN"
    WP_ZWANZIG_2,                                               // 60 = "ZWANZIG"
    WP_GRAD,                                                    // 61 = "GRAD"
    WP_DREISSIG_2,                                              // 62 = "DREISSIG"
    WP_VIERZIG_2,                                               // 63 = "VIERZIG"
    WP_ZWOELF_2,                                                // 64 = "ZWÖLF"
    WP_FUENFZIG_2,                                              // 65 = "FÜNFZIG"
    WP_MINUTE_2,                                                // 66 = "MINUTE"
    WP_MINUTEN_2,                                               // 67 = "MINUTEN"
    WP_UHR_2,                                                   // 68 = "UHR"
    WP_FRUEH,                                                   // 69 = "FRÜH"
    WP_VOR_2,                                                   // 70 = "VOR"
    WP_ABENDS,                                                  // 71 = "ABENDS"
    WP_MITTERNACHT,                                             // 72 = "MITTERNACHT"
    WP_NACH_3,                                                  // 73 = "NACH"
    WP_NACHTS,                                                  // 74 = "NACHTS"
    WP_MORGENS,                                                 // 75 = "MORGENS"
    WP_WARM,                                                    // 76 = "WARM"
    WP_MITTAGS,                                                 // 77 = "MITTAGS"
    WP_COUNT,                                                   // number of words
};

static const DISPLAY_MODES tbl_modes[DISPLAY_MODES_COUNT] =
{
    { HM_4, MM_9,  "hh mm (12-)"        },
    { HM_4, MM_10, "hh mm (12)"         },
    { HM_2, MM_2,  "hh mm (12+)"        },
    { HM_5, MM_9,  "hh mm (24-)"        },
    { HM_5, MM_10, "hh mm (24)"         },
    { HM_3, MM_2,  "hh mm (24+)"        },
    { HM_1, MM_13, "Ossi/Berlin (12-)"  },
    { HM_1, MM_3,  "Ossi/Berlin (12)"   },
    { HM_2, MM_3,  "Ossi/Berlin (12+)"  },
    { HM_1, MM_14, "Oesi (12-)"         },
    { HM_1, MM_4,  "Oesi (12)"          },
    { HM_2, MM_4,  "Oesi (12+)"         },
    { HM_1, MM_15, "Rhein/Ruhr (12-)"   },
    { HM_1, MM_5,  "Rhein/Ruhr (12)"    },
    { HM_2, MM_5,  "Rhein/Ruhr (12+)"   },
    { HM_1, MM_16, "Schwaben (12-)"     },
    { HM_1, MM_6,  "Schwaben (12)"      },
    { HM_2, MM_6,  "Schwaben (12+)"     },
    { HM_1, MM_17, "Wessi (12-)"        },
    { HM_1, MM_7,  "Wessi (12)"         },
    { HM_2, MM_7,  "Wessi (12+)"        },
    { HM_1, MM_18, "Tirol (12-)"        },
    { HM_1, MM_8,  "Tirol (12)"         },
    { HM_2, MM_8,  "Tirol (12+)"        },
    { HM_6, MM_11, "Countdown"          },
    { HM_1, MM_12, "Jester"             },
    { HM_0, MM_19, "Temperatur"         },
};

static const uint8_t tbl_hours[HOUR_MODES_COUNT][HOUR_COUNT][MAX_HOUR_WORDS]=
{
    {   // tbl_hours[0][] = Mode 0 leer, fuer Temperaturanzeige!
        { WP_ES, WP_IST, 0                                                                                              },     // 00:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 01:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 02:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 03:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 04:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 05:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 06:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 07:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 08:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 09:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 10:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 11:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 12:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 13:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 14:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 15:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 16:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 17:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 18:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 19:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 20:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 21:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 22:mm in Mode HM_0
        { WP_ES, WP_IST, 0                                                                                              },     // 23:mm in Mode HM_0
    },
    {   // tbl_hours[1][] = Mode 1 MM - HH (12)
        { WP_ES, WP_IST, WP_ZWOELF_2                                                                                    },     // 00:mm in Mode HM_1
        { WP_ES, WP_IST, WP_IF_MINUTE_IS_0, WP_EIN_4, WP_EINS_2                                                         },     // 01:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ZWEI_2                                                                                      },     // 02:mm in Mode HM_1
        { WP_ES, WP_IST, WP_DREI_2                                                                                      },     // 03:mm in Mode HM_1
        { WP_ES, WP_IST, WP_VIER_2                                                                                      },     // 04:mm in Mode HM_1
        { WP_ES, WP_IST, WP_FUENF_2                                                                                     },     // 05:mm in Mode HM_1
        { WP_ES, WP_IST, WP_SECHS_2                                                                                     },     // 06:mm in Mode HM_1
        { WP_ES, WP_IST, WP_SIEBEN_2                                                                                    },     // 07:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ACHT_2                                                                                      },     // 08:mm in Mode HM_1
        { WP_ES, WP_IST, WP_NEUN_2                                                                                      },     // 09:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ZEHN_2                                                                                      },     // 10:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ELF_2                                                                                       },     // 11:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ZWOELF_2                                                                                    },     // 12:mm in Mode HM_1
        { WP_ES, WP_IST, WP_IF_MINUTE_IS_0, WP_EIN_4, WP_EINS_2                                                         },     // 13:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ZWEI_2                                                                                      },     // 14:mm in Mode HM_1
        { WP_ES, WP_IST, WP_DREI_2                                                                                      },     // 15:mm in Mode HM_1
        { WP_ES, WP_IST, WP_VIER_2                                                                                      },     // 16:mm in Mode HM_1
        { WP_ES, WP_IST, WP_FUENF_2                                                                                     },     // 17:mm in Mode HM_1
        { WP_ES, WP_IST, WP_SECHS_2                                                                                     },     // 18:mm in Mode HM_1
        { WP_ES, WP_IST, WP_SIEBEN_2                                                                                    },     // 19:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ACHT_2                                                                                      },     // 20:mm in Mode HM_1
        { WP_ES, WP_IST, WP_NEUN_2                                                                                      },     // 21:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ZEHN_2                                                                                      },     // 22:mm in Mode HM_1
        { WP_ES, WP_IST, WP_ELF_2                                                                                       },     // 23:mm in Mode HM_1
     },
    {   // tbl_hours[3][] = Mode 3 MM - HH UHR (12) NACHTS
        { WP_ES, WP_IST, WP_ZWOELF_2, WP_UHR_2, WP_NACHTS                                                               },     // 00:mm in Mode HM_2
        { WP_ES, WP_IST, WP_EIN_4, WP_UHR_2, WP_NACHTS                                                                  },     // 01:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ZWEI_2, WP_UHR_2, WP_NACHTS                                                                 },     // 02:mm in Mode HM_2
        { WP_ES, WP_IST, WP_DREI_2, WP_UHR_2, WP_NACHTS                                                                 },     // 03:mm in Mode HM_2
        { WP_ES, WP_IST, WP_VIER_2, WP_UHR_2, WP_FRUEH, WP_MORGENS                                                      },     // 04:mm in Mode HM_2
        { WP_ES, WP_IST, WP_FUENF_2, WP_UHR_2, WP_FRUEH, WP_MORGENS                                                     },     // 05:mm in Mode HM_2
        { WP_ES, WP_IST, WP_SECHS_2, WP_UHR_2, WP_MORGENS                                                               },     // 06:mm in Mode HM_2
        { WP_ES, WP_IST, WP_SIEBEN_2, WP_UHR_2, WP_MORGENS                                                              },     // 07:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ACHT_2, WP_UHR_2, WP_MORGENS                                                                },     // 08:mm in Mode HM_2
        { WP_ES, WP_IST, WP_NEUN_2, WP_UHR_2, WP_MORGENS                                                                },     // 09:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ZEHN_2, WP_UHR_2, WP_VOR_2, WP_MITTAGS                                                      },     // 10:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ELF_2, WP_UHR_2, WP_VOR_2, WP_MITTAGS                                                       },     // 11:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ZWOELF_2, WP_UHR_2, WP_MITTAGS                                                              },     // 12:mm in Mode HM_2
        { WP_ES, WP_IST, WP_EIN_4, WP_UHR_2, WP_MITTAGS                                                                 },     // 13:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ZWEI_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                     },     // 14:mm in Mode HM_2
        { WP_ES, WP_IST, WP_DREI_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                     },     // 15:mm in Mode HM_2
        { WP_ES, WP_IST, WP_VIER_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                     },     // 16:mm in Mode HM_2
        { WP_ES, WP_IST, WP_FUENF_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                    },     // 17:mm in Mode HM_2
        { WP_ES, WP_IST, WP_SECHS_2, WP_UHR_2, WP_ABENDS                                                                },     // 18:mm in Mode HM_2
        { WP_ES, WP_IST, WP_SIEBEN_2, WP_UHR_2, WP_ABENDS                                                               },     // 19:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ACHT_2, WP_UHR_2, WP_ABENDS                                                                 },     // 20:mm in Mode HM_2
        { WP_ES, WP_IST, WP_NEUN_2, WP_UHR_2, WP_ABENDS                                                                 },     // 21:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ZEHN_2, WP_UHR_2, WP_ABENDS                                                                 },     // 22:mm in Mode HM_2
        { WP_ES, WP_IST, WP_ELF_2, WP_UHR_2, WP_NACHTS                                                                  },     // 23:mm in Mode HM_2
     },
    {   // tbl_hours[4][] = Mode 4 MM - HH UHR (24)
        { WP_ES, WP_IST, WP_NULL_2, WP_UHR_2                                                                            },     // 00:mm in Mode HM_3
        { WP_ES, WP_IST, WP_EIN_4, WP_UHR_2                                                                             },     // 01:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ZWEI_2, WP_UHR_2                                                                            },     // 02:mm in Mode HM_3
        { WP_ES, WP_IST, WP_DREI_2, WP_UHR_2                                                                            },     // 03:mm in Mode HM_3
        { WP_ES, WP_IST, WP_VIER_2, WP_UHR_2                                                                            },     // 04:mm in Mode HM_3
        { WP_ES, WP_IST, WP_FUENF_2, WP_UHR_2                                                                           },     // 05:mm in Mode HM_3
        { WP_ES, WP_IST, WP_SECHS_2, WP_UHR_2                                                                           },     // 06:mm in Mode HM_3
        { WP_ES, WP_IST, WP_SIEBEN_2, WP_UHR_2                                                                          },     // 07:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ACHT_2, WP_UHR_2                                                                            },     // 08:mm in Mode HM_3
        { WP_ES, WP_IST, WP_NEUN_2, WP_UHR_2                                                                            },     // 09:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ZEHN_2, WP_UHR_2                                                                            },     // 10:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ELF_2, WP_UHR_2                                                                             },     // 11:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ZWOELF_2, WP_UHR_2                                                                          },     // 12:mm in Mode HM_3
        { WP_ES, WP_IST, WP_DREI_2, WP_ZEHN_2, WP_UHR_2                                                                 },     // 13:mm in Mode HM_3
        { WP_ES, WP_IST, WP_VIER_2, WP_ZEHN_2, WP_UHR_2                                                                 },     // 14:mm in Mode HM_3
        { WP_ES, WP_IST, WP_FUENF_2, WP_ZEHN_2, WP_UHR_2                                                                },     // 15:mm in Mode HM_3
        { WP_ES, WP_IST, WP_SECH_2, WP_ZEHN_2, WP_UHR_2                                                                 },     // 16:mm in Mode HM_3
        { WP_ES, WP_IST, WP_SIEB_2, WP_ZEHN_2, WP_UHR_2                                                                 },     // 17:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ACHT_2, WP_ZEHN_2, WP_UHR_2                                                                 },     // 18:mm in Mode HM_3
        { WP_ES, WP_IST, WP_NEUN_2, WP_ZEHN_2, WP_UHR_2                                                                 },     // 19:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ZWANZIG_2, WP_UHR_2                                                                         },     // 20:mm in Mode HM_3
        { WP_ES, WP_IST, WP_EIN_4, WP_UND_3, WP_ZWANZIG_2, WP_UHR_2                                                     },     // 21:mm in Mode HM_3
        { WP_ES, WP_IST, WP_ZWEI_2, WP_UND_3, WP_ZWANZIG_2, WP_UHR_2                                                    },     // 22:mm in Mode HM_3
        { WP_ES, WP_IST, WP_DREI_2, WP_UND_3, WP_ZWANZIG_2, WP_UHR_2                                                    },     // 23:mm in Mode HM_3
     },
    {   // tbl_hours[5][] = Mode 5 HH UHR (12) - MM
        { WP_ES, WP_IST, WP_NULL_1, WP_UHR_1                                                                            },     // 00:mm in Mode HM_4
        { WP_ES, WP_IST, WP_EIN_1, WP_UHR_1                                                                             },     // 01:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ZWEI_1, WP_UHR_1                                                                            },     // 02:mm in Mode HM_4
        { WP_ES, WP_IST, WP_DREI_1, WP_UHR_1                                                                            },     // 03:mm in Mode HM_4
        { WP_ES, WP_IST, WP_VIER_1, WP_UHR_1                                                                            },     // 04:mm in Mode HM_4
        { WP_ES, WP_IST, WP_FUENF_1, WP_UHR_1                                                                           },     // 05:mm in Mode HM_4
        { WP_ES, WP_IST, WP_SECHS_1, WP_UHR_1                                                                           },     // 06:mm in Mode HM_4
        { WP_ES, WP_IST, WP_SIEBEN_1, WP_UHR_1                                                                          },     // 07:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ACHT_1, WP_UHR_1                                                                            },     // 08:mm in Mode HM_4
        { WP_ES, WP_IST, WP_NEUN_1, WP_UHR_1                                                                            },     // 09:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ZEHN_1, WP_UHR_1                                                                            },     // 10:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ELF_1, WP_UHR_1                                                                             },     // 11:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ZWOELF_1, WP_UHR_1                                                                          },     // 12:mm in Mode HM_4
        { WP_ES, WP_IST, WP_EIN_1, WP_UHR_1                                                                             },     // 13:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ZWEI_1, WP_UHR_1                                                                            },     // 14:mm in Mode HM_4
        { WP_ES, WP_IST, WP_DREI_1, WP_UHR_1                                                                            },     // 15:mm in Mode HM_4
        { WP_ES, WP_IST, WP_VIER_1, WP_UHR_1                                                                            },     // 16:mm in Mode HM_4
        { WP_ES, WP_IST, WP_FUENF_1, WP_UHR_1                                                                           },     // 17:mm in Mode HM_4
        { WP_ES, WP_IST, WP_SECHS_1, WP_UHR_1                                                                           },     // 18:mm in Mode HM_4
        { WP_ES, WP_IST, WP_SIEBEN_1, WP_UHR_1                                                                          },     // 19:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ACHT_1, WP_UHR_1                                                                            },     // 20:mm in Mode HM_4
        { WP_ES, WP_IST, WP_NEUN_1, WP_UHR_1                                                                            },     // 21:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ZEHN_1, WP_UHR_1                                                                            },     // 22:mm in Mode HM_4
        { WP_ES, WP_IST, WP_ELF_1, WP_UHR_1                                                                             },     // 23:mm in Mode HM_4
     },
    {   // tbl_hours[6][] = Mode 6 HH UHR 24) - MM
        { WP_ES, WP_IST, WP_NULL_1, WP_UHR_1                                                                            },     // 00:mm in Mode HM_5
        { WP_ES, WP_IST, WP_EIN_1, WP_UHR_1                                                                             },     // 01:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ZWEI_1, WP_UHR_1                                                                            },     // 02:mm in Mode HM_5
        { WP_ES, WP_IST, WP_DREI_1, WP_UHR_1                                                                            },     // 03:mm in Mode HM_5
        { WP_ES, WP_IST, WP_VIER_1, WP_UHR_1                                                                            },     // 04:mm in Mode HM_5
        { WP_ES, WP_IST, WP_FUENF_1, WP_UHR_1                                                                           },     // 05:mm in Mode HM_5
        { WP_ES, WP_IST, WP_SECHS_1, WP_UHR_1                                                                           },     // 06:mm in Mode HM_5
        { WP_ES, WP_IST, WP_SIEBEN_1, WP_UHR_1                                                                          },     // 07:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ACHT_1, WP_UHR_1                                                                            },     // 08:mm in Mode HM_5
        { WP_ES, WP_IST, WP_NEUN_1, WP_UHR_1                                                                            },     // 09:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ZEHN_1, WP_UHR_1                                                                            },     // 10:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ELF_1, WP_UHR_1                                                                             },     // 11:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ZWOELF_1, WP_UHR_1                                                                          },     // 12:mm in Mode HM_5
        { WP_ES, WP_IST, WP_DREI_1, WP_ZEHN_1, WP_UHR_1                                                                 },     // 13:mm in Mode HM_5
        { WP_ES, WP_IST, WP_VIER_1, WP_ZEHN_1, WP_UHR_1                                                                 },     // 14:mm in Mode HM_5
        { WP_ES, WP_IST, WP_FUENF_1, WP_ZEHN_1, WP_UHR_1                                                                },     // 15:mm in Mode HM_5
        { WP_ES, WP_IST, WP_SECH_1, WP_ZEHN_1, WP_UHR_1                                                                 },     // 16:mm in Mode HM_5
        { WP_ES, WP_IST, WP_SIEB_1, WP_ZEHN_1, WP_UHR_1                                                                 },     // 17:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ACHT_1, WP_ZEHN_1, WP_UHR_1                                                                 },     // 18:mm in Mode HM_5
        { WP_ES, WP_IST, WP_NEUN_1, WP_ZEHN_1, WP_UHR_1                                                                 },     // 19:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ZWANZIG_1, WP_UHR_1                                                                         },     // 20:mm in Mode HM_5
        { WP_ES, WP_IST, WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_UHR_1                                                     },     // 21:mm in Mode HM_5
        { WP_ES, WP_IST, WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_UHR_1                                                    },     // 22:mm in Mode HM_5
        { WP_ES, WP_IST, WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_UHR_1                                                    },     // 23:mm in Mode HM_5
     },
    {   // tbl_hours[7][] = Mode 7 MITTERNACHT (0 UHR)
        { WP_ES, WP_IST, WP_MITTERNACHT                                                                                 },     // 00:mm in Mode HM_6
        { WP_ES, WP_IST, WP_EIN_4, WP_UHR_2, WP_FRUEH                                                                   },     // 01:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ZWEI_2, WP_UHR_2, WP_FRUEH                                                                  },     // 02:mm in Mode HM_6
        { WP_ES, WP_IST, WP_DREI_2, WP_UHR_2, WP_FRUEH                                                                  },     // 03:mm in Mode HM_6
        { WP_ES, WP_IST, WP_VIER_2, WP_UHR_2, WP_FRUEH                                                                  },     // 04:mm in Mode HM_6
        { WP_ES, WP_IST, WP_FUENF_2, WP_UHR_2, WP_FRUEH                                                                 },     // 05:mm in Mode HM_6
        { WP_ES, WP_IST, WP_SECHS_2, WP_UHR_2, WP_FRUEH                                                                 },     // 06:mm in Mode HM_6
        { WP_ES, WP_IST, WP_SIEBEN_2, WP_UHR_2, WP_MORGENS                                                              },     // 07:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ACHT_2, WP_UHR_2, WP_MORGENS                                                                },     // 08:mm in Mode HM_6
        { WP_ES, WP_IST, WP_NEUN_2, WP_UHR_2, WP_MORGENS                                                                },     // 09:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ZEHN_2, WP_UHR_2, WP_MORGENS                                                                },     // 10:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ELF_2, WP_UHR_2, WP_MORGENS                                                                 },     // 11:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ZWOELF_2, WP_UHR_2, WP_MITTAGS                                                              },     // 12:mm in Mode HM_6
        { WP_ES, WP_IST, WP_EIN_4, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                      },     // 13:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ZWEI_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                     },     // 14:mm in Mode HM_6
        { WP_ES, WP_IST, WP_DREI_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                     },     // 15:mm in Mode HM_6
        { WP_ES, WP_IST, WP_VIER_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                     },     // 16:mm in Mode HM_6
        { WP_ES, WP_IST, WP_FUENF_2, WP_UHR_2, WP_NACH_3, WP_MITTAGS                                                    },     // 17:mm in Mode HM_6
        { WP_ES, WP_IST, WP_SECHS_2, WP_UHR_2, WP_ABENDS                                                                },     // 18:mm in Mode HM_6
        { WP_ES, WP_IST, WP_SIEBEN_2, WP_UHR_2, WP_ABENDS                                                               },     // 19:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ACHT_2, WP_UHR_2, WP_ABENDS                                                                 },     // 20:mm in Mode HM_6
        { WP_ES, WP_IST, WP_NEUN_2, WP_UHR_2, WP_ABENDS                                                                 },     // 21:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ZEHN_2, WP_UHR_2, WP_NACHTS                                                                 },     // 22:mm in Mode HM_6
        { WP_ES, WP_IST, WP_ELF_2, WP_UHR_2, WP_NACHTS                                                                  },     // 23:mm in Mode HM_6
    }
};

static const MINUTEDISPLAY tbl_minutes[MINUTE_MODES_COUNT][MINUTE_COUNT]=
{
    {   // tbl_minutes[0][] = Mode 1 MM NACH
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, {0                                                                           }},    // hh:00 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINS_1, WP_NACH_1                                                       }},    // hh:01 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_ZEHN_1, WP_NACH_1                                           }},    // hh:15 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_NACH_1                                                    }},    // hh:20 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                                }},    // hh:21 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                               }},    // hh:22 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                               }},    // hh:23 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                               }},    // hh:24 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                              }},    // hh:25 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                              }},    // hh:26 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                             }},    // hh:27 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                               }},    // hh:28 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_ZWANZIG_1, WP_NACH_1                               }},    // hh:29 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREISSIG_1, WP_NACH_1                                                   }},    // hh:30 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                               }},    // hh:31 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                              }},    // hh:32 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                              }},    // hh:33 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                              }},    // hh:34 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                             }},    // hh:35 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                             }},    // hh:36 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                            }},    // hh:37 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                              }},    // hh:38 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_DREISSIG_1, WP_NACH_1                              }},    // hh:39 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERZIG_1, WP_NACH_1                                                    }},    // hh:40 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                                }},    // hh:41 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                               }},    // hh:42 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                               }},    // hh:43 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                               }},    // hh:44 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                              }},    // hh:45 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                              }},    // hh:46 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                             }},    // hh:47 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                               }},    // hh:48 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                               }},    // hh:49 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENFZIG_1, WP_NACH_1                                                   }},    // hh:50 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                               }},    // hh:51 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                              }},    // hh:52 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                              }},    // hh:53 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                              }},    // hh:54 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                             }},    // hh:55 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                             }},    // hh:56 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                            }},    // hh:57 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                              }},    // hh:58 in Mode MM_1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_FUENFZIG_1, WP_NACH_1                              }},    // hh:59 in Mode MM_1
     },
    {   // tbl_minutes[1][] = Mode 2 MM MINUTEN NACH
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, {0                                                                           }},    // hh:00 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                             }},    // hh:15 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                                      }},    // hh:20 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                  }},    // hh:21 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:22 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:23 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:24 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:25 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:26 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:27 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:28 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:29 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                                     }},    // hh:30 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:31 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:32 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:33 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:34 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:35 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:36 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1              }},    // hh:37 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:38 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:39 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                                      }},    // hh:40 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                  }},    // hh:41 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:42 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:43 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:44 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:45 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:46 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:47 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:48 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:49 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                                     }},    // hh:50 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                 }},    // hh:51 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:52 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:53 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:54 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:55 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:56 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1              }},    // hh:57 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:58 in Mode MM_2
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:59 in Mode MM_2
    },
    {   // tbl_minutes[2][] = Mode 3 OSSI - MM MINUTEN NACH (VIERTEL NACH, HALB, VIERTEL VOR)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1                                                               }},    // hh:15 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:20 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:21 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:22 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:23 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:24 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:25 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:26 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:27 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:28 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:32 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:33 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:34 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:35 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:37 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:38 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:39 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:40 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                 }},    // hh:41 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:42 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:43 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:44 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_3
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_3
    },
    {   // tbl_minutes[3][] = Mode 4 OESI - MINUTEN NACH (VIERTEL NACH, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                                      }},    // hh:20 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:21 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:22 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:23 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:24 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:25 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:26 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:27 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:28 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:32 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:33 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:34 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:35 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:37 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:38 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:39 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                                       }},    // hh:40 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:41 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:42 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:43 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:44 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_4
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_4
    },
    {   // tbl_minutes[4][] = Mode 5 RHEIN/ RUHR - MINUTEN NACH (VIERTEL, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                                      }},    // hh:20 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:21 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:22 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:23 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:24 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:25 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:26 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:27 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:28 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:32 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:33 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:34 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:35 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:37 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:38 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:39 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                                       }},    // hh:40 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:41 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:42 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:43 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:44 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1, WP_VOR_1                                                     }},    // hh:45 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_5
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_5
    },
    {   // tbl_minutes[5][] = Mode 6 SCHWABEN - MM MINUTEN NACH (VIERTEL NACH, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1                                                               }},    // hh:15 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                                      }},    // hh:20 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:21 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:22 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:23 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:24 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:25 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:26 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:27 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:28 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:32 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:33 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:34 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:35 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:37 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:38 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:39 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                                       }},    // hh:40 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:41 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:42 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:43 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:44 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_6
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_6
    },
    {   // tbl_minutes[6][] = Mode 7 WESSI - MM MINUTEN NACH (VIERTEL, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:20 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:21 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:22 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:23 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:24 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:25 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:26 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:27 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:28 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:32 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:33 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:34 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:35 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:37 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:38 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:39 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:40 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                 }},    // hh:41 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:42 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:43 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:44 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1, WP_VOR_1                                                     }},    // hh:45 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_7
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_7
    },
    {   // tbl_minutes[7][] = Mode 8 TIROL
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:02 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:04 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:05 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1                                        }},    // hh:06 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:07 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:08 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:09 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:10 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_MINUTEN_1, WP_NACH_1                                          }},    // hh:11 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_MINUTEN_1, WP_NACH_1                                       }},    // hh:12 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:13 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:14 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:16 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:17 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:18 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1                              }},    // hh:19 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1                                      }},    // hh:20 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:21 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:22 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:23 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:24 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                }},    // hh:25 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:26 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:27 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                                 }},    // hh:28 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:32 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:33 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:34 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:35 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                              }},    // hh:37 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:38 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:39 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                                       }},    // hh:40 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:41 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:42 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:43 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:44 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_8
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_8
    },
    {   // tbl_minutes[8][] = Mode 9 MM
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, {0                                                                           }},    // hh:00 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINS_2                                                                  }},    // hh:01 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_2                                                                  }},    // hh:02 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_2                                                                  }},    // hh:03 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_2                                                                  }},    // hh:04 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_2                                                                 }},    // hh:05 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_2                                                                 }},    // hh:06 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_2                                                                }},    // hh:07 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_2                                                                  }},    // hh:08 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_2                                                                  }},    // hh:09 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_2                                                                  }},    // hh:10 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_2                                                                   }},    // hh:11 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_2                                                                }},    // hh:12 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_2, WP_ZEHN_2                                                       }},    // hh:13 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_2, WP_ZEHN_2                                                       }},    // hh:14 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_2, WP_ZEHN_2                                                      }},    // hh:15 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_2, WP_ZEHN_2                                                       }},    // hh:16 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_2, WP_ZEHN_2                                                       }},    // hh:17 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_2, WP_ZEHN_2                                                       }},    // hh:18 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_2, WP_ZEHN_2                                                       }},    // hh:19 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_2                                                               }},    // hh:20 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_4, WP_UND_3, WP_ZWANZIG_2                                           }},    // hh:21 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_2, WP_UND_3, WP_ZWANZIG_2                                          }},    // hh:22 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_2, WP_UND_3, WP_ZWANZIG_2                                          }},    // hh:23 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_2, WP_UND_3, WP_ZWANZIG_2                                          }},    // hh:24 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_2, WP_UND_3, WP_ZWANZIG_2                                         }},    // hh:25 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_2, WP_UND_3, WP_ZWANZIG_2                                         }},    // hh:26 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_2, WP_UND_3, WP_ZWANZIG_2                                        }},    // hh:27 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_2, WP_UND_3, WP_ZWANZIG_2                                          }},    // hh:28 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_2, WP_UND_3, WP_ZWANZIG_2                                          }},    // hh:29 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREISSIG_2                                                              }},    // hh:30 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_4, WP_UND_3, WP_DREISSIG_2                                          }},    // hh:31 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_2, WP_UND_3, WP_DREISSIG_2                                         }},    // hh:32 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_2, WP_UND_3, WP_DREISSIG_2                                         }},    // hh:33 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_2, WP_UND_3, WP_DREISSIG_2                                         }},    // hh:34 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_2, WP_UND_3, WP_DREISSIG_2                                        }},    // hh:35 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_2, WP_UND_3, WP_DREISSIG_2                                        }},    // hh:36 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_2, WP_UND_3, WP_DREISSIG_2                                       }},    // hh:37 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_2, WP_UND_3, WP_DREISSIG_2                                         }},    // hh:38 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_2, WP_UND_3, WP_DREISSIG_2                                         }},    // hh:39 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERZIG_2                                                               }},    // hh:40 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_4, WP_UND_3, WP_VIERZIG_2                                           }},    // hh:41 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_2, WP_UND_3, WP_VIERZIG_2                                          }},    // hh:42 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_2, WP_UND_3, WP_VIERZIG_2                                          }},    // hh:43 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_2, WP_UND_3, WP_VIERZIG_2                                          }},    // hh:44 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_2, WP_UND_3, WP_VIERZIG_2                                         }},    // hh:45 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_2, WP_UND_3, WP_VIERZIG_2                                         }},    // hh:46 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_2, WP_UND_3, WP_VIERZIG_2                                        }},    // hh:47 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_2, WP_UND_3, WP_VIERZIG_2                                          }},    // hh:48 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_2, WP_UND_3, WP_VIERZIG_2                                          }},    // hh:49 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENFZIG_2                                                              }},    // hh:50 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_4, WP_UND_3, WP_FUENFZIG_2                                          }},    // hh:51 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_2, WP_UND_3, WP_FUENFZIG_2                                         }},    // hh:52 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_2, WP_UND_3, WP_FUENFZIG_2                                         }},    // hh:53 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_2, WP_UND_3, WP_FUENFZIG_2                                         }},    // hh:54 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_2, WP_UND_3, WP_FUENFZIG_2                                        }},    // hh:55 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_2, WP_UND_3, WP_FUENFZIG_2                                        }},    // hh:56 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_2, WP_UND_3, WP_FUENFZIG_2                                       }},    // hh:57 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_2, WP_UND_3, WP_FUENFZIG_2                                         }},    // hh:58 in Mode MM_9
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_2, WP_UND_3, WP_FUENFZIG_2                                         }},    // hh:59 in Mode MM_9
    },
    {   // tbl_minutes[9][] = Mode 10 UND MM MINUTEN
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, {0                                                                           }},    // hh:00 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_EINE_2, WP_MINUTE_2                                           }},    // hh:01 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWEI_2, WP_MINUTEN_2                                          }},    // hh:02 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREI_2, WP_MINUTEN_2                                          }},    // hh:03 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIER_2, WP_MINUTEN_2                                          }},    // hh:04 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENF_2, WP_MINUTEN_2                                         }},    // hh:05 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SECHS_2, WP_MINUTEN_2                                         }},    // hh:06 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SIEBEN_2, WP_MINUTEN_2                                        }},    // hh:07 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ACHT_2, WP_MINUTEN_2                                          }},    // hh:08 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_NEUN_2, WP_MINUTEN_2                                          }},    // hh:09 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZEHN_2, WP_MINUTEN_2                                          }},    // hh:10 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ELF_2, WP_MINUTEN_2                                           }},    // hh:11 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWOELF_2, WP_MINUTEN_2                                        }},    // hh:12 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREI_2, WP_ZEHN_2, WP_MINUTEN_2                               }},    // hh:13 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIER_2, WP_ZEHN_2, WP_MINUTEN_2                               }},    // hh:14 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENF_2, WP_ZEHN_2, WP_MINUTEN_2                              }},    // hh:15 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SECH_2, WP_ZEHN_2, WP_MINUTEN_2                               }},    // hh:16 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SIEB_2, WP_ZEHN_2, WP_MINUTEN_2                               }},    // hh:17 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ACHT_2, WP_ZEHN_2, WP_MINUTEN_2                               }},    // hh:18 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_NEUN_2, WP_ZEHN_2, WP_MINUTEN_2                               }},    // hh:19 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWANZIG_2, WP_MINUTEN_2                                       }},    // hh:20 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_EIN_4, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                   }},    // hh:21 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWEI_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                  }},    // hh:22 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREI_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                  }},    // hh:23 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIER_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                  }},    // hh:24 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENF_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                 }},    // hh:25 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SECHS_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                 }},    // hh:26 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SIEBEN_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                }},    // hh:27 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ACHT_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                  }},    // hh:28 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_NEUN_2, WP_UND_3, WP_ZWANZIG_2, WP_MINUTEN_2                  }},    // hh:29 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREISSIG_2, WP_MINUTEN_2                                      }},    // hh:30 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_EIN_4, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                  }},    // hh:31 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWEI_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                 }},    // hh:32 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREI_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                 }},    // hh:33 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIER_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                 }},    // hh:34 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENF_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                }},    // hh:35 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SECHS_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                }},    // hh:36 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SIEBEN_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2               }},    // hh:37 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ACHT_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                 }},    // hh:38 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_NEUN_2, WP_UND_3, WP_DREISSIG_2, WP_MINUTEN_2                 }},    // hh:39 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIERZIG_2, WP_MINUTEN_2                                       }},    // hh:40 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_EIN_4, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                   }},    // hh:41 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWEI_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                  }},    // hh:42 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREI_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                  }},    // hh:43 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIER_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                  }},    // hh:44 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENF_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                 }},    // hh:45 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SECHS_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                 }},    // hh:46 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SIEBEN_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                }},    // hh:47 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ACHT_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                  }},    // hh:48 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_NEUN_2, WP_UND_3, WP_VIERZIG_2, WP_MINUTEN_2                  }},    // hh:49 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENFZIG_2, WP_MINUTEN_2                                      }},    // hh:50 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_EIN_4, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                  }},    // hh:51 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ZWEI_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                 }},    // hh:52 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_DREI_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                 }},    // hh:53 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_VIER_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                 }},    // hh:54 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_FUENF_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                }},    // hh:55 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SECHS_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                }},    // hh:56 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_SIEBEN_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2               }},    // hh:57 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_ACHT_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                 }},    // hh:58 in Mode MM_10
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UND_2, WP_NEUN_2, WP_UND_3, WP_FUENFZIG_2, WP_MINUTEN_2                 }},    // hh:59 in Mode MM_10
    },
    {   // tbl_minutes[10][] = Mode 11 MM MINUTEN VOR
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, {0                                                                           }},    // hh:00 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:01 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:02 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1               }},    // hh:03 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:04 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:05 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:06 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:07 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:08 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EIN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:09 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                                      }},    // hh:10 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:11 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:12 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:13 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:14 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:15 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:16 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:17 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:18 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EIN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_VOR_1                   }},    // hh:19 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                                       }},    // hh:20 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:21 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:22 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1               }},    // hh:23 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:24 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:25 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:26 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:27 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:28 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EIN_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:29 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                                      }},    // hh:30 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:31 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:32 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:33 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:34 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:35 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:36 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:37 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:38 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                   }},    // hh:39 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                                       }},    // hh:40 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:41 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:42 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:43 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                              }},    // hh:44 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                              }},    // hh:45 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:46 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                               }},    // hh:47 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:48 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_MINUTEN_1, WP_VOR_1                                           }},    // hh:49 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:50 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:51 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:52 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_MINUTEN_1, WP_VOR_1                                        }},    // hh:53 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:55 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:56 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:57 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_11
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_11
    },

    {   // tbl_minutes[11][] = Mode 12 Jester1
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NULL_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:00 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1, WP_DREIVIERTEL                             }},    // hh:01 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_UND_1, WP_ZWANZIG_1, WP_VOR_1, WP_HALB                       }},    // hh:02 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1                                         }},    // hh:03 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:04 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:05 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:06 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:07 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:08 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EIN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:09 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_VOR_1, WP_HALB                                            }},    // hh:10 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_MINUTEN_1, WP_VOR_1, WP_VIERTEL_2                            }},    // hh:11 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                      }},    // hh:12 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1, WP_VIERTEL_2                            }},    // hh:13 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_VIERTEL_2                             }},    // hh:14 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1, WP_VOR_1, WP_HALB                                            }},    // hh:15 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1, WP_HALB                                    }},    // hh:16 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_VIERTEL_2                                         }},    // hh:17 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_MINUTEN_1, WP_VOR_1, WP_HALB                               }},    // hh:18 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_VIERTEL_2                                         }},    // hh:19 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERZIG_1, WP_VOR_1                                                     }},    // hh:20 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_VIERTEL_2                                        }},    // hh:21 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_VOR_1                               }},    // hh:22 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_VIERTEL_2                                         }},    // hh:23 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_VOR_1, WP_DREIVIERTEL                 }},    // hh:24 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                }},    // hh:25 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:26 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_VOR_1                 }},    // hh:27 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_VIERTEL_2                }},    // hh:28 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NULL_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                                }},    // hh:30 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_UND_1, WP_ZWANZIG_1, WP_VOR_1, WP_MINUTEN_1                  }},    // hh:31 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1, WP_VIERTEL_2                              }},    // hh:32 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_UND_1, WP_ZWANZIG_1, WP_VOR_1                              }},    // hh:33 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_VOR_1, WP_MINUTEN_1                 }},    // hh:35 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                               }},    // hh:36 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_VOR_1                  }},    // hh:37 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:38 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1, WP_VIERTEL_2  }},    // hh:40 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_VIERZIG_1, WP_NACH_1                                }},    // hh:41 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_DREIVIERTEL                                        }},    // hh:42 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:43 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:44 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1, WP_NACH_1, WP_HALB                                           }},    // hh:45 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:46 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_VIERZIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:47 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_MINUTEN_1, WP_NACH_1, WP_DREIVIERTEL                         }},    // hh:48 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB                     }},    // hh:49 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_NACH_1, WP_HALB                                           }},    // hh:50 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_NACH_1, WP_DREIVIERTEL                        }},    // hh:51 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_MINUTEN_1, WP_NACH_1, WP_HALB        }},    // hh:52 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:53 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_MINUTEN_1, WP_VOR_1                                         }},    // hh:54 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_NACH_1, WP_DREIVIERTEL                                       }},    // hh:55 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1               }},    // hh:56 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1              }},    // hh:57 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_MINUTEN_1, WP_VOR_1                                          }},    // hh:58 in Mode MM_12
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_FUENFZIG_1, WP_MINUTEN_1, WP_NACH_1                }},    // hh:59 in Mode MM_12
    },
    {   // tbl_minutes[12][] = Mode 13 OSSI - MM NACH (VIERTEL NACH, HALB, VIERTEL VOR)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1                                                               }},    // hh:15 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1, WP_HALB                                               }},    // hh:20 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1, WP_HALB                                               }},    // hh:21 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1, WP_HALB                                               }},    // hh:22 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1, WP_HALB                                             }},    // hh:23 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1, WP_HALB                                              }},    // hh:24 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1, WP_HALB                                              }},    // hh:25 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1, WP_HALB                                               }},    // hh:26 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_HALB                                               }},    // hh:27 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1, WP_HALB                                               }},    // hh:28 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_HALB                                              }},    // hh:32 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_NACH_1, WP_HALB                                              }},    // hh:33 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_NACH_1, WP_HALB                                             }},    // hh:35 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_HALB                                             }},    // hh:36 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_NACH_1, WP_HALB                                            }},    // hh:37 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_HALB                                              }},    // hh:38 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_NACH_1, WP_HALB                                              }},    // hh:40 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_NACH_1, WP_HALB                                               }},    // hh:41 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_NACH_1, WP_HALB                                            }},    // hh:42 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1, WP_HALB                                   }},    // hh:43 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1, WP_HALB                                   }},    // hh:44 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:46 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:47 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_VOR_1                                                      }},    // hh:48 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_VOR_1                                                         }},    // hh:49 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1                                                        }},    // hh:50 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1                                                        }},    // hh:51 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1                                                        }},    // hh:52 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1                                                      }},    // hh:53 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1                                                       }},    // hh:54 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1                                                       }},    // hh:55 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1                                                        }},    // hh:56 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1                                                        }},    // hh:57 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1                                                        }},    // hh:58 in Mode MM_13
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_13
    },
    {   // tbl_minutes[13][] = Mode 14 OESI - NACH (VIERTEL NACH, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_NACH_1                                                    }},    // hh:20 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1, WP_HALB                                               }},    // hh:21 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1, WP_HALB                                               }},    // hh:22 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1, WP_HALB                                             }},    // hh:23 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1, WP_HALB                                              }},    // hh:24 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1, WP_HALB                                              }},    // hh:25 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1, WP_HALB                                               }},    // hh:26 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_HALB                                               }},    // hh:27 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1, WP_HALB                                               }},    // hh:28 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_HALB                                              }},    // hh:32 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_NACH_1, WP_HALB                                              }},    // hh:33 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_NACH_1, WP_HALB                                             }},    // hh:35 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_HALB                                             }},    // hh:36 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_NACH_1, WP_HALB                                            }},    // hh:37 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_HALB                                              }},    // hh:38 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_VOR_1                                                     }},    // hh:40 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:41 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:42 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:43 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:44 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:46 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:47 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_VOR_1                                                      }},    // hh:48 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_VOR_1                                                         }},    // hh:49 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1                                                        }},    // hh:50 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1                                                        }},    // hh:51 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1                                                        }},    // hh:52 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1                                                      }},    // hh:53 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1                                                       }},    // hh:54 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1                                                       }},    // hh:55 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1                                                        }},    // hh:56 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1                                                        }},    // hh:57 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1                                                        }},    // hh:58 in Mode MM_14
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_14
    },
    {   // tbl_minutes[14][] = Mode 15 RHEIN/ RUHR - NACH (VIERTEL, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_NACH_1                                                    }},    // hh:20 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1, WP_HALB                                               }},    // hh:21 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1, WP_HALB                                               }},    // hh:22 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1, WP_HALB                                             }},    // hh:23 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1, WP_HALB                                              }},    // hh:24 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1, WP_HALB                                              }},    // hh:25 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1, WP_HALB                                               }},    // hh:26 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_HALB                                               }},    // hh:27 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1, WP_HALB                                               }},    // hh:28 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_HALB                                              }},    // hh:32 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_NACH_1, WP_HALB                                              }},    // hh:33 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_NACH_1, WP_HALB                                             }},    // hh:35 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_HALB                                             }},    // hh:36 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_NACH_1, WP_HALB                                            }},    // hh:37 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_HALB                                              }},    // hh:38 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_VOR_1                                                     }},    // hh:40 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:41 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:42 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:43 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:44 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1, WP_VOR_1                                                     }},    // hh:45 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:46 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:47 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_VOR_1                                                      }},    // hh:48 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_VOR_1                                                         }},    // hh:49 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1                                                        }},    // hh:50 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1                                                        }},    // hh:51 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1                                                        }},    // hh:52 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1                                                      }},    // hh:53 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1                                                       }},    // hh:54 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1                                                       }},    // hh:55 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1                                                        }},    // hh:56 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1                                                        }},    // hh:57 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1                                                        }},    // hh:58 in Mode MM_15
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_15
    },
    {   // tbl_minutes[15][] = Mode 16 SCHWABEN - MM MINACH (VIERTEL NACH, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1                                                               }},    // hh:15 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_NACH_1                                                    }},    // hh:20 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1, WP_HALB                                               }},    // hh:21 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1, WP_HALB                                               }},    // hh:22 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1, WP_HALB                                             }},    // hh:23 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1, WP_HALB                                              }},    // hh:24 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1, WP_HALB                                              }},    // hh:25 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1, WP_HALB                                               }},    // hh:26 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_HALB                                               }},    // hh:27 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1, WP_HALB                                               }},    // hh:28 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_HALB                                              }},    // hh:32 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_NACH_1, WP_HALB                                              }},    // hh:33 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_NACH_1, WP_HALB                                             }},    // hh:35 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_HALB                                             }},    // hh:36 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_NACH_1, WP_HALB                                            }},    // hh:37 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_HALB                                              }},    // hh:38 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_VOR_1                                                     }},    // hh:40 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:41 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:42 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:43 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:44 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:46 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:47 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_VOR_1                                                      }},    // hh:48 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_VOR_1                                                         }},    // hh:49 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1                                                        }},    // hh:50 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1                                                        }},    // hh:51 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1                                                        }},    // hh:52 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1                                                      }},    // hh:53 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1                                                       }},    // hh:54 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1                                                       }},    // hh:55 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1                                                        }},    // hh:56 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1                                                        }},    // hh:57 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1                                                        }},    // hh:58 in Mode MM_16
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_16
    },
    {   // tbl_minutes[16][] = Mode 17 WESSI - MM NACH (VIERTEL, HALB, DREIVIERTEL)
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1, WP_HALB                                               }},    // hh:20 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1, WP_HALB                                               }},    // hh:21 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1, WP_HALB                                               }},    // hh:22 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1, WP_HALB                                             }},    // hh:23 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1, WP_HALB                                              }},    // hh:24 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1, WP_HALB                                              }},    // hh:25 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1, WP_HALB                                               }},    // hh:26 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_HALB                                               }},    // hh:27 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1, WP_HALB                                               }},    // hh:28 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_HALB                                              }},    // hh:32 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_NACH_1, WP_HALB                                              }},    // hh:33 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_NACH_1, WP_HALB                                             }},    // hh:35 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_HALB                                             }},    // hh:36 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_NACH_1, WP_HALB                                            }},    // hh:37 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_HALB                                              }},    // hh:38 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_NACH_1, WP_HALB                                              }},    // hh:40 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_NACH_1, WP_HALB                                               }},    // hh:41 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_NACH_1, WP_HALB                                            }},    // hh:42 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1, WP_HALB                                   }},    // hh:43 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1, WP_HALB                                   }},    // hh:44 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIERTEL_1, WP_VOR_1                                                     }},    // hh:45 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:46 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:47 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_VOR_1                                                      }},    // hh:48 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_VOR_1                                                         }},    // hh:49 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1                                                        }},    // hh:50 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1                                                        }},    // hh:51 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1                                                        }},    // hh:52 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1                                                      }},    // hh:53 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1                                                       }},    // hh:54 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1                                                       }},    // hh:55 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1                                                        }},    // hh:56 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1                                                        }},    // hh:57 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1                                                        }},    // hh:58 in Mode MM_17
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_17
    },
    {   // tbl_minutes[17][] = Mode 18 TIROL
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_UHR_2                                                                   }},    // hh:00 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1                                          }},    // hh:01 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_NACH_1                                                       }},    // hh:02 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_NACH_1                                                       }},    // hh:03 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_NACH_1                                                       }},    // hh:04 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_NACH_1                                                      }},    // hh:05 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_NACH_1                                                      }},    // hh:06 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_NACH_1                                                     }},    // hh:07 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_NACH_1                                                       }},    // hh:08 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_NACH_1                                                       }},    // hh:09 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_NACH_1                                                       }},    // hh:10 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_NACH_1                                                        }},    // hh:11 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_NACH_1                                                     }},    // hh:12 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:13 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:14 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIERTEL_1, WP_NACH_1                                                    }},    // hh:15 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:16 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:17 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:18 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_NACH_1                                            }},    // hh:19 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_NACH_1                                                    }},    // hh:20 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1, WP_HALB                                               }},    // hh:21 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1, WP_HALB                                               }},    // hh:22 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1, WP_HALB                                             }},    // hh:23 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1, WP_HALB                                              }},    // hh:24 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1, WP_HALB                                              }},    // hh:25 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1, WP_HALB                                               }},    // hh:26 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1, WP_HALB                                               }},    // hh:27 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1, WP_HALB                                               }},    // hh:28 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1, WP_HALB                                  }},    // hh:29 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_HALB                                                                    }},    // hh:30 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_NACH_1, WP_HALB                                 }},    // hh:31 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_NACH_1, WP_HALB                                              }},    // hh:32 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_NACH_1, WP_HALB                                              }},    // hh:33 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_NACH_1, WP_HALB                                              }},    // hh:34 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_NACH_1, WP_HALB                                             }},    // hh:35 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_NACH_1, WP_HALB                                             }},    // hh:36 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_NACH_1, WP_HALB                                            }},    // hh:37 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_NACH_1, WP_HALB                                              }},    // hh:38 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_NACH_1, WP_HALB                                              }},    // hh:39 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWANZIG_1, WP_VOR_1                                                     }},    // hh:40 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:41 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:42 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEB_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:43 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECH_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:44 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREIVIERTEL                                                             }},    // hh:45 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:46 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_ZEHN_1, WP_VOR_1                                             }},    // hh:47 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWOELF_1, WP_VOR_1                                                      }},    // hh:48 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ELF_1, WP_VOR_1                                                         }},    // hh:49 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZEHN_1, WP_VOR_1                                                        }},    // hh:50 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_NEUN_1, WP_VOR_1                                                        }},    // hh:51 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ACHT_1, WP_VOR_1                                                        }},    // hh:52 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SIEBEN_1, WP_VOR_1                                                      }},    // hh:53 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_SECHS_1, WP_VOR_1                                                       }},    // hh:54 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_FUENF_1, WP_VOR_1                                                       }},    // hh:55 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_VIER_1, WP_VOR_1                                                        }},    // hh:56 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_DREI_1, WP_VOR_1                                                        }},    // hh:57 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_ZWEI_1, WP_VOR_1                                                        }},    // hh:58 in Mode MM_18
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_1, { WP_EINE_1, WP_MINUTE_1, WP_VOR_1                                           }},    // hh:59 in Mode MM_18
    },

    {   // tbl_minutes[18][] = Mode 19 TEMPERATUR "CC GRAD"
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_GRAD                                                         }},    // hh:00 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                                      }},    // hh:01 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_GRAD                                                          }},    // hh:02 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ELF_1, WP_EIN_3, WP_HALB, WP_GRAD                                       }},    // hh:03 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_GRAD                                                       }},    // hh:04 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWOELF_1, WP_EIN_3, WP_HALB, WP_GRAD                                    }},    // hh:05 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_GRAD                                              }},    // hh:06 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                           }},    // hh:07 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_GRAD                                              }},    // hh:08 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                           }},    // hh:09 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_ZEHN_1, WP_GRAD                                             }},    // hh:10 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                          }},    // hh:11 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_GRAD                                              }},    // hh:12 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECH_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                           }},    // hh:13 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_GRAD                                              }},    // hh:14 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEB_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                           }},    // hh:15 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_GRAD                                              }},    // hh:16 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                           }},    // hh:17 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_GRAD                                              }},    // hh:18 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_ZEHN_1, WP_EIN_3, WP_HALB, WP_GRAD                           }},    // hh:19 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_GRAD                                                      }},    // hh:20 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD                                   }},    // hh:21 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD                                  }},    // hh:22 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD               }},    // hh:23 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                        }},    // hh:24 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM     }},    // hh:25 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                        }},    // hh:26 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM     }},    // hh:27 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                        }},    // hh:28 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM     }},    // hh:29 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                       }},    // hh:30 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:31 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                       }},    // hh:32 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:33 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                      }},    // hh:34 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM   }},    // hh:35 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                        }},    // hh:36 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM     }},    // hh:37 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_ZWANZIG_1, WP_GRAD, WP_WARM                        }},    // hh:38 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_ZWANZIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM     }},    // hh:39 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREISSIG_1, WP_GRAD, WP_WARM                                            }},    // hh:40 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM                         }},    // hh:41 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                        }},    // hh:42 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_EIN_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM     }},    // hh:43 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                       }},    // hh:44 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ZWEI_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:45 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                       }},    // hh:46 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_DREI_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:47 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                       }},    // hh:48 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_VIER_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:49 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                      }},    // hh:50 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_FUENF_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM   }},    // hh:51 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                      }},    // hh:52 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SECHS_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM   }},    // hh:53 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                     }},    // hh:54 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_SIEBEN_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM  }},    // hh:55 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                       }},    // hh:56 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_ACHT_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:57 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_DREISSIG_1, WP_GRAD, WP_WARM                       }},    // hh:58 in Mode MM_19
        { MDF_IT_IS_1 | MDF_HOUR_OFFSET_0, { WP_NEUN_1, WP_UND_1, WP_DREISSIG_1, WP_EIN_3, WP_HALB, WP_GRAD, WP_WARM    }},    // hh:59 in Mode MM_19
    },

};

static const WORD_ILLUMINATION illumination[WP_COUNT]=
{
    {  0,  0,  0 },                                                                         // 0 = WP_END_OF_WORDS      = ""
    {  0,  0,  2 | ILLUMINATION_FLAG_IT_IS },                                               // 1 = WP_ES                = "ES"
    {  0,  3,  3 | ILLUMINATION_FLAG_IT_IS },                                               // 3 = WP_IST               = "IST"
    {  0,  7,  7 },                                                                         // 5 = WP_VIERTEL_1         = "VIERTEL"
    {  0, 14,  3 },                                                                         // 6 = WP_EIN_1             = "EIN"
    {  0, 14,  4 },                                                                         // 7 = WP_EINS_1            = "EINS"
    {  0, 15,  2 },                                                                         // 8 = WP_IN                = "IN"
    {  1,  0,  4 },                                                                         // 9 = WP_DREI_1            = "DREI"
    {  1,  2,  3 },                                                                         // 10 = WP_EIN_2             = "EIN"
    {  1,  2,  4 },                                                                         // 11 = WP_EINE_1            = "EINE"
    {  1,  2,  5 },                                                                         // 12 = WP_EINER            = "EINER"
    {  1,  7,  4 },                                                                         // 13 = WP_SECH_1           = "SECH"
    {  1,  7,  5 },                                                                         // 14 = WP_SECHS_1          = "SECHS"
    {  1, 11,  4 },                                                                         // 15 = WP_SIEB_1           = "SIEB"
    {  1, 11,  6 },                                                                         // 16 = WP_SIEBEN_1         = "SIEBEN"
    {  2,  0,  3 },                                                                         // 17 = WP_ELF_1            = "ELF"
    {  2,  2,  4 },                                                                         // 18 = WP_FUENF_1          = "FÃNF"
    {  2,  6,  4 },                                                                         // 19 = WP_NEUN_1           = "NEUN"
    {  2, 10,  4 },                                                                         // 20 = WP_VIER_1           = "VIER"
    {  2, 14,  4 },                                                                         // 21 = WP_ACHT_1           = "ACHT"
    {  3,  0,  4 },                                                                         // 22 = WP_NULL_1           = "NULL"
    {  3,  4,  4 },                                                                         // 23 = WP_ZWEI_1           = "ZWEI"
    {  3,  9,  5 },                                                                         // 24 = WP_ZWOELF_1         = "ZWÃLF"
    {  3, 14,  4 },                                                                         // 25 = WP_ZEHN_1           = "ZEHN"
    {  4,  0,  3 },                                                                         // 26 = WP_UND_1            = "UND"
    {  4,  4,  7 },                                                                         // 27 = WP_ZWANZIG_1        = "ZWANZIG"
    {  4, 11,  7 },                                                                         // 28 = WP_VIERZIG_1        = "VIERZIG"
    {  5,  0,  8 },                                                                         // 29 = WP_DREISSIG_1       = "DREISSIG"
    {  5,  8,  7 },                                                                         // 30 = WP_FUENFZIG_1       = "FÃNFZIG"
    {  5, 15,  3 },                                                                         // 31 = WP_UHR_1            = "UHR"
    {  6,  0,  6 },                                                                         // 32 = WP_MINUTE_1         = "MINUTE"
    {  6,  0,  7 },                                                                         // 33 = WP_MINUTEN_1        = "MINUTEN"
    {  6,  8,  3 },                                                                         // 34 = WP_VOR_1            = "VOR"
    {  6, 11,  3 },                                                                         // 35 = WP_UND_2            = "UND"
    {  6, 14,  4 },                                                                         // 36 = WP_NACH_1           = "NACH"
    {  7,  0,  3 },                                                                         // 37 = WP_EIN_3            = "EIN"
    {  7,  3, 11 },                                                                         // 38 = WP_DREIVIERTEL      = "DREIVIERTEL"
    {  7,  7,  7 },                                                                         // 39 = WP_VIERTEL_2        = "VIERTEL"
    {  7, 14,  4 },                                                                         // 40 = WP_HALB             = "HALB"
    {  8,  0,  4 },                                                                         // 41 = WP_SIEB_2           = "SIEB"
    {  8,  0,  6 },                                                                         // 42 = WP_SIEBEN_2         = "SIEBEN"
    {  8,  5,  4 },                                                                         // 43 = WP_NEUN_2           = "NEUN"
    {  8,  8,  4 },                                                                         // 44 = WP_NULL_2           = "NULL"
    {  8, 12,  4 },                                                                         // 45 = WP_ZWEI_2           = "ZWEI"
    {  8, 14,  3 },                                                                         // 46 = WP_EIN_4            = "EIN"
    {  8, 14,  4 },                                                                         // 47 = WP_EINE_2           = "EINE"
    {  9,  0,  4 },                                                                         // 48 = WP_FUENF_2          = "FÃNF"
    {  9,  4,  4 },                                                                         // 49 = WP_SECH_2           = "SECH"
    {  9,  4,  5 },                                                                         // 50 = WP_SECHS_2          = "SECHS"
    {  9,  9,  4 },                                                                         // 51 = WP_NACH_2           = "NACH"
    {  9, 10,  4 },                                                                         // 51 = WP_ACHT_2           = "ACHT"
    {  9, 14,  4 },                                                                         // 52 = WP_VIER_2           = "VIER"
    { 10,  0,  4 },                                                                         // 53 = WP_DREI_2           = "DREI"
    { 10,  2,  3 },                                                                         // 54 = WP_EIN_5            = "EIN"
    { 10,  2,  4 },                                                                         // 55 = WP_EINS_2           = "EINS"
    { 10,  6,  3 },                                                                         // 56 = WP_UND_3            = "UND"
    { 10, 10,  3 },                                                                         // 57 = WP_ELF_2            = "ELF"
    { 10, 14,  4 },                                                                         // 58 = WP_ZEHN_2           = "ZEHN"
    { 11,  0,  7 },                                                                         // 59 = WP_ZWANZIG_2        = "ZWANZIG"
    { 11,  7,  4 },                                                                         // 60 = WP_GRAD             = "GRAD"
    { 11, 10,  8 },                                                                         // 61 = WP_DREISSIG_2       = "DREISSIG"
    { 12,  0,  7 },                                                                         // 62 = WP_VIERZIG_2        = "VIERZIG"
    { 12,  7,  5 },                                                                         // 63 = WP_ZWOELF_2         = "ZWÃLF"
    { 12, 11,  7 },                                                                         // 64 = WP_FUENFZIG_2       = "FÃNFZIG"
    { 13,  0,  6 },                                                                         // 65 = WP_MINUTE_2         = "MINUTE"
    { 13,  0,  7 },                                                                         // 66 = WP_MINUTEN_2        = "MINUTEN"
    { 13,  7,  3 },                                                                         // 67 = WP_UHR_2            = "UHR"
    { 13, 11,  4 },                                                                         // 68 = WP_FRUEH            = "FRÃH"
    { 13, 15,  3 },                                                                         // 69 = WP_VOR_2            = "VOR"
    { 14,  0,  6 },                                                                         // 70 = WP_ABENDS           = "ABENDS"
    { 14,  6, 11 },                                                                         // 71 = WP_MITTERNACHT      = "MITTERNACHT"
    { 14, 12,  4 },                                                                         // 72 = WP_NACH_3           = "NACH"
    { 14, 12,  6 },                                                                         // 73 = WP_NACHTS           = "NACHTS"
    { 15,  0,  7 },                                                                         // 74 = WP_MORGENS          = "MORGENS"
    { 15,  7,  4 },                                                                         // 75 = WP_WARM             = "WARM"
    { 15, 11,  7 },                                                                         // 76 = WP_MITTAGS          = "MITTAGS"
 };

static_assert(sizeof(tbl_modes) / sizeof(tbl_modes[0]) == DISPLAY_MODES_COUNT,
              "display mode table size");
static_assert(sizeof(illumination) / sizeof(illumination[0]) == WP_COUNT,
              "illumination table size");

} // namespace wc24h
