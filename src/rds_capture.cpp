#include "rds_capture.h"

static String capturedPS[RDS_CAPTURE_MAX];
static int capturedPSCount = 0;
static String capturedRT[RDS_CAPTURE_MAX];
static int capturedRTCount = 0;

void addCapturedPS(const String& s) {
  if (s.length() == 0) return;
  String su = s; su.trim();
  if (su.length() == 0) return;
  for (int i = 0; i < capturedPSCount; i++)
    if (capturedPS[i] == su) return;
  if (capturedPSCount < RDS_CAPTURE_MAX) {
    capturedPS[capturedPSCount++] = su;
  } else {
    for (int i = 0; i < RDS_CAPTURE_MAX - 1; i++) capturedPS[i] = capturedPS[i + 1];
    capturedPS[RDS_CAPTURE_MAX - 1] = su;
  }
}

void addCapturedRT(const String& s) {
  if (s.length() == 0) return;
  String su = s; su.trim();
  if (su.length() == 0) return;
  for (int i = 0; i < capturedRTCount; i++)
    if (capturedRT[i] == su) return;
  if (capturedRTCount < RDS_CAPTURE_MAX) {
    capturedRT[capturedRTCount++] = su;
  } else {
    for (int i = 0; i < RDS_CAPTURE_MAX - 1; i++) capturedRT[i] = capturedRT[i + 1];
    capturedRT[RDS_CAPTURE_MAX - 1] = su;
  }
}

String getCapturedPS(int i) { return (i >= 0 && i < capturedPSCount) ? capturedPS[i] : ""; }
String getCapturedRT(int i) { return (i >= 0 && i < capturedRTCount) ? capturedRT[i] : ""; }
int getCapturedPSCount() { return capturedPSCount; }
int getCapturedRTCount() { return capturedRTCount; }
