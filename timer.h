#ifndef Timer_h
#define Timer_h

class Timer {

  private: 
  bool isRunning_ = false;
  bool isAlarm_ = false;
  long time = -1;
  long timer = -1;


  public:
  void start(int timer) {
    isRunning_ = true;
    isAlarm_ = false;
    time = millis();
    this->timer = timer;
  }

  bool isRunning() {
    return isRunning_;
  }

  bool isAlarm() {
    if (isAlarm_) {
      isAlarm_ = false;
      isRunning_ = false;
      return true;
    }
    return false;
  }

  void cancel() {
    isRunning_ = false;
    isAlarm_ = false;
  }

  void loop() {
    if (isRunning_) {
      long lastLoop = millis() - time;
      if (lastLoop > timer) {
        isRunning_ = false;
        isAlarm_ = true;
      } 
    }
  }
};

#endif