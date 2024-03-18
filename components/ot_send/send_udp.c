#include "ot_send.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>

otError handleError(otError error) {
  if (error != OT_ERROR_NONE) {
    ERROR_PRINT(otLogCritPlat("%s", otThreadErrorToString(error)));
  }
  return error;
}

void handleMessageError(otMessage *aMessage, otError error) {
  if (handleError(error) != OT_ERROR_NONE) {
    otMessageFree(aMessage);
  }
  return;
}

void udpCreateSocket(otUdpSocket *aSocket,
                      otInstance *aInstance,
                      otSockAddr *aSockName)
{
  handleError(otUdpOpen(aInstance, aSocket, NULL, NULL));
  handleError(otUdpBind(aInstance, aSocket, aSockName, OT_NETIF_THREAD));
  return;
}

uint32_t udpAttachPayload(otMessage *aMessage, nvs_handle_t counterHandle) {
  uint32_t count;
  ESP_ERROR_CHECK(nvs_get_u32(counterHandle, OT_SEND_CTR_KEY, &count));

  char payload[PAYLOAD_SIZE];
  EmptyMemory(payload, PAYLOAD_SIZE);

  sprintf(payload, "Packet number %" PRIu32 "", count);
  count += 1;
  ESP_ERROR_CHECK(nvs_set_u32(counterHandle, OT_SEND_CTR_KEY, count));

  otError error = otMessageAppend(aMessage, payload, PAYLOAD_SIZE);
  handleMessageError(aMessage, error);

  return count - 1;
}

void udpTransmitMessage(otInstance *aInstance,
                        uint16_t port,
                        uint16_t destPort,
                        otUdpSocket *aSocket,
                        otMessageInfo *aMessageInfo,
                        nvs_handle_t counterHandle)
{
  otMessage *aMessage = otUdpNewMessage(aInstance, NULL);

  uint32_t count = udpAttachPayload(aMessage, counterHandle);
  otLogNotePlat("Sent UDP packet %" PRIu32 "", count);

  otError error = otUdpSend(aInstance, aSocket, aMessage, aMessageInfo);
  handleMessageError(aMessage, error);
  return;
}

void udpSend(otInstance *aInstance,
             uint16_t port,
             uint16_t destPort,
             otSockAddr *aSockName,
             otUdpSocket *aSocket,
             nvs_handle_t counterHandle)
{
  checkConnection(aInstance);

  aSockName->mAddress = *otThreadGetMeshLocalEid(aInstance);
  aSockName->mPort = port;
  udpCreateSocket(aSocket, aInstance, aSockName);

  otMessageInfo aMessageInfo;
  aMessageInfo.mSockAddr = aSockName->mAddress;
  aMessageInfo.mSockPort = port;
  aMessageInfo.mPeerPort = destPort;
  aMessageInfo.mHopLimit = 0;  // default
  otIp6Address *peerAddr = &(aMessageInfo.mPeerAddr);
  handleError(otIp6AddressFromString(RECEIVER_ADDRESS, peerAddr));

  udpTransmitMessage(aInstance, port, destPort, aSocket,
                     &aMessageInfo, counterHandle);
  return;
}