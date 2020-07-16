#ifndef DEVICESTATE_H_
#define DEVICESTATE_H_

struct DeviceState
{
  // Connected to WiFi
  bool online = false;
};

extern DeviceState globalState;

#endif // DEVICESTATE_H_