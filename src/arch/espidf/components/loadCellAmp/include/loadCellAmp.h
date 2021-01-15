#ifndef LOAD_CELL_AMP_H
#define LOAD_CELL_AMP_H

#include <driver/gpio.h>
#include <esp_types.h>
#include <loadCellAmpCommon.h>
#include <driver/timer.h>

// We have to declare this static here so that we can declare it as 'friend' inside the class definition.
static void IRAM_ATTR dataISR(void *that);
static bool IRAM_ATTR clkISR(void *that);

class LoadCellAmp : public LoadCellAmpCommon<gpio_num_t>{

  public:
  LoadCellAmp(gpio_num_t dout_pin, gpio_num_t sp_clk_pin);

  LoadCellAmp(gpio_num_t dout_pin, 
      gpio_num_t sp_clk_pin, 
      timer_group_t timer_group,
      timer_idx_t timer_idx);

	virtual ~LoadCellAmp();

  private: 
  // TODO: Make this settable via menuconfig. Maybe even divise
  // a simple constexpr function so we can enter a frequency and get the appropriate divider / alarm value pair.
  const uint32_t CLK_TIMER_DIVIDER = 3200;
  const uint64_t CLK_TIMER_ALARM_VALUE = 1;

  timer_group_t timer_group;
  timer_idx_t timer_idx;
  volatile uint32_t timer_counter = 0;

  void setupGPIO();

  void setupClkTimer();
  void setupDataInterrupt();
  void init(timer_group_t timer_group, timer_idx_t timer_idx);
  inline void toggleClkOutput();

  friend void IRAM_ATTR dataISR(void *that);
  friend bool IRAM_ATTR clkISR(void *that);
};

#endif
