#include QMK_KEYBOARD_H
#include "os_detection.h" // Подключаем встроенный детектор ОС

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

#ifdef OLED_ENABLE

// Кастомная функция для отрисовки тонких рамок попиксельно (так как oled_write_rect работает с тайлами)
void draw_block_border(uint8_t start_y) {
    // Верхняя и нижняя линии рамки (ширина 32 пикселя)
    for (uint8_t x = 0; x < 32; x++) {
        oled_write_pixel(x, start_y, true);
        oled_write_pixel(x, start_y + 30, true);
    }
    // Левая и правая линии рамки (высота 31 пиксель)
    for (uint8_t y = start_y; y < start_y + 31; y++) {
        oled_write_pixel(0, y, true);
        oled_write_pixel(31, y, true);
    }
}

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
    return OLED_ROTATION_90;
}

static uint16_t last_keycode = 0;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        last_keycode = keycode;
    }
    return true;
}

bool oled_task_user(void) {
    // ==========================================
    // БЛОК 1: WPM (Y: 0 ... 30)
    // ==========================================
    draw_block_border(0);
    #ifdef WPM_ENABLE
    oled_set_cursor(1, 1);
    oled_write_P(PSTR("WPM"), false);
    oled_set_cursor(1, 2);
    oled_write(get_u8_str(get_current_wpm(), ' '), false);
    #endif

    // ==========================================
    // БЛОК 2: СЛОЙ (Y: 32 ... 62)
    // ==========================================
    draw_block_border(32);
    oled_set_cursor(1, 5);
    oled_write_P(PSTR("LYR"), false);
    oled_set_cursor(1, 6);
    switch (get_highest_layer(layer_state)) {
        case 0:  oled_write_P(PSTR("BASE"), false); break;
        case 1:  oled_write_P(PSTR("LOWR"), false); break;
        case 2:  oled_write_P(PSTR("RAIS"), false); break;
        default: oled_write_P(PSTR("GKEY"), false); break;
    }

    // ==========================================
    // БЛОК 3: КЕЙКОД (Y: 64 ... 94)
    // ==========================================
    draw_block_border(64);
    oled_set_cursor(1, 9);
    oled_write_P(PSTR("KEY"), false);
    oled_set_cursor(1, 10);
    if (last_keycode == 0) {
        oled_write_P(PSTR("NONE"), false);
    } else {
        char kc_str[5];
        snprintf(kc_str, sizeof(kc_str), "%02X", last_keycode & 0xFF);
        oled_write(kc_str, false);
    }

    // ==========================================
    // БЛОК 4: ОПЕРАЦИОННАЯ СИСТЕМА (Y: 96 ... 126)
    // ==========================================
    draw_block_border(96);
    oled_set_cursor(1, 13);
    oled_write_P(PSTR("OS:"), false);
    oled_set_cursor(1, 14);

    // Вызываем корректное имя функции детектора из QMK API
    os_variant_t detected_os = detected_host_os();
    switch (detected_os) {
        case OS_MACOS:
        case OS_IOS:
            oled_write_P(PSTR("APL"), false);
            break;
        case OS_WINDOWS:
            oled_write_P(PSTR("WIN"), false);
            break;
        case OS_LINUX:
            oled_write_P(PSTR("GNU"), false);
            break;
        default: // Сюда же попадает OS_UNSURE на этапе инициализации
            oled_write_P(PSTR("???"), false);
            break;
    }

    return false;
}
#endif
