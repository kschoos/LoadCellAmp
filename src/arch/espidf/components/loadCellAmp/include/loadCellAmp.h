#include <driver/gpio.h>
#include <esp_types.h>
#include <loadCellAmpCommon.h>
#include <driver/timer.h>

static void IRAM_ATTR dataISR(void *that);

class LoadCellAmp : LoadCellAmpCommon<gpio_num_t>{

  public:
  LoadCellAmp(gpio_num_t dout_pin, gpio_num_t sp_clk_pin) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin) {
    init(TIMER_GROUP_0, TIMER_0);
  };

  LoadCellAmp(gpio_num_t dout_pin, 
      gpio_num_t sp_clk_pin, 
      timer_group_t timer_group,
      timer_idx_t timer_idx) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin) {
    init(timer_group, timer_idx);
  };


  private: 
  const uint32_t DATA_TIMER_DIVIDER = 800;
  const uint64_t DATA_TIMER_ALARM_VALUE = 1;

  timer_group_t timer_group;
  timer_idx_t timer_idx;
  uint32_t timer_counter;

  void setupGPIO();

  void setupDataTimer();
  void setupDataInterrupt();
  void init(timer_group_t timer_group, timer_idx_t timer_idx);
  inline void toggleClkOutput();

  friend void IRAM_ATTR dataISR(void *that);

  // static void IRAM_ATTR dataISR(void* that);
};
