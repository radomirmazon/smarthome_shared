#ifndef Switch_Button_h
#define Switch_Button_h

#include "timer.h"
#include "base_device.h"

#define DEBOUNCE_TIME 60

class ChangeDetector{
  public:
  virtual void stateChanged(bool state)=0;
};


class SwitchButton {

    private:
    Timer debounceTimner;

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
        debounceTimner.loop();
        if (debounceTimner.isAlarm()) {
            debounceTimner.cancel();
            if (debounceState != internalState) {
                detectNewState();
            }
        }
    }

    void setState(bool state) {
        debounceState = state;
        if (!debounceTimner.isRunning() && internalState != state) {
            debounceTimner.start(DEBOUNCE_TIME);
        }
    }

    private:
    void detectNewState() {
        internalState = debounceState;
        if (!bell) {
            pDevice->stateChanged(internalState ^ invertState);
        } else {
            
        }    }
};

#endif