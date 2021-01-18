#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include <loadCellAmp.h>
#include <driver/gpio.h>
#include "unity.h"
#include <string_view>

#define MESSAGE(x) ("  >>> " #x " <<<   ")


TEST_CASE("Test if test framework is working correctly", ""){
  TEST_ASSERT(1);
}

TEST_CASE("RAII - Destruction means resource resignation", ""){
	LoadCellAmp loadCellAmp(GPIO_NUM_16, GPIO_NUM_17);
}

typedef struct{
  SemaphoreHandle_t sem;
} task_test_t;

static void test_delay_task(void* p){
  task_test_t* arg = static_cast<task_test_t*>(p);

  vTaskDelay(1000 / portTICK_PERIOD_MS);
  xSemaphoreGive(arg->sem);
  vTaskDelete(NULL);
}


TEST_CASE("Test the use of delays in tests.", ""){
  const task_test_t arg = {
    .sem = xSemaphoreCreateBinary()
  };

  xTaskCreate(test_delay_task, "", 2048, (void*) &arg, 3, 0);
  TEST_ASSERT(xSemaphoreTake(arg.sem, 2000 / portTICK_PERIOD_MS));
  vSemaphoreDelete(arg.sem);
}

volatile uint32_t ticksMeasured = 0;

static void IRAM_ATTR clkinISR(void *params){
  ticksMeasured += 1;
}

void setupGPIO(gpio_num_t data_out, gpio_num_t clk_in){
	printf("Setting up GPIOs...\n");
  gpio_config_t io_conf;

  uint64_t output_mask = 1 << data_out; 

  io_conf.intr_type= GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = output_mask;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

  esp_err_t err = gpio_config(&io_conf);


  uint64_t input_mask = 1 << clk_in; 
  
  io_conf.intr_type = GPIO_INTR_POSEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = input_mask;
  // TODO: Revise. For now, we leave pulling up and down to the amp
  io_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;

  // ESP_ERROR_CHECK(gpio_config(&io_conf));
  gpio_config(&io_conf);

  // We want to disable the interrupt for this after it is triggered for as long as we are reading data.
  // After that we will enable it again. (ESP_INTR_FLAG_INTRDISABLED)
  // gpio_install_isr_service( ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_EDGE | ESP_INTR_FLAG_LEVEL1 ) ;
  gpio_isr_handler_add(clk_in, clkinISR, 0);
	printf("GPIOs set up.\n");
}

typedef struct{
  SemaphoreHandle_t sem;
  gpio_num_t dout_pin;
  gpio_num_t clk_in_pin;
} task_test_pulldown_t;

static void test_pulling_down_data(void* p){
  task_test_pulldown_t* arg = static_cast<task_test_pulldown_t*>(p);

  // printf("\nTicks measured: %d\n", ticksMeasured);
  TEST_ASSERT_MESSAGE(ticksMeasured == 0, MESSAGE(Ticks should be 0)); 

  // vTaskDelay(1);
  gpio_set_level(arg->dout_pin, 0);
  vTaskDelay(500 / portTICK_PERIOD_MS);


  xSemaphoreGive(arg->sem);
  vTaskDelete(NULL);
}

static void test_pulling_down_data_impl(){
  LoadCellAmp loadCellAmp(GPIO_NUM_16, GPIO_NUM_17);
  ticksMeasured = 0;

  const gpio_num_t DATA_OUT_PIN = GPIO_NUM_18;
  const gpio_num_t CLK_IN_PIN   = GPIO_NUM_19;

  setupGPIO(DATA_OUT_PIN, CLK_IN_PIN);
  gpio_set_level(DATA_OUT_PIN, 1);

  const task_test_pulldown_t arg = {
    .sem = xSemaphoreCreateBinary(),
    .dout_pin = DATA_OUT_PIN,
    .clk_in_pin = CLK_IN_PIN
  };

  xTaskCreate(test_pulling_down_data, "", 2048, (void*) &arg, 3, 0);
  TEST_ASSERT(xSemaphoreTake(arg.sem, 1000 / portTICK_PERIOD_MS));

  // printf("\nTicks measured: %d\n", ticksMeasured);
  TEST_ASSERT_MESSAGE(ticksMeasured == 27, MESSAGE(Ticks should be exactly 27)); 

  vSemaphoreDelete(arg.sem);
}

TEST_CASE("Pulling down the data line starts the clock which ticks for 27 ticks", ""){
  test_pulling_down_data_impl();
}


static void task_data_should_be_correct(void* p){
  task_test_pulldown_t* arg = static_cast<task_test_pulldown_t*>(p);

  // printf("\nTicks measured: %d\n", ticksMeasured);
  TEST_ASSERT_MESSAGE(ticksMeasured == 0, MESSAGE(Ticks should be 0)); 

  // vTaskDelay(1);
  gpio_set_level(arg->dout_pin, 0);
  
  // vTaskDelay(1);

  gpio_set_level(arg->dout_pin, 1);

  vTaskDelay(1000 / portTICK_PERIOD_MS);


  xSemaphoreGive(arg->sem);
  vTaskDelete(NULL);
}

static void test_data_should_be_correct_impl(){
  LoadCellAmp loadCellAmp(GPIO_NUM_16, GPIO_NUM_17);
  ticksMeasured = 0;

  const gpio_num_t DATA_OUT_PIN = GPIO_NUM_18;
  const gpio_num_t CLK_IN_PIN   = GPIO_NUM_19;

  setupGPIO(DATA_OUT_PIN, CLK_IN_PIN);
  gpio_set_level(DATA_OUT_PIN, 1);

  const task_test_pulldown_t arg = {
    .sem = xSemaphoreCreateBinary(),
    .dout_pin = DATA_OUT_PIN,
    .clk_in_pin = CLK_IN_PIN
  };

  xTaskCreate(task_data_should_be_correct, "", 2048, (void*) &arg, 3, 0);
  TEST_ASSERT(xSemaphoreTake(arg.sem, 1000 / portTICK_PERIOD_MS));

  // printf("\nTicks measured: %d\n", ticksMeasured);
  TEST_ASSERT_MESSAGE(ticksMeasured == 27, MESSAGE(Ticks should be exactly 27)); 


  printf("\nValue: %x\n", loadCellAmp.getLatestReading());
  TEST_ASSERT_MESSAGE(loadCellAmp.getLatestReading() == 0xFFFFFF, MESSAGE(Value should be 0xFFFFFF));

  ticksMeasured = 0;

  xTaskCreate(test_pulling_down_data, "", 2048, (void*) &arg, 3, 0);
  TEST_ASSERT(xSemaphoreTake(arg.sem, 1000 / portTICK_PERIOD_MS));

  // printf("\nTicks measured: %d\n", ticksMeasured);
  printf("\nValue: %x\n", loadCellAmp.getLatestReading());
  TEST_ASSERT_MESSAGE(ticksMeasured == 0, MESSAGE(Now Value should be 0)); 

  vSemaphoreDelete(arg.sem);
}

static void task_integration_test(void* p){
  task_test_pulldown_t* arg = static_cast<task_test_pulldown_t*>(p);

  // printf("\nTicks measured: %d\n", ticksMeasured);
  TEST_ASSERT_MESSAGE(ticksMeasured == 0, MESSAGE(Ticks should be 0)); 

  // vTaskDelay(1);
  gpio_set_level(arg->dout_pin, 0);
  
  // vTaskDelay(1);

  gpio_set_level(arg->dout_pin, 1);

  vTaskDelay(200 / portTICK_PERIOD_MS);


  xSemaphoreGive(arg->sem);
  vTaskDelete(NULL);
}


TEST_CASE("Data should be correct", ""){
  test_data_should_be_correct_impl();
}

static void simple_integration_test_data_should_not_be_zero_impl(){
  LoadCellAmp loadCellAmp(GPIO_NUM_16, GPIO_NUM_17);
  loadCellAmp.oneshot = true;

  ticksMeasured = 0;

  const gpio_num_t DATA_OUT_PIN = GPIO_NUM_18;
  const gpio_num_t CLK_IN_PIN   = GPIO_NUM_19;

  setupGPIO(DATA_OUT_PIN, CLK_IN_PIN);
  gpio_set_level(DATA_OUT_PIN, 1);

  const task_test_pulldown_t arg = {
    .sem = xSemaphoreCreateBinary(),
    .dout_pin = DATA_OUT_PIN,
    .clk_in_pin = CLK_IN_PIN
  };

  xTaskCreate(task_integration_test, "", 2048, (void*) &arg, 3, 0);
  TEST_ASSERT(xSemaphoreTake(arg.sem, 400 / portTICK_PERIOD_MS));

  printf("\nValue: %x\n", loadCellAmp.getLatestReading());
  TEST_ASSERT_MESSAGE(loadCellAmp.getLatestReading() != 0x0, MESSAGE(Value should not be 0x0));

  vSemaphoreDelete(arg.sem);
}

TEST_CASE("Integration test: Wire the amp: DAT - Pin16, CLK - Pin17, rest obvious.", ""){
  simple_integration_test_data_should_not_be_zero_impl();
}
