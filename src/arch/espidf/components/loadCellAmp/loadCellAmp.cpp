#include "include/loadCellAmp.h"


// Constructors ================================================================================================
// =============================================================================================================
LoadCellAmp::LoadCellAmp(gpio_num_t dout_pin, gpio_num_t sp_clk_pin) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin) {
  init(TIMER_GROUP_0, TIMER_0);
};

LoadCellAmp::LoadCellAmp(gpio_num_t dout_pin, 
    gpio_num_t sp_clk_pin, 
    timer_group_t timer_group,
    timer_idx_t timer_idx) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin) {
  init(timer_group, timer_idx);
};
// Private Members  ============================================================================================
// =============================================================================================================

void LoadCellAmp::init(timer_group_t timer_group, timer_idx_t timer_idx){
  this->timer_group = timer_group;
  this->timer_idx = timer_idx;

  setupGPIO();
  setupClkTimer();
}

inline void LoadCellAmp::toggleClkOutput(){
  gpio_set_level(this->sp_clk_pin, this->timer_counter % 2);
}

// ISR Setup ===================================================================================================
// =============================================================================================================

static void IRAM_ATTR clkISR(void* params){
  LoadCellAmp *that = static_cast<LoadCellAmp*>(params);
  that->toggleClkOutput();
	that->timer_counter += 1;

  timer_group_clr_intr_status_in_isr(that->timer_group, that->timer_idx);
	timer_group_enable_alarm_in_isr(that->timer_group, that->timer_idx);
}

// Hardware Setup ==============================================================================================
// =============================================================================================================

void LoadCellAmp::setupGPIO(){
  gpio_config_t io_conf;
  uint64_t output_mask = 1 << this->sp_clk_pin;

  io_conf.intr_type= GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = output_mask;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

  gpio_config(&io_conf);
} 

void LoadCellAmp::setupClkTimer(){
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = this->CLK_TIMER_DIVIDER,
    };

    timer_init(this->timer_group, this->timer_idx, &config);
    timer_set_counter_value(this->timer_group, this->timer_idx, 0);
    timer_set_alarm_value(this->timer_group, this->timer_idx, this->CLK_TIMER_ALARM_VALUE);
    timer_enable_intr(this->timer_group, this->timer_idx);
    timer_isr_register(this->timer_group, this->timer_idx, clkISR, (void*)this, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(this->timer_group, this->timer_idx);
}

// =============================================================================================================
