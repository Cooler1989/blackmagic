#ifndef _GPIO_GENERIC_H_
#define _GPIO_GENERIC_H_

using pin_t = uint32_t;
using port_t = uint32_t;

struct GenericGpioType {
  GenericGpioType(){}
  GenericGpioType(port_t port_a, pin_t pin_a):
    port(port_a), pin(pin_a){}
  port_t port;
  pin_t pin;
};

template<class T>
concept GpioType = requires {
  T::gpio_set_value;
  T::gpio_get_value;
  T::gpio_set_high;
  T::gpio_set_low;
};

struct GenericGpio final {
  static constexpr auto gpio_set_value = [](const GenericGpioType &pin, bool value)
  {
    gpio_set_val(pin.port, pin.pin, value);
  };
  static constexpr auto gpio_get_value = [](const GenericGpioType &pin)
  {
    return gpio_get(pin.port, pin.pin);
  };
  static constexpr auto gpio_set_high = [](const GenericGpioType &pin)
  {
    gpio_set(pin.port, pin.pin);
  };
  static constexpr auto gpio_set_low = [](const GenericGpioType &pin)
  {
    gpio_clear(pin.port, pin.pin);
  };
};

#endif  //  _GPIO_GENERIC_H_
