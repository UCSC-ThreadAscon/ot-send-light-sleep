#ifndef OT_SEND_
#define OT_SEND_

#include <openthread/logging.h>

#include "esp_err.h"
#include "esp_log.h"
#include "esp_openthread.h"
#include "esp_openthread_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "openthread/cli.h"
#include "openthread/instance.h"
#include "openthread/logging.h"
#include "openthread/tasklet.h"
#include "openthread/udp.h"

#include "nvs_flash.h"
#include "nvs.h"

/**
 * Each send packet will have the following payload:
 * 
 *    "Packet Number [uint_32t]"
 *
 * Substring "Packet Number " makes up 14 bytes.
 *
 * The 32 bit integer will be represented as a string.
 * 2^32 - 1 = 4294967295 has 10 digits, and thus, the
 * string representation of the uint32_t will make up 10 bytes.
 *
 * Thus, the total payload isze is 14 + 10 = 24 bytes.
*/
#define PAYLOAD_SIZE 24

#define MS_TO_TICKS(ms) ms / portTICK_PERIOD_MS
#define MS_TO_MICRO(ms) ms * 1000

#define DEFAULT_WAIT_TIME_MS MS_TO_TICKS(100)
#define MAIN_WAIT_TIME MS_TO_TICKS(5000)
#define PACKET_SEND_DELAY_MS 5000
#define IDLE_DELAY_TICKS MS_TO_TICKS(500)

#define OT_SEND_CTR "ot-send-counter"
#define OT_SEND_CTR_KEY "counter"

#define OT_DISCONNECTED(role) (role == OT_DEVICE_ROLE_DISABLED) || (role == OT_DEVICE_ROLE_DETACHED)

/**
 * The sender will broadcast packets using the multicast address.
 *
 * https://openthread.io/guides/thread-primer/ipv6-addressing#multicast
*/
#define RECEIVER_ADDRESS CONFIG_RECEIVER_ADDRESS

#define UDP_SOCK_PORT 12345
#define UDP_DEST_PORT 54321

/**
 * Empties all memory for `size` bytes starting at memory address `pointer`.
 *
 * @param[in] pointer: the pointer of the stack memory
 * @param[in] size:    the size of the memory that `pointer` points to
 *
 * I got the idea to use `memset()` to clear stack memory from
 * the Google Search AI:
 * https://docs.google.com/document/d/1o-NaEOA-vzWPCv7VX1dONUfwos2epveDk4H_Y2Y5g1Y/edit?usp=sharing
*/
#define EmptyMemory(pointer, size) memset((void *) pointer, 0, size)

void udpSend(otInstance *aInstance,
             uint16_t port,
             uint16_t destPort,
             otSockAddr *aSockName,
             otUdpSocket *aSocket,
             nvs_handle_t counterHandle);

void checkConnection(otInstance *aInstance);

#define DEBUG true
#define DELIMITER "************************************"
#define PRINT_DELIMIER otLogNotePlat(DELIMITER)
#define DEBUG_PRINT(ot_note) PRINT_DELIMIER; ot_note; PRINT_DELIMIER;
#define ERROR_PRINT(ot_error) otLogCritPlat(DELIMITER); ot_error; otLogCritPlat(DELIMITER);

#endif // OT_SEND_