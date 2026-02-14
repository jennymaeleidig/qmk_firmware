#include QMK_KEYBOARD_H

// 1. Define Layer Colors
// Orange (19, 74%, 95%) -> QMK {13, 189, 242}
// New: Pure Orange (Hue 13), Max Saturation (255), Lower Brightness (180)
const rgblight_segment_t PROGMEM layer0_indicator[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 13, 255, 180});
// Purple (302, 86%, 80%) -> QMK {214, 219, 204}
const rgblight_segment_t PROGMEM layer1_indicator[] = RGBLIGHT_LAYER_SEGMENTS({0, 1, 214, 255, 180});

const rgblight_segment_t *const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(layer0_indicator, layer1_indicator);

void keyboard_post_init_user(void) {
    rgblight_layers = my_rgb_layers;
    // Set Layer 0 to active on bootup
    rgblight_set_layer_state(0, true);
}

// Handle Layer changes
layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(1, get_highest_layer(state) == 1);
    return state;
}

// 2. Caps Lock Blink Logic (Toggle Power, Preserve Color)
static uint16_t blink_timer;
static bool     led_is_on = true;

void matrix_scan_user(void) {
    if (host_keyboard_led_state().caps_lock) {
        if (timer_elapsed(blink_timer) > 250) { // Blink every 250ms
            blink_timer = timer_read();
            led_is_on   = !led_is_on;
            if (led_is_on) {
                rgblight_enable_noeeprom();
            } else {
                rgblight_disable_noeeprom();
            }
        }
    } else {
        // Ensure LED stays on if Caps Lock is turned off
        if (!led_is_on) {
            led_is_on = true;
            rgblight_enable_noeeprom();
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case KC_BSPC: {
            static uint16_t registered_key = KC_NO;
            if (record->event.pressed) { // On key press.
                const uint8_t mods = get_mods();
#ifndef NO_ACTION_ONESHOT
                uint8_t shift_mods = (mods | get_oneshot_mods()) & MOD_MASK_SHIFT;
#else
                uint8_t shift_mods = mods & MOD_MASK_SHIFT;
#endif                            // NO_ACTION_ONESHOT
                if (shift_mods) { // At least one shift key is held.
                    registered_key = KC_DEL;
                    // If one shift is held, clear it from the mods. But if both
                    // shifts are held, leave as is to send Shift + Del.
                    if (shift_mods != MOD_MASK_SHIFT) {
#ifndef NO_ACTION_ONESHOT
                        del_oneshot_mods(MOD_MASK_SHIFT);
#endif // NO_ACTION_ONESHOT
                        unregister_mods(MOD_MASK_SHIFT);
                    }
                } else {
                    registered_key = KC_BSPC;
                }

                register_code(registered_key);
                set_mods(mods);
            } else { // On key release.
                unregister_code(registered_key);
            }
        }
            return false;

            // Other macros...
    }

    return true;
}