#include <catch2/catch_all.hpp>
#include <loadCellAmpCommon.h>

TEST_CASE("LoadCellAmp class exists and is included correctly and CMake is configure correctly", "") {
  LoadCellAmp<uint8_t> loadCellAmp;
}

TEST_CASE("Test LoadCellAmp intialization"){
  const uint32_t DOUT_PIN = 8;
  const uint32_t SP_CLK_PIN = 9;

  LoadCellAmp<uint32_t> loadCellAmp(DOUT_PIN, SP_CLK_PIN);
  REQUIRE(loadCellAmp.getDoutPin() == 8);
  REQUIRE(loadCellAmp.getSPClkPin() == 9);
}

TEST_CASE("Test if clocked in data works correctly"){
  LoadCellAmp<uint32_t> loadCellAmp;

  REQUIRE(loadCellAmp.getLatestReading() == 0);
  REQUIRE(loadCellAmp.getCurrentValue() == 0);
  REQUIRE(loadCellAmp.getCounterValue() == 0);

  for(int i = 0; i < 24; i++){
    loadCellAmp.isrNewValue(0);
    loadCellAmp.isrDataReady();
  }

  REQUIRE(loadCellAmp.getLatestReading() == 0);
  REQUIRE(loadCellAmp.getCurrentValue() == 0);
  REQUIRE(loadCellAmp.getCounterValue() == 0);

  for(int i = 0; i < 23; i++){
    loadCellAmp.isrNewValue(0);
  }

  loadCellAmp.isrNewValue(1);
  loadCellAmp.isrDataReady();

  REQUIRE(loadCellAmp.getLatestReading() == 1);
  REQUIRE(loadCellAmp.getCurrentValue() == 0);
  REQUIRE(loadCellAmp.getCounterValue() == 0);

  for(int i = 0; i < 24; i++){
    loadCellAmp.isrNewValue(((i % 2) == 0) ? 1 : 0);
    REQUIRE(loadCellAmp.getCounterValue() == i+1);

    if(i == 12){
      REQUIRE(loadCellAmp.getCurrentValue() == 0x1555);
      REQUIRE(loadCellAmp.getLatestReading() == 1);
    }
  }

  loadCellAmp.isrDataReady();

  REQUIRE(loadCellAmp.getLatestReading() == 0xAAAAAA);
  REQUIRE(loadCellAmp.getCurrentValue() == 0);
  REQUIRE(loadCellAmp.getCounterValue() == 0);

  for(int i = 0; i < 26; i++){
    loadCellAmp.isrNewValue(((i % 2) == 0) ? 1 : 0);
  }

  loadCellAmp.isrDataReady();

  REQUIRE(loadCellAmp.getLatestReading() == 0xAAAAAA);
  REQUIRE(loadCellAmp.getCurrentValue() == 0);
  REQUIRE(loadCellAmp.getCounterValue() == 0);
}

TEST_CASE("Test data ready interrupt callback"){
  const uint32_t DOUT_PIN = 8;
  const uint32_t SP_CLK_PIN = 9;

  // Whenever data is ready, an interrupt should notify the driver
  // This resets the counter and the current value
  // And pushes the now finished read to be the current read
  LoadCellAmp<uint32_t> loadCellAmp(DOUT_PIN, SP_CLK_PIN);

  loadCellAmp.isrDataReady();

  REQUIRE(loadCellAmp.getLatestReading() == 0);
  REQUIRE(loadCellAmp.getCurrentValue() == 0);
  REQUIRE(loadCellAmp.getCounterValue() == 0);
}
