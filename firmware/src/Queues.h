#ifndef QUEUES_H_
#define QUEUES_H_

#include <Arduino.h>

QueueHandle_t xQueueScannedDevices;
QueueHandle_t xQueueSendResults;

void setupQueues();

#endif // QUEUES_H_