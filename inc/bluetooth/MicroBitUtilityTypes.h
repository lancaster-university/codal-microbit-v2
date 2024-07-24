/*
The MIT License (MIT)

This header has been written for the Microbit Educational Foundation.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#ifndef MICROBIT_UTILITY_TYPES_H
#define MICROBIT_UTILITY_TYPES_H

#include <stdint.h>

/* Define types for the MicroBitUtilityService
 *
 * request_t  - Request sent from client to service.
 *              job   (1 byte)          - see below
 *              type  (1 byte)          - specifies the request type
 *              data  (up to 18 bytes)  - depends on request type
 *
 * reply_t    - Reply from service to client.
 *              job   (1 byte)          - see below
 *              data  (up to 19 bytes)  - depends on request type
 *
 * job        - Synchronize requests and replies.
 *              Enables the client to ignore replies to a previous request, and detect missing reply packets.
 *              Client cycles high nibble i.e. { 0x00, 0x10, 0x20, ..., 0xF0, 0x00, ...}
 *              Service reply cycles low nibble, i.e client job + { 0x00, 0x01, 0x02, ..., 0x0E, 0x00, ... }
 *              low nibble == 0x0F (jobLowERR) in a reply indicates error (data = 4 bytes signed integer error)
 *
 * Request types and data contents:
 *
 * type 1     - Log file length
 * request      format    (1 byte)          - 0 = HTML header; 1 = HTML; 2 = CSV
 * reply        length    (4 bytes)         - unsigned integer log data length
 *
 * type 2     - Log file data
 * request      format    (1 byte)          - 0 = HTML header; 1 = HTML; 2 = CSV
 *              reserved  (1 byte)          - set to zero
 *              index     (4 bytes)         - unsigned integer index into file
 *              batchlen  (4 bytes)         - unsigned size in bytes to return
 *              length    (4 bytes)         - length of whole file, from request type 1 (Log file length)
 * reply        data      (up to 19 bytes)  - 1 or more reply packets, to total batchlen bytes
 *
 */
namespace codal::MicroBitUtility
{
    typedef enum requestType_t
    {
        requestTypeNone,
        requestTypeLogLength,           // reply data = 4 bytes log data length
        requestTypeLogRead              // reply data = up to 19 bytes of log data
    } requestType_t;

    typedef struct request_t
    {
        uint8_t  job;                   // Client cycles high nibble i.e. { 0x00, 0x10, 0x20, ..., 0xF0, 0x00, ...}
        uint8_t  type;                  // requestType_t
        uint8_t  data[18];
    } request_t;
    
    typedef struct reply_t
    {
        uint8_t  job;                   // Service cycles low nibble i.e client job + { 0x00, 0x01, 0x02, ..., 0x0E, 0x00, ... }
                                        // low nibble == 0x0F (jobLowERR) indicates error and data = 4 bytes signed integer error
        uint8_t  data[19];
    } reply_t;
    
    typedef enum requestLogFormat
    {
        requestLogHTMLHeader = 0,
        requestLogHTML = 1,
        requestLogCSV = 2
    } requestLogFormat;

    typedef struct requestLog_t
    {
        uint8_t  job;
        uint8_t  type;                  // requestType_t
        uint8_t  format;                // requestLogFormat
    } requestLog_t;

    typedef struct requestLogRead_t
    {
        uint8_t  job;
        uint8_t  type;                  // requestType_t
        uint8_t  format;                // requestLogFormat
        uint8_t  reserved;              // set to zero
        uint32_t index;                 // index into data
        uint32_t batchlen;              // size in bytes to return
        uint32_t length;                // length of whole file, from requestTypeLogLength
    } requestLogRead_t;
    
    const uint8_t jobLowMAX = 0x0E;
    const uint8_t jobLowERR = 0x0F;     // reply data = 4 bytes signed integer error
};


#endif
