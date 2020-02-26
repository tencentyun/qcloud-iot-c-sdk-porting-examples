#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_NET_C)


#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/platform.h"
#else
#include <stdlib.h>
#endif

#include "mbedtls/net_sockets.h"
#include "at_socket_inf.h"
#include "qcloud_iot_import.h"
#include "utils_timer.h"
#include "qcloud_iot_export_error.h"
#include "qcloud_iot_export_log.h"
#include "network_interface.h"


void mbedtls_net_init(mbedtls_net_context* ctx)
{
    int rc;

    ctx->fd = -1;

    /*at device init entry: at_client init, device driver register to at_socket*/
    rc = at_device_init();
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("at device init fail,rc:%d", rc);
    }

    /*do after at device init*/
    rc |= at_socket_init();
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("at socket init fail,rc:%d", rc);
    }
}

int mbedtls_net_connect(mbedtls_net_context *ctx, const char* host, const char* port, int proto)
{
    int fd = at_socket_connect(host, atoi(port), eNET_TCP);

    if (fd < 0) {
        Log_e("fail to connect with TCP server: %s:%u", host, atoi(port));
        ctx->fd  = AT_NO_CONNECTED_FD;
        return -1;
    } else {
        ctx->fd  = fd;
        return 0;
    }
}

int mbedtls_net_set_block( mbedtls_net_context *ctx )
{
    //do nothing
    return 0;
}

int mbedtls_net_recv(void* ctx, unsigned char* buf, size_t len)
{
    IOT_FUNC_ENTRY;

    int fd = ((mbedtls_net_context *) ctx)->fd;

    int ret;
    uint32_t len_recv = 0;
    do {
        ret = at_socket_recv(fd, buf + len_recv, len - len_recv);
        if (ret > 0) {
            len_recv += ret;
        } else if (ret == 0) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        } else { //ret < 0
            Log_e("recv fail\n");
            IOT_FUNC_EXIT_RC(MBEDTLS_ERR_NET_RECV_FAILED);
        }
        //} while ((len_recv < len));
    } while (0);

    IOT_FUNC_EXIT_RC(len_recv);
}

int mbedtls_net_recv_timeout(void* ctx, unsigned char* buf, size_t len,
                             uint32_t timeout)
{
    IOT_FUNC_ENTRY;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    int ret;
    uint32_t len_recv = 0;
    Timer timer;
    InitTimer(&timer);
    countdown_ms(&timer, timeout);
    do {
        if (expired(&timer)) {
            IOT_FUNC_EXIT_RC(MBEDTLS_ERR_SSL_TIMEOUT);
        }
        ret = at_socket_recv(fd, buf + len_recv, len - len_recv);
        if (ret > 0) {
            len_recv += ret;
        } else if (ret == 0) {
            return MBEDTLS_ERR_SSL_WANT_READ;
        } else { //ret < 0
            Log_e("recv fail\n");
            IOT_FUNC_EXIT_RC(MBEDTLS_ERR_NET_RECV_FAILED);
        }
    } while ((len_recv < len));

    IOT_FUNC_EXIT_RC(len_recv);
}

int mbedtls_net_send(void* ctx, const unsigned char* buf, size_t len)
{
    int fd = ((mbedtls_net_context *) ctx)->fd;

    int ret;
    uint32_t len_sent;

    len_sent = 0;
    ret = 1; /* send one time if timeout_ms is value 0 */

    do {
        ret = at_socket_send(fd, buf + len_sent, len - len_sent);
        if (ret > 0) {
            len_sent += ret;
        } else if (0 == ret) {
            Log_e("No data be sent\n");
            return MBEDTLS_ERR_SSL_WANT_WRITE;
        } else {
            Log_e("send fail, ret:%d\n", ret);
            return MBEDTLS_ERR_NET_SEND_FAILED;
        }
    } while (len_sent < len);

    return len_sent;
}

void mbedtls_net_free(mbedtls_net_context* ctx)
{
    int fd = ((mbedtls_net_context *) ctx)->fd;
    int rc;
    rc = at_socket_close(fd);
    if (QCLOUD_RET_SUCCESS != rc) {
        Log_e("socket close error\n");
    }
}

#endif /* MBEDTLS_NET_C */

