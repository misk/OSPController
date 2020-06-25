#pragma once
#include <Arduino.h>
#include "utils.h"
#include "publishable.h"
#include <PubSubClient.h>
#include <WebServer.h>

struct DBConnection {
  String serv, user, pass, feed;
  PubSubClient client;
  int32_t period = 1000;
  int getPort() const;
  String getEndpoint() const;
};

struct SPoint {
  double v, i, input; bool collapsed;
  String toString() const;
  double p() const { return v * i; }
};

class Solar {
public:
  Solar(String version);
  void setup();
  void loop();
  void sendOutgoingLogs();
  void publishTask();
  void doConnect();
  void applyAdjustment();
  void printStatus();
  void startSweep();
  void doSweepStep();
  bool hasCollapsed() const;
  int getCollapses() const;
  void doUpdate(String url);

  int getBackoff(int period) const;
  void setState(const String state, String reason="");

  const String version_;
  String state_;
  uint8_t pinInvolt_ = 32;
  float inVolt_ = 0;
  double setpoint_ = 0, pgain_ = 0.005, ramplimit_ = 2;
  double currentCap_ = 8.5;
  CircularArray<uint32_t, 32> collapses_;
  int measperiod_ = 200, printPeriod_ = 1000, adjustPeriod_ = 2000;
  int autoSweep_ = 10 * 60; //every 10m
  float vadjust_ = 116.50;
  CircularArray<SPoint, 10> sweepPoints_; //size here is important, larger == more stable setpoint
  String wifiap, wifipass;
  uint32_t ignoreSubsUntil_ = 0;
  int8_t backoffLevel_ = 0;

  PowerSupply psu_;
  WebServer server_;
  Publishable pub_;
  DBConnection db_;
};

#define STATE(x) static constexpr const char* x = #x

struct States {
  STATE(error);
  STATE(off);
  STATE(mppt);
  STATE(sweeping);
  STATE(full_cv);
  STATE(capped);
  STATE(collapsemode);
};
