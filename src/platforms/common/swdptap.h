#ifndef _PLATFORM_COMMON_SWDPTAP_H_
#define _PLATFORM_COMMON_SWDPTAP_H_

#include "general.h"
#include "timing.h"
#include "adiv5.h"

#include <bitset>

enum {
	SWDIO_STATUS_FLOAT = 0,
	SWDIO_STATUS_DRIVE
};

using Bits = std::bitset<32>;

class ADIv5DpHwLayer_i
{
  public:
    virtual void seq_out_parity(uint32_t MS, int ticks) = 0;
    virtual void seq_out(Bits MS, int ticks) = 0;
    virtual bool seq_in_parity(uint32_t *ret, int ticks) = 0;
    virtual Bits seq_in(int ticks) = 0;
};

template<GpioType GpioObj>
class ADIv5DpHwLayer: public ADIv5DpHwLayer_i
{
  public:
    void seq_out_parity(uint32_t MS, int ticks) override
    {
      int parity = __builtin_popcount(MS);
#ifdef DEBUG_SWD_BITS
      for (int i = 0; i < ticks; i++)
        DEBUG("%d", (MS & (1 << i)) ? 1 : 0);
#endif
      register volatile int32_t cnt;
      swdptap_turnaround(SWDIO_STATUS_DRIVE);
      GpioObj::gpio_set_value(swdio_gpio, MS & 1);
      MS >>= 1;
      if (swd_delay_cnt) {
        while (ticks--) {
          GpioObj::gpio_set_high(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
          GpioObj::gpio_set_value(swdio_gpio, MS & 1);
          MS >>= 1;
          GpioObj::gpio_set_low(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
        }
      } else {
        while (ticks--) {
          GpioObj::gpio_set_high(swclk_gpio);
          GpioObj::gpio_set_value(swdio_gpio, MS & 1);
          MS >>= 1;
          GpioObj::gpio_set_low(swclk_gpio);
        }
      }
      GpioObj::gpio_set_value(swdio_gpio, parity & 1);
      GpioObj::gpio_set_high(swclk_gpio);
      for(cnt = swd_delay_cnt; --cnt > 0;);
      GpioObj::gpio_set_low(swclk_gpio);
      for(cnt = swd_delay_cnt; --cnt > 0;);
    }

    void seq_out(Bits MS, int ticks) override
    {
#ifdef DEBUG_SWD_BITS
      for (int i = 0; i < ticks; i++)
        DEBUG("%d", (MS & (1 << i)) ? 1 : 0);
#endif
      register volatile int32_t cnt;
      swdptap_turnaround(SWDIO_STATUS_DRIVE);
      GpioObj::gpio_set_value(swdio_gpio, MS[0]);
      if (swd_delay_cnt) {
        while (ticks--) {
          GpioObj::gpio_set_high(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
          MS >>= 1;
          GpioObj::gpio_set_value(swdio_gpio, MS[0]);
          GpioObj::gpio_set_low(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
        }
      } else {
        while (ticks--) {
          GpioObj::gpio_set_high(swclk_gpio);
          MS >>= 1;
          GpioObj::gpio_set_value(swdio_gpio, MS[0]);
          GpioObj::gpio_set_low(swclk_gpio);
        }
      }
    }
    bool seq_in_parity(uint32_t *ret, int ticks) override
    {
      uint32_t index = 1;
      uint32_t res = 0;
      bool bit;
      int len = ticks;
      register volatile int32_t cnt;

      swdptap_turnaround(SWDIO_STATUS_FLOAT);
      if (swd_delay_cnt) {
        while (len--) {
          bit = GpioObj::gpio_get_value(swdio_gpio);
          GpioObj::gpio_set_high(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
          res |= (bit) ? index : 0;
          index <<= 1;
          GpioObj::gpio_set_low(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
        }
      } else {
        while (len--) {
          bit = GpioObj::gpio_get_value(swdio_gpio);
          GpioObj::gpio_set_high(swclk_gpio);
          res |= (bit) ? index : 0;
          index <<= 1;
          GpioObj::gpio_set_low(swclk_gpio);
        }
      }
      int parity = __builtin_popcount(res);
      bit = GpioObj::gpio_get_value(swdio_gpio);
      GpioObj::gpio_set_high(swclk_gpio);
      for(cnt = swd_delay_cnt; --cnt > 0;);
      parity += (bit) ? 1 : 0;
      GpioObj::gpio_set_low(swclk_gpio);
      for(cnt = swd_delay_cnt; --cnt > 0;);
#ifdef DEBUG_SWD_BITS
      for (int i = 0; i < len; i++)
        DEBUG("%d", (res & (1 << i)) ? 1 : 0);
#endif
      *ret = res;
      /* Terminate the read cycle now */
      swdptap_turnaround(SWDIO_STATUS_DRIVE);
      return (parity & 1);
    }

    Bits seq_in(int ticks) override
    {
      uint32_t index = 0;
      Bits ret = 0;
      int len = ticks;
      register volatile int32_t cnt;

      swdptap_turnaround(SWDIO_STATUS_FLOAT);
      if (swd_delay_cnt) {
        while (len--) {
          int res;
          res = GpioObj::gpio_get_value(swdio_gpio);
          GpioObj::gpio_set_high(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
          ret[index] = res;
          index++;
          GpioObj::gpio_set_low(swclk_gpio);
          for(cnt = swd_delay_cnt; --cnt > 0;);
        }
      } else {
        int res;
        while (len--) {
          res = GpioObj::gpio_get_value(swdio_gpio);
          GpioObj::gpio_set_high(swclk_gpio);
          ret[index] = res;
          index++;
          GpioObj::gpio_set_low(swclk_gpio);
        }
      }
#ifdef DEBUG_SWD_BITS
      for (int i = 0; i < len; i++)
        DEBUG("%d", (ret & (1 << i)) ? 1 : 0);
#endif
      return ret;
    }

  private:

    void swdptap_turnaround(int dir)
    {
      static int olddir = SWDIO_STATUS_FLOAT;
      register volatile int32_t cnt;

      /* Don't turnaround if direction not changing */
      if(dir == olddir) return;
      olddir = dir;

#ifdef DEBUG_SWD_BITS
      DEBUG("%s", dir ? "\n-> ":"\n<- ");
#endif

      if(dir == SWDIO_STATUS_FLOAT)
        SWDIO_MODE_FLOAT();
      GpioObj::gpio_set_high(swclk_gpio);
      for(cnt = swd_delay_cnt; --cnt > 0;);
      GpioObj::gpio_set_low(swclk_gpio);
      for(cnt = swd_delay_cnt; --cnt > 0;);
      if(dir == SWDIO_STATUS_DRIVE)
        SWDIO_MODE_DRIVE();
    }
    const GenericGpioType swclk_gpio{SWCLK_PORT,SWCLK_PIN};
    const GenericGpioType swdio_gpio{SWDIO_PORT,SWDIO_PIN};
};

ADIv5DpHwLayer_i& SwdpTapInstance();

#endif  //  _PLATFORM_COMMON_SWDPTAP_H_
