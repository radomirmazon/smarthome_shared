#ifndef Switch_Button_h
#define Switch_Button_h

#include "timer.h"
#include "base_device.h"

#define DEBOUNCE_TIME 50
#define DEAD_ZONE_TIME 350

class ChangeDetector{
  public:
  virtual void stateChanged(bool state)=0;
};


class SwitchButton {

    private:
    Timer debounceTimer;
    Timer deadZoneTimer;

    uint8_t clickCounter = 0;

    bool internalState = false;
    bool debounceState = false;
    bool bell = false;
    bool invertState = false;
    ChangeDetector* pDevice;

    public:
    SwitchButton(ChangeDetector* pDevice, bool initState, bool invertState, bool bell) {
        this->pDevice = pDevice;
        internalState = initState;
        this->bell = bell;
        this->invertState = invertState;
        pDevice->stateChanged(internalState);
    }

    void begin(){
    }

    void loop(){
        debounceTimer.loop();
        deadZoneTimer.loop();

        if (debounceTimer.isAlarm()) {
            if (debounceState != internalState) {
                detectedNewState();
            }
        }

        if (deadZoneTimer.isAlarm() && clickCounter > 0) {
            Serial.print("Detect clicks: ");
            Serial.println(clickCounter);
            clickCounter = 0;
        }
    }

    void setState(bool state) {
        debounceState = state;
        if (!debounceTimer.isRunning() && internalState != state) {
            debounceTimer.start(DEBOUNCE_TIME);
        }
    }

    private:
    void detectedNewState() {
        bool logicalStateBefore = getLogicalState();
        internalState = debounceState;
        bool logicalStateAfter = getLogicalState();
        bool isRisingEdgeLogicalState = !logicalStateBefore && logicalStateAfter;

        //if (!bell) {
        //    switchProcessing();
        //} else {
            bellProcessing(isRisingEdgeLogicalState);
        //}    
    }

    void switchProcessing() {
        pDevice->stateChanged(getLogicalState());
    }

    void bellProcessing(bool isRisingEdgeLogicalState) {

        if (!deadZoneTimer.isRunning() && isRisingEdgeLogicalState) {
            deadZoneTimer.start(DEAD_ZONE_TIME);
        }

        if (deadZoneTimer.isRunning() && isRisingEdgeLogicalState) {
            if (clickCounter<3) {
                clickCounter++;
            }
            deadZoneTimer.start(DEAD_ZONE_TIME);
        }
        
    }

    bool getLogicalState() {
        return internalState ^ invertState;
    }

};

#endif