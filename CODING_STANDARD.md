# Gestell Mini-ECU — Embedded C Coding Standard

> **Scope:** All firmware under `Embedded/Firmware/Src` (MCAL → HAL → OS → APP).
> **Target:** ATmega128 @ 16 MHz, `avr-gcc -std=gnu99`, MISRA-C inspired (`NFR-08`).
> **Reference drivers:** `Mcal/Usart0` and `Mcal/Adc`. When this document is silent, copy what they do.

---

## 1. Module File Structure

Every driver lives in its own folder and is split into exactly four files:

| File | Content | Included by |
|:---|:---|:---|
| `<MOD>_Private.h` | Register bit positions, masks, enums, typedefs, internal macros | `_Config.h`, `_Interface.h` |
| `<MOD>_Config.h` | Pre-compile user options (`#define`) | `_Interface.h` |
| `<MOD>_Interface.h` | Public API prototypes + full Doxygen | Upper layers, `_Program.c` |
| `<MOD>_Program.c` | Implementation, static data, ISRs | — (never `#include`d) |

- `<MOD>` is the module name as used in the folder: `ADC`, `Usart0`, `EXTI`.
- Upper layers include **only** `<MOD>_Interface.h` (`NFR-02`).
- Only `_Program.c` includes `Atmega128Register.h` and `MacroFunction.h`. Registers never leak into a header.

---

## 2. File Header

Every file starts with this block, tags aligned on one column:

```c
/**
 * @file      EXTI_Interface.h
 * @brief     One line: what the file is.
 * @details   One or two lines: what it contains.
 * @version   1.0.0
 * @author    Developer: Name (email)
 * @author    Reviewer: Name
 * @date      YYYY-MM-DD
 * @copyright Copyright (c) 2026 Gestell. All rights reserved.
 */
```

---

## 3. Include Guards & Includes

- Guard comes **after** the file header. Name: `<MOD>_<FILE>_H` in upper case, no leading underscore.

```c
#ifndef EXTI_INTERFACE_H
#define EXTI_INTERFACE_H
...
#endif /* EXTI_INTERFACE_H */
```

- Use **relative paths with forward slashes**. Never use backslashes or project-root paths.

```c
#include "../../Common/Config.h"
#include "../Atmega128Register.h"
```

- Order in `_Program.c`: Common headers → own `_Interface.h` → `Atmega128Register.h` → system headers (`<avr/interrupt.h>`).
- `_Config.h` includes `_Private.h` when its options use enum values (ADC pattern).

---

## 4. Module Enable Guard

The whole body of `_Program.c` is wrapped by the switch from `Common/Config.h`:

```c
#if ModuleExtiEnabled == Enable
...
#endif
```

---

## 5. Naming

| Element | Rule | Example |
|:---|:---|:---|
| Public function | `m` + `MOD` + `_` + PascalCase verb | `mADC_SetChannel`, `mUSART0_SendByte` |
| Parameter | PascalCase | `ChannelNo`, `ResultPtr`, `Line` |
| Local variable | camelCase | `sreg`, `counter`, `temp` |
| Static global | `g_` + camelCase | `g_rxRingBuf`, `g_resetEventPending` |
| Enum / struct type | `MOD_Name_t` | `ADC_Status_t`, `USART0_RingBuffer_t` |
| Enum value | `MOD_Group_Value` | `ADC_Status_Ok`, `EXTI_Sense_FallingEdge` |
| Register bit | `NAME_Bit` (datasheet name) | `ADEN_Bit`, `ISC40_Bit` |
| Register mask | `REG_PurposeMask` | `ADMUX_ChannelMask`, `EICRB_SenseMask` |
| Config option | `MOD_UPPER_SNAKE` | `ADC_PRESCALER`, `USART0_RING_BUFFER_SIZE` |
| Register access | `NAME_Reg` (from `Atmega128Register.h`) | `ADCSRA_Reg`, `EIMSK_Reg` |

- The `_Bit` suffix is mandatory: it avoids clashes with `<avr/io.h>` names (`INT4`, `ISC40`, ...).
- No abbreviations nobody can read. Pointers end with `Ptr`.

---

## 6. Types

- Use `<stdint.h>` types only (`uint8_t`, `uint16_t`). No `int`, `char` for data, `unsigned`.
- Use `bool` from `<stdbool.h>` for flags.
- Use `Null` from `Definition.h` for pointers.
- Each enum gets a short Doxygen block; each value a trailing `/* */` comment:

```c
/**
 * @enum ADC_Status_t
 * @brief ADC operation return status codes.
 */
typedef enum {
    ADC_Status_Ok      = 0,  /* Operation completed successfully */
    ADC_Status_Error   = 3   /* Invalid argument or configuration */
} ADC_Status_t;
```

- Unsigned literals use the `U` / `UL` suffix (`64U`, `10000UL`).
- Masks are written in hex with the binary pattern as comment: `0xF8 /* 0b 1111 1000 */`.

---

## 7. Formatting

- 4 spaces indentation, no tabs.
- Allman braces for functions and control blocks. Braces are mandatory, even for one line.
- One statement per line. Align `=` and comments in groups of defines/enums.
- Wrap long conditions one operand per line:

```c
if ((Refsource != ADC_Ref_AREF) &&
    (Refsource != ADC_Ref_AVCC) &&
    (Refsource != ADC_Ref_Internal))
{
    return;
}
```

- Max function length 50 lines, max nesting depth 4 (`NFR-08`).

---

## 8. Function Flow

Every function follows the same flow as the reference drivers:

1. **Validate** arguments / state first with guard clauses and early `return`.
2. **Act** on the hardware with short step comments.
3. **Return** status if the function can fail.

```c
void mADC_SetChannel(uint8_t ChannelNo)
{
    if (ChannelNo > 7)
    {
        return;
    }

    uint8_t Temp = ADMUX_Reg;
    Temp &= ADMUX_ChannelMask;
    Temp |= ChannelNo;
    ADMUX_Reg = Temp;
}
```

- `Init` reuses the module's own setters instead of duplicating register code (`mADC_Init` → `mADC_SetReference`, `mADC_SetChannel`).
- Variables are declared where they are first needed (C99).

---

## 9. Register Access

- Hardware is accessed **only** through `XXX_Reg` macros, **only** inside MCAL (`NFR-01`).
- Single bits: `SetBit`, `ClearBit`, `GetBit` from `MacroFunction.h`.
- Multi-bit fields: read-modify-write through a local `temp` so other fields are preserved.
- **Write-1-to-clear flag registers** (`EIFR`, `TIFR`, ...) are written with `=`, never `|=`/`SetBit`, otherwise every pending flag is cleared.

```c
EIFR_Reg = (uint8_t)(1U << Line);   /* Write 1 to clear this flag only */
```

---

## 10. Interrupts & Shared Data

- Data shared between an ISR and normal code is `static volatile`.
- Multi-byte shared data (pointers, `uint16_t`, ring buffers) is accessed inside a critical section:

```c
uint8_t sreg = SREG_Reg;
cli();
/* ... access shared data ... */
SREG_Reg = sreg;
```

- Restore `SREG` before **every** `return` inside the critical section.
- ISRs are short: set a flag / move one byte, then leave. **No** delays, loops, UART sends, LCD writes or state machine logic in an ISR.
- Debouncing and decisions belong to the OS scheduler, not to MCAL.

---

## 11. Comments & Doxygen

Keep Doxygen **clean and short** — describe what, not an essay.

**`_Interface.h`** — full contract of each API:

```c
/**
 * @brief Selects the analog input channel in ADMUX without corrupting reference bits.
 * @param ChannelNo Input channel index (ADC_Channel0 through ADC_Channel7).
 * @note  Invalid channel values are ignored.
 */
void mADC_SetChannel(uint8_t ChannelNo);
```

- Allowed tags: `@brief`, `@details` (only when needed), `@param`, `@return`, `@note`, `@warning`.

**`_Program.c`** — repeat only `@brief` (+ `@param` / `@return`) above each function. No `@details`.

**`_Config.h`** — `@def` + `@brief` (+ allowed values) for each option.

**`_Private.h`** — register bits as one-line defines with trailing comment:

```c
#define ADEN_Bit    7   /* ADC enable */
```

**Inside functions** — short `/* */` step comments above a group of lines. Do not comment the obvious.

---

## 12. Forbidden

- `malloc` / `calloc` / `free` (`NFR-06`).
- `_delay_ms` / blocking loops in ISRs or in the main control loop (`NFR-03`).
- Register access above MCAL (`NFR-01`).
- Magic numbers in code — use a named bit, mask, enum or config option.
- Backslashes in `#include` paths.
- Including a `.c` file or another module's `_Private.h`.

---

## 13. Git

Follow [GIT_WORKFLOW.md](Documention/Rules/GIT_WORKFLOW.md):

- Branch: `feature/<n>-em-<name>` from `em_develop`.
- Commit: `<type>(<scope>): <subject>` — e.g. `feat(embedded): Implement EXTI driver for INT4 and INT5`.

---

## 14. Review Checklist

- [ ] Four files, correct names, header block, guard.
- [ ] Relative forward-slash includes; module enable guard in `_Program.c`.
- [ ] Naming table respected; `_Bit` suffix on bit defines.
- [ ] Arguments validated with guard clauses.
- [ ] RMW on shared registers; `=` on write-1-to-clear registers.
- [ ] Shared ISR data `volatile`; multi-byte access in a critical section.
- [ ] ISR contains no delay, loop or business logic.
- [ ] Doxygen short; full contract in `_Interface.h` only.
- [ ] Builds with `make build` without warnings.
