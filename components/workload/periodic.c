/**
 * This file contains the worker thread that will do periodic sending
 * of CoAP requests.
*/
#include "workload.h"

void periodicWorker(void* context) {
  otSockAddr *socket = (otSockAddr *) context;

  while (true) {
    sendRequest(Periodic, socket);
    otLogNotePlat("Successfully sent CoAP request.");

    otLogNotePlat(
      "Will wait %d ms before sending next the periodic CoAP request.",
      PERIODIC_WAIT_TIME_MS
    );

    TickType_t lastWakeupTime = xTaskGetTickCount();
    vTaskDelayUntil(&lastWakeupTime, MS_TO_TICKS(PERIODIC_WAIT_TIME_MS));
  }
  return;
}