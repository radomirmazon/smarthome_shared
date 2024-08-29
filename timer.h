#ifndef Timer_h
#define Timer_h

class Timer {

  private: 
  bool isRunning = false;
  bool isAlarm_ = false;
  long time = -1;
  long timer = -1;


  public:
  void start(int timer) {
    isRunning = true;
    isAlarm_ = false;
    time = millis();
    this->timer = timer;
  }

  bool isAlarm() {
    if (isAlarm_) {
      isAlarm_ = false;
      isRunning = false;
      return true;
    }
    return false;
  }

  void cancle() {
    isRunning = false;
    isAlarm_ = false;
  }

  void loop() {
    if (isRunning) {
      long lastLoop = millis() - time;
      if (lastLoop > timer) {
        isRunning = false;
        isAlarm_ = true;
      } 
    }
  }
};

#endif