#include <stdint.h>

struct LoadCellAmpProtectedMembers{
  uint64_t dout_pin;
  uint64_t sp_clk_pin;
  uint8_t n_pulses = 27;
};

struct LoadCellAmpPrivateMembers{
  uint8_t counter = 0;
  int32_t latestValue = 0;
  uint32_t currentValue = 0;
};

template <class GPIO_TYPE>
class LoadCellAmpCommon{
  protected:
    LoadCellAmpProtectedMembers prot;

  private:
    LoadCellAmpPrivateMembers priv;

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
      this->prot.dout_pin = dout_pin;
      this->prot.sp_clk_pin = sp_clk_pin;
      this->prot.n_pulses = (gain == AmpGain::gain128 ? 25 : 27);
    }


    LoadCellAmpCommon(GPIO_TYPE dout_pin, GPIO_TYPE sp_clk_pin, AmpGain gain){
      init(dout_pin, sp_clk_pin, gain);
    }

    void isrDataReady(){
      this->priv.counter = 0;
      this->priv.latestValue = static_cast<int32_t>(this->priv.currentValue);
      this->priv.currentValue = 0;
    }

    void isrNewValue(uint8_t value){
      this->priv.currentValue = (this->priv.currentValue << 1) + value;
      this->priv.counter++;
    }

    int32_t getLatestReading(){
      return this->priv.latestValue;
    }

    GPIO_TYPE getDoutPin(){
      return static_cast<GPIO_TYPE>(this->prot.dout_pin);
    }

    GPIO_TYPE getSPClkPin(){
      return static_cast<GPIO_TYPE>(this->prot.sp_clk_pin);
    }

    uint8_t getCounterValue(){
      return this->priv.counter;
    }

    uint32_t getCurrentValue(){
      return this->priv.currentValue;
    }

    uint8_t getNumPulses(){
      return this->prot.n_pulses;
    }
};
