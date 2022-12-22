#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#ifndef ANALOG_BUTTON_MAX_COUNT
#define ANALOG_BUTTON_MAX_COUNT 10
#endif

#ifndef ANALOG_BUTTON_DEFAULT_SAMPLING_INTERVAL
#define ANALOG_BUTTON_DEFAULT_SAMPLING_INTERVAL 50
#endif

#ifndef ANALOG_BUTTON_DEFAULT_HOLD_DURATION
#define ANALOG_BUTTON_DEFAULT_HOLD_DURATION 3000
#endif

#ifndef ANALOG_BUTTON_DEFAULT_MIN_VALID_VALUE
#define ANALOG_BUTTON_DEFAULT_MIN_VALID_VALUE 1024
#endif

class ButtonRange
{
    friend class SimpleAnalogButtons;

private:
    uint16_t mMinMean, mMaxMean;

public:
    ButtonRange()
        : mMinMean(0),
          mMaxMean(1023)
    {
    }

    ButtonRange(uint16_t minMean, uint16_t maxMean)
        : mMinMean(minMean),
          mMaxMean(maxMean)
    {
    }

    void setRange(uint16_t minMean, uint16_t maxMean)
    {
        mMinMean = minMean;
        mMaxMean = maxMean;
    }

    bool inRange(uint16_t mean)
    {
        return mMinMean <= mean && mean <= mMaxMean;
    }
};

class SimpleAnalogButtons
{
private:
    ButtonRange buttonRange[ANALOG_BUTTON_MAX_COUNT];
    int8_t mIndexOfLastButtonPressed, mIndexOfLastButtonHeld;
    uint8_t mPin, mButtonCount;
    uint16_t mMinValidValue, mSamplingInterval, mSampleCount, mHoldDuration;
    uint32_t mSumOfValues, mStartPollAt;
    bool mPrevButtonState;

    void (*mPressedCallbackFunction)(uint8_t buttonIndex);
    void (*mHeldCallbackFunction)(uint8_t buttonIndex);

public:
#if defined(ARDUINO_STM_NUCLEO_F103RB) || defined(ARDUINO_GENERIC_STM32F103C)
    SimpleAnalogButtons(uint8_t pin, WiringPinMode mode = INPUT_ANALOG, uint16_t samplingInterval = ANALOG_BUTTON_DEFAULT_SAMPLING_INTERVAL);
#else
    SimpleAnalogButtons(uint8_t pin, uint8_t mode = INPUT, uint16_t samplingInterval = ANALOG_BUTTON_DEFAULT_SAMPLING_INTERVAL);
#endif

    ~SimpleAnalogButtons();

    bool addButton(uint16_t minMean, uint16_t maxMean);
    void setSamplingInterval(uint16_t samplingInterval);
    void setPressedCallbackFunction(void (*func)(uint8_t));
    void setHeldCallbackFunction(void (*func)(uint8_t));
    void setHoldDuration(uint16_t duration);
    void setMinValidValue(uint16_t minValidValue);
    uint16_t readRaw();
    void update();
    int getPressed();
    int getHeld();
};
