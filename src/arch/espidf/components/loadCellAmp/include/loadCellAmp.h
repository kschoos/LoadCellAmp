#ifndef LOAD_CELL_AMP_H
#define LOAD_CELL_AMP_H

#include <driver/gpio.h>
#include <esp_types.h>
#include <loadCellAmpCommon.h>
#include <driver/timer.h>
#include <esp_err.h>

enum class AmpGain;

class LoadCellAmp : public LoadCellAmpCommon<gpio_num_t>{

  public:
  bool oneshot = false;

  LoadCellAmp(const gpio_num_t dout_pin,const gpio_num_t sp_clk_pin);

  LoadCellAmp(const gpio_num_t dout_pin, 
      const gpio_num_t sp_clk_pin, 
      const timer_group_t timer_group,
      const timer_idx_t timer_idx);

  LoadCellAmp(const gpio_num_t dout_pin, 
      const gpio_num_t sp_clk_pin, 
      const timer_group_t timer_group,
      const timer_idx_t timer_idx,
      const AmpGain gain);

	virtual ~LoadCellAmp();

  private: 
  // TODO: Make this settable via menuconfig. Maybe even divise
  // a simple constexpr function so we can enter a frequency and get the appropriate divider / alarm value pair.
  const uint32_t CLK_TIMER_DIVIDER = 32;
  const uint64_t CLK_TIMER_ALARM_VALUE = 1;

  timer_group_t timer_group;
  timer_idx_t timer_idx;
  volatile uint32_t timer_counter = 0;

  void setupGPIO();

  void setupClkTimer();
  void setupDataInterrupt();
  void init();
  inline void toggleClkOutput();

  static void IRAM_ATTR dataISR(void *that);
  static bool IRAM_ATTR clkISR(void *that);
};

#endif
