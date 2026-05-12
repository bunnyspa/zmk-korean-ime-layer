# zmk-korean-ime-layer

A [ZMK](https://zmk.dev) module that switches keymap layers based on the host PC's Korean IME state, so the keyboard automatically swaps physical layouts when you toggle 한/영. Pair with a host reporter app such as [korean-ime-reporter-windows](https://github.com/bunnyspa/korean-ime-reporter-windows) — the reporter detects the OS IME state and pushes it over Raw HID; this module listens and switches layers.

**Common use case:** a Colemak-DH layer for 영문 typing and a QWERTY layer for 한글 input (the Microsoft Korean IME maps jamo to QWERTY positions, so Colemak would scramble them).

**On split keyboards, runs on the central half only** (Raw HID lives on central).

---

## Behavior

| Host state | Module action |
|------------|---------------|
| `HANGUL` | activate `korean-layer`, deactivate `english-layer` |
| `ENGLISH` | activate `english-layer`, deactivate `korean-layer` |
| `NONE` (or unknown) | deactivate both — ZMK's always-active layer 0 takes over |

Redundant states are filtered, so the module is silent until the host reports a transition.

---

## Getting Started

### `config/west.yml`

```yaml
manifest:
  remotes:
    - name: zmkfirmware
      url-base: https://github.com/zmkfirmware
    # --- copy from here ---
    - name: zzeneg
      url-base: https://github.com/zzeneg
    - name: bunnyspa
      url-base: https://github.com/bunnyspa
    # --- to here ---
  projects:
    - name: zmk
      remote: zmkfirmware
      revision: main
      import: app/west.yml
    # --- copy from here ---
    - name: zmk-raw-hid
      remote: zzeneg
      revision: main
    - name: zmk-korean-ime-layer
      remote: bunnyspa
      revision: main
    # --- to here ---
  self:
    path: config
```

### `<keyboard>.conf`

```ini
CONFIG_RAW_HID=y
CONFIG_ZMK_KOREAN_IME_LAYER=y
```

On split keyboards, enable these only on the central half.

### `<keyboard>.keymap` or `<keyboard>.overlay`

```c
/ {
    korean_ime_layer {
        compatible = "zmk,korean-ime-layer";
        korean-layer = <0>;   // e.g. QWERTY layer
        english-layer = <1>;  // e.g. Colemak-DH layer
    };
};
```

If you don't already define a base/fallback layout, put it on layer 0 — when the host reports `NONE`, both configured layers are deactivated and layer 0 becomes the only active layer.

### Host reporter

Install [korean-ime-reporter-windows](https://github.com/bunnyspa/korean-ime-reporter-windows) (or any host app that speaks the same protocol) and leave it running. Without it, the module compiles in but never receives any state and never switches layers.

---

## Parameters

**`korean-layer`** — layer index activated when the host IME is in 한글 mode.

**`english-layer`** — layer index activated when the host IME is in 영문 mode.

Both are required. They typically point to two physical-layout layers (e.g. QWERTY and Colemak-DH). Set one of them to `0` if you want that layer to also act as the `NONE`-state fallback.

---

## Notes

The transport is plain QMK-compatible Raw HID via [zzeneg/zmk-raw-hid](https://github.com/zzeneg/zmk-raw-hid). See the [korean-ime-reporter-windows README](https://github.com/bunnyspa/korean-ime-reporter-windows#wire-protocol) for the wire-protocol spec (usage page, marker byte, state values). Other listeners on the same Raw HID channel coexist by ignoring non-matching markers.
