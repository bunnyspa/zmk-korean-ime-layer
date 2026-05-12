#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/keymap.h>
#include <raw_hid/events.h>

#include <zmk/korean_ime_layer.h>

LOG_MODULE_REGISTER(korean_ime_layer, CONFIG_ZMK_LOG_LEVEL);

#define DT_DRV_COMPAT zmk_korean_ime_layer

#if !DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)
#error "Define a zmk,korean-ime-layer node (with korean-layer and english-layer) to enable CONFIG_ZMK_KOREAN_IME_LAYER."
#endif

#define KIM_NODE       DT_INST(0, zmk_korean_ime_layer)
#define KOREAN_LAYER   DT_PROP(KIM_NODE, korean_layer)
#define ENGLISH_LAYER  DT_PROP(KIM_NODE, english_layer)

static uint8_t last_state = 0xFF; /* sentinel: no state seen yet */

static void apply_state(uint8_t state) {
    if (state == last_state) {
        return;
    }
    last_state = state;

    switch (state) {
    case KOREAN_IME_STATE_HANGUL:
        zmk_keymap_layer_deactivate(ENGLISH_LAYER);
        zmk_keymap_layer_activate(KOREAN_LAYER);
        LOG_DBG("IME → HANGUL (layer %d)", KOREAN_LAYER);
        break;
    case KOREAN_IME_STATE_ENGLISH:
        zmk_keymap_layer_deactivate(KOREAN_LAYER);
        zmk_keymap_layer_activate(ENGLISH_LAYER);
        LOG_DBG("IME → ENGLISH (layer %d)", ENGLISH_LAYER);
        break;
    case KOREAN_IME_STATE_NONE:
    default:
        zmk_keymap_layer_deactivate(KOREAN_LAYER);
        zmk_keymap_layer_deactivate(ENGLISH_LAYER);
        LOG_DBG("IME → NONE (both layers off)");
        break;
    }
}

static int korean_ime_layer_listener(const zmk_event_t *eh) {
    struct raw_hid_received_event *ev = as_raw_hid_received_event(eh);
    if (ev == NULL || ev->data == NULL || ev->length < 2) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    if (ev->data[0] != KOREAN_IME_MARKER) {
        return ZMK_EV_EVENT_BUBBLE;
    }
    apply_state(ev->data[1]);
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(korean_ime_layer, korean_ime_layer_listener);
ZMK_SUBSCRIPTION(korean_ime_layer, raw_hid_received_event);
