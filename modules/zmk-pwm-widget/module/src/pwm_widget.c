#include <zephyr/device.h>
#include <zephyr/drivers/pwm.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/pwm_widget.h>

#define PWM_NODE DT_NODELABEL(pwm0)
#define PWM_PERIOD 32767

static const struct device *pwm_dev = DEVICE_DT_GET(PWM_NODE);

static void pwm_set_rgb(uint8_t r, uint8_t g, uint8_t b) {
    pwm_set(pwm_dev, 2, PWM_PERIOD, r * 257);  // 赤
    pwm_set(pwm_dev, 1, PWM_PERIOD, g * 257);  // 緑
    pwm_set(pwm_dev, 0, PWM_PERIOD, b * 257);  // 青
}

static int pwm_widget_listener(const zmk_event_t *eh) {
    const struct layer_state_changed *ev = as_layer_state_changed(eh);
    if (!ev) return 0;

    int layer = ev->state;

    switch (layer) {
        // 通常系（黄緑系：充電中の緑と区別）
        case 0: pwm_set_rgb(128, 255, 0); break;     // BASE 黄緑
        case 1: pwm_set_rgb(160, 255, 40); break;    // NAVI 明るい黄緑
        case 2: pwm_set_rgb(100, 200, 0); break;     // SYM 落ち着いた黄緑
        // Lower 系（黄色系）
        case 3: pwm_set_rgb(255, 255, 0); break;     // KMOV
        case 4: pwm_set_rgb(220, 220, 0); break;     // MMOV
        // BTL（青）
        case 5: pwm_set_rgb(0, 0, 255); break;       // BLT
        // ADJ（グレー）
        case 6: pwm_set_rgb(128, 128, 128); break;   // ADJ
        // Raise 系（紫系）
        case 7: pwm_set_rgb(255, 0, 255); break;     // FUNC
        case 8: pwm_set_rgb(200, 0, 200); break;     // NUM
        default: pwm_set_rgb(0, 0, 0); break;        // 消灯
    }

    return 0;
}

ZMK_LISTENER(pwm_widget, pwm_widget_listener);
ZMK_SUBSCRIPTION(pwm_widget, layer_state_changed);

int pwm_widget_init(void) {
    return 0;
}
