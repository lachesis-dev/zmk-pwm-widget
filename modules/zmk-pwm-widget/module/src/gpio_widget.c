#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zmk/event_manager.h>
#include <zmk/events/layer_state_changed.h>

//
// XIAO BLE の LED ピン
// 赤   → P0.26
// 緑   → P0.30
// 青   → P0.06
//

#define LED_RED_NODE   DT_NODELABEL(led_red)
#define LED_GREEN_NODE DT_NODELABEL(led_green)
#define LED_BLUE_NODE  DT_NODELABEL(led_blue)

static const struct gpio_dt_spec led_red   = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
static const struct gpio_dt_spec led_green = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
static const struct gpio_dt_spec led_blue  = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

static void gpio_set_rgb(bool r, bool g, bool b) {
    gpio_pin_set_dt(&led_red,   r);
    gpio_pin_set_dt(&led_green, g);
    gpio_pin_set_dt(&led_blue,  b);
}

static int gpio_widget_listener(const zmk_event_t *eh) {
    const struct layer_state_changed *ev = as_layer_state_changed(eh);
    if (!ev) return 0;

    int layer = ev->state;

    switch (layer) {
        case 0: gpio_set_rgb(1, 1, 1); break;  // base → 白
        case 1: gpio_set_rgb(1, 0, 1); break;  // num（右）→ ピンク
        case 2: gpio_set_rgb(0, 1, 1); break;  // lower（左）→ 水色
        case 3: gpio_set_rgb(1, 0, 0); break;  // raise（右）→ 赤
        case 4: gpio_set_rgb(0, 1, 0); break;  // nav（左）→ 緑
        case 5: gpio_set_rgb(1, 1, 0); break;  // func（右）→ 黄
        case 6: gpio_set_rgb(0, 1, 1); break;  // lmod（左）→ 水色
        case 7: gpio_set_rgb(1, 0, 1); break;  // rmod（右）→ ピンク
        case 8: gpio_set_rgb(0, 0, 1); break;  // adjust（BLT含む）→ 青（固定）
        default: gpio_set_rgb(0, 0, 0); break; // 消灯
    }

    return 0;
}

ZMK_LISTENER(gpio_widget, gpio_widget_listener);
ZMK_SUBSCRIPTION(gpio_widget, layer_state_changed);

int gpio_widget_init(void) {
    gpio_pin_configure_dt(&led_red,   GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_green, GPIO_OUTPUT_INACTIVE);
    gpio_pin_configure_dt(&led_blue,  GPIO_OUTPUT_INACTIVE);

    // 起動時ピンク（あなたの好み）
    gpio_set_rgb(1, 0, 1);

    return 0;
}
