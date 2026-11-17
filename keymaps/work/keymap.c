#include QMK_KEYBOARD_H
#include "os_detection.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_based(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,     KC_9,    KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,     KC_O,    KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,     KC_L,    KC_SCLN,  KC_QUOT,  KC_ENT,   KC_NO,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMMA, KC_DOT,  KC_SLSH,  KC_RSFT,  KC_NO,    KC_NO,
        KC_LCTL, KC_LALT, KC_LGUI, MO(1),   KC_NO,   KC_SPC,  KC_SPC,  KC_LEFT, KC_DOWN,  KC_UP,   KC_RIGHT, KC_NO,    KC_NO,    KC_NO
    ),
    [1] = LAYOUT_based(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,    KC_F9,    KC_F10,  KC_F11,   KC_F12,   KC_DEL,
        _______, _______, _______, _______, _______, _______, KC_HOME, KC_PGUP, KC_PGDN,  KC_END,   _______, _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, KC_LEFT, KC_DOWN, KC_UP,    KC_RIGHT, _______, _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______, _______,  _______,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______,  _______, _______,  _______,  _______
    )
};

void draw_block(uint8_t start_y, uint8_t height, bool invert) {
    for (uint8_t y = start_y; y < start_y + height; y++) {
        for (uint8_t x = 0; x < 32; x++) {
            bool is_border = (x == 0 || x == 31 || y == start_y || y == start_y + height - 1);
            oled_write_pixel(x, y, is_border ? true : invert);
        }
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_90;
}

bool oled_task_user(void) {
    static bool first_draw = true;
    static uint8_t last_mods = 0xFF;
    static bool last_caps = false;
    static layer_state_t last_layer = 255;
    static os_variant_t last_os = OS_UNSURE;

    uint8_t cur_mods = get_mods() | get_oneshot_mods();
    bool cur_caps = host_keyboard_led_state().caps_lock;
    layer_state_t cur_layer = get_highest_layer(layer_state);
    os_variant_t cur_os = detected_host_os();

    if (!first_draw &&
        cur_mods == last_mods &&
        cur_caps == last_caps &&
        cur_layer == last_layer &&
        cur_os == last_os) {
        return false;
    }

    if (first_draw) {
        draw_block(0, 31, false);
        draw_block(32, 31, false);
        draw_block(64, 63, cur_caps);
        first_draw = false;
    } else if (cur_caps != last_caps) {
        draw_block(64, 63, cur_caps);
    }

    last_mods = cur_mods;
    last_caps = cur_caps;
    last_layer = cur_layer;
    last_os = cur_os;

    oled_set_cursor(1, 1);
    oled_write_P(PSTR("OS:"), false);
    oled_set_cursor(1, 2);
    switch (cur_os) {
        case OS_MACOS:   oled_write_P(PSTR("MAC "), false); break;
        case OS_IOS:     oled_write_P(PSTR("IOS "), false); break;
        case OS_WINDOWS: oled_write_P(PSTR("WIN "), false); break;
        case OS_LINUX:   oled_write_P(PSTR("GNU "), false); break;
        default:         oled_write_P(PSTR("WTF "), false); break;
    }

    oled_set_cursor(1, 5);
    switch (cur_layer) {
        case 0:  oled_write_P(PSTR("BASE"), false); break;
        case 1:  oled_write_P(PSTR("FUNC"), false); break;
        default: oled_write_P(PSTR("    "), false); break;
    }
    oled_set_cursor(1, 6);
    oled_write_P(PSTR("LAYR"), false);

    oled_set_cursor(1, 9);
    oled_write_P(cur_caps ? PSTR("CAPS") : PSTR("MOD:"), cur_caps);

    oled_set_cursor(1, 11);
    oled_write_P((cur_mods & MOD_MASK_CTRL)  ? PSTR("CTRL") : PSTR("    "), cur_caps);

    oled_set_cursor(1, 12);
    oled_write_P((cur_mods & MOD_MASK_SHIFT) ? PSTR("SHFT") : PSTR("    "), cur_caps);

    oled_set_cursor(1, 13);
    oled_write_P((cur_mods & MOD_MASK_ALT)   ? PSTR("ALT ") : PSTR("    "), cur_caps);

    oled_set_cursor(1, 14);
    oled_write_P((cur_mods & MOD_MASK_GUI)   ? PSTR("SUPR") : PSTR("    "), cur_caps);
    return false;
}
