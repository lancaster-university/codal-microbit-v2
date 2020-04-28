/**
 * Copyright (c) 2016 - 2019, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// nRF5SDK_mods
// based on nrf_log_backend_uart

#include "sdk_common.h"
#if NRF_MODULE_ENABLED(NRF_LOG) && NRF_MODULE_ENABLED(NRF_LOG_BACKEND_DMESG)
#include "nrf_log_backend_dmesg.h"
#include "nrf_log_backend_serial.h"
#include "nrf_log_internal.h"
#include "nrf_log_ctrl.h"
#include "app_error.h"

#include "CodalDmesg.h"

static uint8_t m_string_buff[NRF_LOG_BACKEND_DMESG_TEMP_BUFFER_SIZE];


extern const nrf_log_backend_api_t nrf_log_backend_dmesg_api;

typedef struct {
    nrf_log_backend_t               backend;
} nrf_log_backend_dmesg_t;

#define NRF_LOG_BACKEND_DMESG_DEF(_name)                         \
    NRF_LOG_BACKEND_DEF(_name, nrf_log_backend_dmesg_api, NULL)


#if defined(NRF_LOG_BACKEND_DMESG_ENABLED) && NRF_LOG_BACKEND_DMESG_ENABLED
NRF_LOG_BACKEND_DMESG_DEF(dmesg_log_backend);
#endif


void nrf_log_backend_dmesg_init(void)
{
    DMESG("nrf_log_backend_dmesg_init");
    
#if defined(NRF_LOG_BACKEND_DMESG_ENABLED) && NRF_LOG_BACKEND_DMESG_ENABLED
    int32_t backend_id = nrf_log_backend_add( &dmesg_log_backend, NRF_LOG_SEVERITY_DEBUG);
    (void)backend_id;
    nrf_log_backend_enable( &dmesg_log_backend);
#endif
}

static void serial_tx(void const * p_context, char const * p_buffer, size_t len)
{
    char b[NRF_LOG_BACKEND_DMESG_TEMP_BUFFER_SIZE + 1];
    memcpy( b, p_buffer, len);
    b[ len] = '\0';
    DMESGN( b);
//#if DEVICE_DMESG_BUFFER_SIZE > 0
//    codal_dmesg_flush();
//#endif
}

static void nrf_log_backend_dmesg_put(nrf_log_backend_t const * p_backend,
                                     nrf_log_entry_t * p_msg)
{
    nrf_log_backend_serial_put(p_backend, p_msg, m_string_buff,
                               NRF_LOG_BACKEND_DMESG_TEMP_BUFFER_SIZE, serial_tx);
}

static void nrf_log_backend_dmesg_flush(nrf_log_backend_t const * p_backend)
{

}

static void nrf_log_backend_dmesg_panic_set(nrf_log_backend_t const * p_backend)
{
}

const nrf_log_backend_api_t nrf_log_backend_dmesg_api = {
        .put       = nrf_log_backend_dmesg_put,
        .flush     = nrf_log_backend_dmesg_flush,
        .panic_set = nrf_log_backend_dmesg_panic_set,
};
#endif //NRF_MODULE_ENABLED(NRF_LOG) && NRF_MODULE_ENABLED(NRF_LOG_BACKEND_DMESG)
