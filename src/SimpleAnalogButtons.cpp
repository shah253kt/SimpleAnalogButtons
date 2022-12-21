#include "SimpleAnalogButtons.h"

#if defined(ARDUINO_STM_NUCLEO_F103RB) || defined(ARDUINO_GENERIC_STM32F103C)
SimpleAnalogButtons::SimpleAnalogButtons(uint8_t pin, WiringPinMode mode, uint16_t minValidValue, uint16_t samplingInterval)
#else
SimpleAnalogButtons::SimpleAnalogButtons(uint8_t pin, uint8_t mode, uint16_t minValidValue, uint16_t samplingInterval)
#endif
    : mPin(pin),
      mMinValidValue(minValidValue),
      mSamplingInterval(samplingInterval),
      mButtonCount(0),
      mSampleCount(0),
      mSumOfValues(0),
      mPrevButtonState(false),
      mStartPollAt(0),
      mIndexOfLastButtonPressed(-1),
      mIndexOfLastButtonHeld(-1),
      mHoldDuration(ANALOG_BUTTON_DEFAULT_HOLD_DURATION)
{
    pinMode(mPin, mode);
}

SimpleAnalogButtons::~SimpleAnalogButtons()
{
    delete[] buttonRange;
}

void SimpleAnalogButtons::addButton(uint16_t minMean, uint16_t maxMean)
{
    buttonRange[mButtonCount++].setRange(minMean, maxMean);
}

void SimpleAnalogButtons::setSamplingInterval(uint16_t samplingInterval)
{
    mSamplingInterval = samplingInterval;
}

void SimpleAnalogButtons::setPressedCallbackFunction(void (*func)(uint8_t))
{
    mPressedCallbackFunction = func;
}

void SimpleAnalogButtons::setHeldCallbackFunction(void (*func)(uint8_t))
{
    mHeldCallbackFunction = func;
}

void SimpleAnalogButtons::setHoldDuration(uint16_t duration)
{
    mHoldDuration = duration;
}

uint16_t SimpleAnalogButtons::readRaw()
{
    return analogRead(mPin);
}

void SimpleAnalogButtons::update()
{
    uint16_t currentButtonValue = readRaw();
    bool currentButtonState = currentButtonValue >= mMinValidValue;

    if (!currentButtonState) // No button is pressed
    {
        mSampleCount = 0;
        mSumOfValues = 0;
        mPrevButtonState = currentButtonState;
        mIndexOfLastButtonPressed = -1;
        mIndexOfLastButtonHeld = -1;
        return;
    }

    if (!mPrevButtonState) // A button was just pressed.
    {
        mStartPollAt = millis();
    }

    mPrevButtonState = currentButtonState;

    if (millis() - mStartPollAt < mSamplingInterval) // Still within polling interval
    {
        mSampleCount++;
        mSumOfValues += currentButtonValue;
        return;
    }

    uint16_t meanValue = mSumOfValues / mSampleCount;

    for (uint8_t i = 0; i < mButtonCount; i++)
    {
        if (buttonRange[i].inRange(meanValue))
        {
            if (i != mIndexOfLastButtonPressed && mPressedCallbackFunction)
            {
                mIndexOfLastButtonPressed = i;
                (*mPressedCallbackFunction)(i);
            }

            if (i != mIndexOfLastButtonHeld && millis() - mStartPollAt >= mHoldDuration && mHeldCallbackFunction)
            {
                mIndexOfLastButtonHeld = i;
                (*mHeldCallbackFunction)(i);
            }

            return;
        }
    }

    mIndexOfLastButtonPressed = -1;
    mIndexOfLastButtonHeld = -1;
}
