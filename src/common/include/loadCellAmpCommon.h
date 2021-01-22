#include <stdint.h>

template <class GPIO_TYPE>
class LoadCellAmpCommon{
  protected:
    const GPIO_TYPE dout_pin;
    const GPIO_TYPE sp_clk_pin;
    const uint8_t n_pulses = 27;

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

    LoadCellAmpCommon(const GPIO_TYPE dout_pin, const GPIO_TYPE sp_clk_pin, const AmpGain gain) : 
      dout_pin(dout_pin), sp_clk_pin(sp_clk_pin), n_pulses(gain == AmpGain::gain128 ? 25 : 27),
      counter(0), latestValue(0), currentValue(0){
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

    const GPIO_TYPE getDoutPin(){
      return this->dout_pin;
    }

    const GPIO_TYPE getSPClkPin(){
      return this->sp_clk_pin;
    }

    const uint8_t getNumPulses(){
      return this->n_pulses;
    }

    uint8_t getCounterValue(){
      return this->counter;
    }

    uint32_t getCurrentValue(){
      return this->currentValue;
    }
};
