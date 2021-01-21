#include <stdint.h>

template <class GPIO_TYPE>
class LoadCellAmpCommon{
  protected:
    GPIO_TYPE dout_pin;
    GPIO_TYPE sp_clk_pin;
    uint8_t n_pulses = 27;

  private:
    uint8_t counter = 0;
    int32_t latestValue = 0;
    uint32_t currentValue = 0;

    LoadCellAmpCommon(){
    }

  public:
    enum class AmpGain{
      gain128,
      gain64
    };

    void init(
        GPIO_TYPE dout_pin, 
        GPIO_TYPE sp_clk_pin,
        AmpGain gain){
      this->dout_pin = dout_pin;
      this->sp_clk_pin = sp_clk_pin;
      this->n_pulses = (gain == AmpGain::gain128 ? 25 : 27);
    }


    LoadCellAmpCommon(GPIO_TYPE dout_pin, GPIO_TYPE sp_clk_pin, AmpGain gain) : counter(0), latestValue(0), currentValue(0){
      init(dout_pin, sp_clk_pin, gain);
    }

    void isrDataReady(){
      this->counter = 0;
      this->latestValue = static_cast<int32_t>(this->currentValue);
      this->currentValue = 0;
    }

    void isrNewValue(uint8_t value){
      this->currentValue = (this->currentValue << 1) + value;
      this->counter++;
    }

    int32_t getLatestReading(){
      return this->latestValue;
    }

    GPIO_TYPE getDoutPin(){
      return this->dout_pin;
    }

    GPIO_TYPE getSPClkPin(){
      return this->sp_clk_pin;
    }

    uint8_t getCounterValue(){
      return this->counter;
    }

    uint32_t getCurrentValue(){
      return this->currentValue;
    }
};
