#include "include/loadCellAmp.h"
// TODO: Error checks for all the esp-related functions.

// Constructors ================================================================================================
// =============================================================================================================
LoadCellAmp::LoadCellAmp(gpio_num_t dout_pin, gpio_num_t sp_clk_pin) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin, AmpGain::gain128) {
  init(TIMER_GROUP_0, TIMER_0);
};

LoadCellAmp::LoadCellAmp(gpio_num_t dout_pin, 
    gpio_num_t sp_clk_pin, 
    timer_group_t timer_group,
    timer_idx_t timer_idx) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin, AmpGain::gain128) {
  init(timer_group, timer_idx);
};

LoadCellAmp::LoadCellAmp(gpio_num_t dout_pin, 
    gpio_num_t sp_clk_pin, 
    timer_group_t timer_group,
    timer_idx_t timer_idx,
    AmpGain gain
    ) : LoadCellAmpCommon<gpio_num_t>(dout_pin, sp_clk_pin, gain) {
  init(timer_group, timer_idx);
};
// Destructors =================================================================================================
// =============================================================================================================

LoadCellAmp::~LoadCellAmp(){
	ESP_ERROR_CHECK(gpio_intr_disable(this->dout_pin));
	ESP_ERROR_CHECK(timer_disable_intr(this->timer_group, this->timer_idx));

  ESP_ERROR_CHECK(timer_pause(this->timer_group, this->timer_idx));
	ESP_ERROR_CHECK(timer_deinit(this->timer_group, this->timer_idx));

	ESP_ERROR_CHECK(gpio_isr_handler_remove(this->dout_pin));
	ESP_ERROR_CHECK(gpio_reset_pin(this->dout_pin));
	ESP_ERROR_CHECK(gpio_reset_pin(this->sp_clk_pin));
}

// Private Members  ============================================================================================
// =============================================================================================================

void LoadCellAmp::init(timer_group_t timer_group, timer_idx_t timer_idx){
  this->timer_group = timer_group;
  this->timer_idx = timer_idx;

  setupGPIO();
  setupClkTimer();
}

inline void LoadCellAmp::toggleClkOutput(){
  gpio_set_level(this->sp_clk_pin, this->timer_counter & 2); // Toggle based on 2nd bit
}

// ISR Setup ===================================================================================================
// =============================================================================================================
volatile int test = 0;

static bool IRAM_ATTR clkISR(void* params){
  LoadCellAmp *that = static_cast<LoadCellAmp*>(params);
	that->timer_counter += 1;
  that->toggleClkOutput();

  if((that->timer_counter & 3) == 3){
    that->isrNewValue(static_cast<uint8_t>(gpio_get_level(that->dout_pin)));
  }

  if(that->timer_counter / 4 == that->n_pulses){
     ESP_ERROR_CHECK(gpio_intr_enable(that->dout_pin));
     that->timer_counter = 0;

     ESP_ERROR_CHECK(timer_pause(that->timer_group, that->timer_idx));
     that->isrDataReady();
  }

  timer_group_enable_alarm_in_isr(that->timer_group, that->timer_idx);

  return false;
}

// Whenever we arent reading and receive a negative edge, data is available
static void IRAM_ATTR dataISR(void* params){
  LoadCellAmp *that = static_cast<LoadCellAmp*>(params);

	timer_group_enable_alarm_in_isr(that->timer_group, that->timer_idx);
  timer_enable_intr(that->timer_group, that->timer_idx);

  ESP_ERROR_CHECK(timer_start(that->timer_group, that->timer_idx));
  ESP_ERROR_CHEK(gpio_intr_disable(that->dout_pin));
}

// Hardware Setup ==============================================================================================
// =============================================================================================================
void LoadCellAmp::setupGPIO(){
	printf("Setting up GPIOs...\n");
  gpio_config_t io_conf;

  // SP_CLK GPIO
  uint64_t output_mask = (1 << this->sp_clk_pin) | (1 << GPIO_NUM_25);

  io_conf.intr_type= GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = output_mask;
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

  esp_err_t err = gpio_config(&io_conf);


  // Data GPIO
  uint64_t input_mask = 1 << this->dout_pin; 
  
  io_conf.intr_type = GPIO_INTR_NEGEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = input_mask;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

  ESP_ERROR_CHCK(gpio_config(&io_conf));

  // We want to disable the interrupt for this after it is triggered for as long as we are reading data.
  // After that we will enable it again. (ESP_INTR_FLAG_INTRDISABLED)
  esp_err_t err = gpio_install_isr_service(0);// ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_EDGE | ESP_INTR_FLAG_LEVEL1 ) ;
  switch(err){
    case ESP_ERR_INVALID_STATE:
        break;
    default:
        ESP_ERROR_CHECK(err);
        break;
  }

  ESP_ERROR_CHECK(gpio_isr_handler_add(this->dout_pin, dataISR, (void*)this));
	printf("GPIOs set up.\n");
} 

void LoadCellAmp::setupClkTimer(){
	printf("Setting up Timer...\n");
    timer_config_t config = {
        .alarm_en = TIMER_ALARM_EN,
        .counter_en = TIMER_PAUSE,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = TIMER_AUTORELOAD_EN,
        .divider = this->CLK_TIMER_DIVIDER,
    };

    ESP_ERROR_CHECK(timer_init(this->timer_group, this->timer_idx, &config));
    ESP_ERROR_CHECK(timer_set_counter_value(this->timer_group, this->timer_idx, 0));
    ESP_ERROR_CHECK(timer_set_alarm_value(this->timer_group, this->timer_idx, this->CLK_TIMER_ALARM_VALUE));
    ESP_ERROR_CHECK(timer_isr_callback_add(this->timer_group, this->timer_idx, clkISR, (void*)this, ESP_INTR_FLAG_IRAM));
    ESP_ERROR_CHECK(timer_enable_intr(this->timer_group, this->timer_idx));
    //timer_isr_register(this->timer_group, this->timer_idx, clkISR, (void*)this, ESP_INTR_FLAG_IRAM, NULL);
	  printf("Done setting up timer.\n");
}

// =============================================================================================================
