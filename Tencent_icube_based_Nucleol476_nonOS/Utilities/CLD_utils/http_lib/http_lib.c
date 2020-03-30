/**
  ******************************************************************************
  * @file    http_lib.c
  * @author  MCD Application Team
  * @brief   HTTP Client library
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "main.h"
#include "http_lib.h"
#include "msg.h"
#include "net_connect.h"

/* Private defines -----------------------------------------------------------*/
#define HTTP_MAX_HOST_SIZE        80      /**< Max length of the http server hostname. */
#define HTTP_MAX_QUERY_SIZE       50      /**< Max length of the http query string. */
#define HTTP_BUFFER_SIZE          1050    /**< Size of the HTTP work buffer. */
#define HTTP_VERSION              "1.1"
#define HTTP_HEADER               "HTTP/"

#define MIN(a,b)        ( ((a)<(b)) ? (a) : (b) )

#define http_malloc malloc
#define http_free   free

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief HTTP internal session context.
 */
typedef struct {
  int32_t sock;
  char hostname[HTTP_MAX_HOST_SIZE];  /**< HTTP server name. */
  int port;                           /**< HTTP port */
  http_proto_t protocol;
  char * certificate;
  bool tls_verify_server;
  char query[HTTP_MAX_QUERY_SIZE];    /**< HTTP query parsed from the URL. */
  bool connection_is_open;            /**< connection status. */
  uint8_t buffer[HTTP_BUFFER_SIZE];   /**< work buffer  */
} http_context_t;

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
uint8_t * http_find_headers(uint8_t * http_message, unsigned int len);

/* Functions Definition ------------------------------------------------------*/

int http_url_parse(char * const host, const int host_max_len, int * const port, bool * tls, char * const query, const int query_max_len, const char * url)
{
  bool has_query = true;
  const char * pc = NULL;

#ifdef URL_TRACE
  msg_debug("url: %s\n", url);
#endif

  memset(host, 0, host_max_len);
  memset(query, 0, query_max_len);

#define HT_PX  "http://"
#define HTS_PX  "https://"

  if (0 == strncmp(url, HTS_PX, strlen(HTS_PX)))
  {
    *tls = true;
    pc = url + strlen(HTS_PX);
  }

  if (0 == strncmp(url, HT_PX, strlen(HT_PX)))
  {
    *tls = false;
    pc = url + strlen(HT_PX);
  }

  if ( pc != NULL )
  {
    char *pHostTailCol = strchr(pc, ':');
    char *pHostTailSla = strchr(pc, '/');
    if ((pHostTailCol <= pHostTailSla) && (pHostTailCol != NULL))
    {
      /*host = */strncpy(host, pc, MIN(pHostTailCol - pc, host_max_len));
      pc = pHostTailCol + 1;
      sscanf(pc, "%d", port);
      pc = strchr(pc, '/');
      if (pc != NULL)
      {
        /* pc++; */ /* don't advance after '/' - we want it */
      } else
      {
        has_query = false;
      }
    } else
    {
      if (pHostTailSla != NULL)
      {
        strncpy(host, pc, MIN(pHostTailSla - pc, host_max_len));
        pc = pHostTailSla /* + 1 */; /* don't advance after '/' - we want it */
      } else
      {
        /*host = */strncpy(host, pc, host_max_len);
        has_query = false;
      }
      *port = (*tls) ? 443 : 80;
    }
    if (has_query)
    {
      /*query = */strncpy(query, pc, query_max_len);
    }

#ifdef URL_TRACE
    msg_debug("http host: %s\n", host);
    msg_debug("http port: %d\n", *port);
    msg_debug("http query: %s\n", query);
#endif

    return HTTP_OK;
  } else
  {
    /* Not an HTTP url. */
    return HTTP_ERR;
  }
}


int http_create_session(http_handle_t * const pHnd, const char *host, int port, http_proto_t protocol)
{
  http_context_t * pCtx = 0;
  int ret = 0;

  pCtx = (http_context_t *) http_malloc(sizeof(http_context_t));
  if (!pCtx)
  {
    msg_error("error malloc pCtx");
    return HTTP_ERR;
  }

  memset(pCtx, 0, sizeof(http_context_t));
  strncpy(pCtx->hostname, host, sizeof(pCtx->hostname));
  pCtx->port = port;
  pCtx->protocol = protocol;
  pCtx->connection_is_open = false;

  bool tls = (pCtx->protocol==HTTP_PROTO_HTTPS);

  pCtx->sock = net_socket(NET_AF_INET, NET_SOCK_STREAM, NET_IPPROTO_TCP);

  if(tls == true)
  {
    ret  = net_setsockopt(pCtx->sock, NET_SOL_SOCKET, NET_SO_SECURE, NULL, 0);
  }

  if ((pCtx->sock < 0) || (ret != NET_OK))
  {
    msg_error("Could not create a socket.\n");
    return HTTP_ERR;
  }

  *pHnd = (http_handle_t) pCtx;
  return HTTP_OK;
}

int http_connect(http_handle_t Hnd)
{
  http_context_t * pCtx = (http_context_t *)Hnd;
  int ret = 0;

  if (!pCtx)
  {
    msg_error("error invalid handle");
    return HTTP_ERR;
  }

  sockaddr_in addr;
  addr.sin_len = sizeof(sockaddr_in);
  if (net_if_gethostbyname(NULL,(sockaddr_t *)&addr, pCtx->hostname) < 0)
  {
    msg_error("Could not find hostname ipaddr %s\n", pCtx->hostname);
    return -1;
  }

  addr.sin_port = net_htons(pCtx->port);
  ret = net_connect( pCtx->sock,  (sockaddr_t *)&addr, sizeof( addr ) );

  if (NET_OK != ret)
  {
    msg_error("Could not open a socket.\n");
  }
  else
  {
    pCtx->connection_is_open = true;
    return HTTP_OK;
  }

  return HTTP_ERR;
}


int http_close(const http_handle_t hnd)
{
  int rc = HTTP_ERR;

  http_context_t * pCtx = (http_context_t *) hnd;
  if (pCtx != NULL)
  {
    int ret = 0;
    ret = net_closesocket(pCtx->sock);
    pCtx->connection_is_open = false;

    if (ret == NET_OK)
    {
      rc = HTTP_OK;
      http_free(pCtx);
    }
    else
    {
      msg_error("Could not close and destroy a socket.\n");
    }
  }

  return rc;
}

int http_sock_setopt(http_handle_t hnd, const int32_t optname, const void * optbuf, size_t optlen)
{
  http_context_t * pCtx = (http_context_t *) hnd;

  return net_setsockopt(pCtx->sock, NET_SOL_SOCKET, optname, optbuf, optlen);
}


int http_create_request(http_request_t request_type, char * host, char * resource, uint8_t * buffer, uint32_t length)
{
  int remaining_size = length;
  int printed_size = 0;
  uint8_t * p = buffer;

  switch(request_type)
  {
  case HTTP_REQUEST_GET:
    {
      printed_size = snprintf((char *)p, remaining_size, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
      p += printed_size;
      remaining_size -= printed_size;
    }
    break;
  case HTTP_REQUEST_POST:
    {
    printed_size = snprintf((char *)p, remaining_size, "POST %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
    p += printed_size;
    remaining_size -= printed_size;
    }
    break;
  case HTTP_REQUEST_PUT:
    printed_size = snprintf((char *)p, remaining_size, "PUT %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
    p += printed_size;
    remaining_size -= printed_size;
    break;
  case HTTP_REQUEST_DELETE:
    printed_size = snprintf((char *)p, remaining_size, "DELETE %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
    p += printed_size;
    remaining_size -= printed_size;
    break;
  default:
    break;
  }
  return printed_size;
}

int http_add_header(uint8_t *buffer, uint32_t buffer_length, char * header, char * value)
{
  int ret = 0;
  uint32_t len = buffer_length;
  uint8_t *body_start = NULL;
  uint8_t *header_start = NULL;

  /* We insert the header just before the body. */
  /* There must be a "\r\n\r\n" in the buffer. */
  body_start = http_find_body(buffer, &len);
  if (body_start != NULL)
  {
    if (body_start != buffer)
    {
      header_start = body_start;
      /* we go back two characters to insert the header after the first "\r\n" of "\r\n\r\n" */
      header_start--;
      header_start--;
      len += 2;
      ret = snprintf((char*)header_start, len, "%s: %s\r\n\r\n", header, value);
      ret = ret + (header_start - buffer);
    }
    else {
      ret = HTTP_ERR;
    }
  } else {
    ret = HTTP_ERR;
  }
  return ret;
}

int http_add_body(uint8_t * buffer, uint32_t buffer_length, uint8_t *body, uint32_t body_length)
{
  int ret = 0;
  uint8_t *body_start = NULL;
  uint32_t len = buffer_length;
  uint32_t len_to_copy = body_length;

  body_start = http_find_body(buffer, &len);
  if (len_to_copy > len)
  {
    len_to_copy = len;
  }
  if (body_start != NULL)
  {
    memcpy(body_start, body, len_to_copy);
    ret = (body_start - buffer) + len_to_copy; /* return total length (header + body) */
  } else {
    ret = HTTP_ERR;
  }

  return ret;
}

int http_send(http_handle_t hnd, uint8_t * buffer, uint32_t length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int rc = HTTP_OK;

  /* send request */
  msg_debug("http_send (%lu):\n%.*s\n", length, (int)length, buffer);

  rc = net_send(pCtx->sock, buffer, length, 0);

  msg_debug("net_send() rc = %d\n", rc);

  return rc;
}

int http_recv(http_handle_t hnd, uint8_t * buffer, uint32_t length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int rc = HTTP_OK;

  rc = net_recv(pCtx->sock, buffer, length, 0);

  if(rc == NET_TIMEOUT)
  {
    rc = 0;
  }
  msg_debug("net_recv() rc = %d\n", rc);

  return rc;
}

int http_recv_response(http_handle_t hnd, uint8_t * buffer, uint32_t *buffer_length)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  int received = 0;
  uint8_t *pBody = NULL;
  uint32_t read_offset = 0;
  uint32_t body_length = 0;
  uint32_t content_length = 0;
  uint32_t response_length = 0;

  response_length = *buffer_length;

  do
  {
    received = net_recv(pCtx->sock, buffer + read_offset, *buffer_length - read_offset, 0);

    if(received == NET_TIMEOUT)
    {
      received = 0;
    }

    msg_debug("net_recv() received = %d\n", received);

    if (received >= 0)
    {
      read_offset += received;
      if (pBody == NULL)
      {
        body_length = read_offset;
        pBody = http_find_body(buffer, &body_length);
        if (pBody != NULL)
        {
          content_length = http_content_length(buffer, read_offset);
          response_length = pBody - buffer + content_length;
          msg_debug("buffer=%p pBody=%p content-length=%lu response_length=%lu\n",
                    buffer, pBody, content_length, response_length);
          if (response_length > *buffer_length)
          {
            response_length = *buffer_length;
          }
        }
      }
    } else {
      break;
    }
  } while ((received > 0) && (read_offset < response_length));
  if (received < 0)
  {
    return received;
  }
  else
  {
    *buffer_length = read_offset;
    return HTTP_OK;
  }
}


int http_get(const http_handle_t hnd,
             const char * query,
             const char * additional_headers,
             uint8_t * readbuffer,
             uint32_t *readbuffer_size)
{
  int rc = HTTP_OK;
  int header_size = 0;
  int response_size = 0;
  uint32_t status = 0;
  int additional_headers_length = 0;
  int need_final_crlf = 0;
  http_context_t * pCtx = (http_context_t *) hnd;
  char *connection_header = NULL;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }

  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }

  additional_headers_length = strlen(additional_headers);
  if (additional_headers[additional_headers_length-1] != '\n'
      || additional_headers[additional_headers_length-2] != '\r' )
  {
    need_final_crlf = 1;
  }

  /* format request */
  header_size = snprintf((char *)pCtx->buffer, sizeof(pCtx->buffer),
             "GET %s HTTP/" HTTP_VERSION "\r\n"
             "Host: %s\r\n"
             "%s"
             "%s"
             "\r\n",
             query,
             pCtx->hostname,
             additional_headers,
             need_final_crlf?"\r\n":""
             );
  /* send request */
  msg_debug("sending request (%d):\n%.*s\n", header_size, header_size, pCtx->buffer);

  rc = net_send(pCtx->sock, (uint8_t *) pCtx->buffer, header_size, 0);
  msg_debug("net_send() rc = %d\n", rc);

  if (rc <= 0)
  {
    return rc;
  }
  /* receive response */
  rc = http_recv_response(hnd, readbuffer, readbuffer_size);
  msg_debug("http_recv_response() rc = %d\n", rc);
  if (rc < 0)
  {
    return rc;
  }

  response_size = *readbuffer_size;
#ifdef DEBUG_HTTP_CONTENT
  msg_debug("response (%d)\n%.*s\n", response_size, response_size, readbuffer);
#else
  msg_debug("response: size=%d\n", response_size);
#endif
  if (http_response_validate(readbuffer, response_size) != HTTP_OK)
  {
    msg_debug("incorrect header\n");
    return HTTP_ERR;
  }
  connection_header = http_find_header("connection", readbuffer, response_size);
  if ((connection_header != NULL) && (strncasecmp((char *)connection_header, "close", sizeof("close")-1) == 0))
  {
    msg_debug("found connection: close\n");
    return HTTP_EOF;
  }
  status = http_response_status(readbuffer, response_size);
  if (status < 200 || (status>=300))
  {
    msg_debug("incorrect status code %lu\n", status);
    return HTTP_ERR;
  }
  if (status == 204 /* No Content */)
  {
    msg_debug("status code 204 - No Content\n");
    return HTTP_OK;
  }

  return HTTP_OK;
}


int http_post(const http_handle_t hnd,
              const char * query,
              const char * additional_headers,
              const uint8_t * postbuffer,
              uint32_t postbuffer_size,
              uint8_t *responsebuffer,
              uint32_t *responsebuffer_size
              )
{
  int header_size = 0;
  int len = 0;
  int rc = HTTP_OK;
  int additional_headers_length = 0;
  int need_final_crlf = 0;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }

  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }
  additional_headers_length = strlen(additional_headers);
  if ( additional_headers[additional_headers_length-1] != '\n'
        || additional_headers[additional_headers_length-2] != '\r' )
  {
    need_final_crlf = 1;
  }

  /* format request */
  header_size = snprintf((char*)pCtx->buffer, sizeof(pCtx->buffer),
                 "POST %s HTTP/" HTTP_VERSION "\r\n"
                 "Host: %s\r\n"
                 "Content-Length: %lu\r\n"
                 "%s"
                 "%s"
                 "\r\n",
                 query,
                 pCtx->hostname,
                 postbuffer_size,
                 additional_headers,
                 need_final_crlf?"\r\n":""
               );
  /* append POST body */
  if ((header_size + postbuffer_size) > sizeof(pCtx->buffer))
  {
    msg_debug("postbuffer_size too big (%lu) - truncated to %d\n", postbuffer_size, sizeof(pCtx->buffer) - header_size);
    postbuffer_size = sizeof(pCtx->buffer) - header_size;
  }
  memcpy(pCtx->buffer + header_size, postbuffer, postbuffer_size);
  msg_debug("post buffer (%lu):\n%.*s\n", header_size + postbuffer_size,
            (int)(header_size + postbuffer_size), pCtx->buffer);
  /* send request */
  len = net_send(pCtx->sock, (uint8_t *) pCtx->buffer, header_size + postbuffer_size, 0);
  msg_debug("post net_send: len=%d\n", len);
  if (len < 0)
  {
    return len;
  }

  /* receive response */
  rc = http_recv_response(hnd, responsebuffer, responsebuffer_size);
  msg_debug("http_recv_response() rc = %d\n", rc);

  return rc;
}


int http_put(const http_handle_t hnd,
              const char * resource,
              const char * additional_headers,
              const uint8_t * putbuffer,
              uint32_t putbuffer_size,
              uint8_t *responsebuffer,
              uint32_t *responsebuffer_size
              )
{
  int header_size = 0;
  int len = 0;
  int rc = HTTP_OK;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }

  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }

  /* format request */
  header_size = snprintf((char*)pCtx->buffer, sizeof(pCtx->buffer),
               "PUT %s HTTP/" HTTP_VERSION "\r\n"
               "Host: %s\r\n"
               "Content-Length: %lu\r\n"
               "%s"
               "\r\n",
               resource,
               pCtx->hostname,
               putbuffer_size,
               additional_headers
             );
  /* append PUT body */
  if ((header_size + putbuffer_size) > sizeof(pCtx->buffer))
  {
    msg_debug("putbuffer_size too big (%lu) - truncated to %d\n", putbuffer_size, sizeof(pCtx->buffer) - header_size);
    putbuffer_size = sizeof(pCtx->buffer) - header_size;
  }
  memcpy(pCtx->buffer + header_size, putbuffer, putbuffer_size);
  /* send request */
  msg_debug("sending request (%lu):\n%.*s\n", header_size + putbuffer_size, (int)(header_size + putbuffer_size), pCtx->buffer);
  len = net_send(pCtx->sock, (uint8_t *) pCtx->buffer, header_size + putbuffer_size, 0);
  msg_debug("post net_send: len=%d\n", len);
  if (len < 0)
  {
    return len;
  }
  /* receive response */
  rc = http_recv_response(hnd, responsebuffer, responsebuffer_size);
  msg_debug("http_recv_response() rc = %d\n", rc);

  return rc;
}


int http_delete(const http_handle_t hnd, const char * resource, const char * additional_headers,
              uint8_t *responsebuffer, uint32_t *responsebuffer_size)
{
  int header_size = 0;
  int len = 0;
  http_context_t * pCtx = (http_context_t *) hnd;

  if (!pCtx)
  {
    msg_error("NULL handle");
    return HTTP_ERR;
  }

  if (!http_is_open(hnd))
  {
    msg_error("connection not open");
    return HTTP_ERR;
  }

  /* format request */
  header_size = snprintf((char*)pCtx->buffer, sizeof(pCtx->buffer),
               "DELETE %s HTTP/" HTTP_VERSION "\r\n"
               "Host: %s\r\n"
               "%s"
               "\r\n",
               resource,
               pCtx->hostname,
               additional_headers
             );
  /* send request */
  len = net_send(pCtx->sock, pCtx->buffer, header_size, 0);
  msg_debug("delete net_send: len=%d\n", len);
  if (len < 0)
  {
    return len;
  }
  /* receive response */
  len = net_recv(pCtx->sock, responsebuffer, *responsebuffer_size, 0);

  if(len == NET_TIMEOUT)
  {
    len = 0;
  }
  msg_debug("delete net_recv: len=%d\n", len);

  *responsebuffer_size = len;

  if (len < 0)
  {
    return len;
  } else {
    return HTTP_OK;
  }
}

/**
 * @brief   Tells whether an HTTP session is still open, or has been closed by the server.
 * @param   In: hnd   Session handle.
 * @retval  true:   The session is open.
 *          false:  The session is closed: the handle should be freed by calling http_close().
 */
bool http_is_open(const http_handle_t hnd)
{
  http_context_t * pCtx = (http_context_t *) hnd;
  if (pCtx)
  {
    return pCtx->connection_is_open;
  } else {
    return false;
  }
}

/**
  * @brief  find the body part in an HTTP message
  * @arg    http_message: pointer to full HTTP message
  * @arg    len: pointer to HTTP message length. It will contain body length in return.
  * @retval pointer to body in HTTP message
  */
uint8_t * http_find_body(uint8_t * http_message, uint32_t *len)
{
  uint8_t *p = NULL;

  p = http_message;
  while ((*len) >= 4)
  {
    /* the HTTP message body is after an empty line (CR-LF-CR-LF) */
    if (p[0] == '\r'
        && p[1] == '\n'
        && p[2] == '\r'
        && p[3] == '\n')
    {
      *len = (*len) - 4;
      return p+4;
    }
    p++;
    (*len)--;
  }
  *len = 0;
  /* if not found, return NULL */
  return NULL;
}

/**
  * @brief  return the status code from an HTTP response
  * @arg    http_response: pointer to full HTTP response
  * @arg    len: HTTP response length.
  * @retval HTTP status code ( 200 = success, 404 = not found ... )
  */
uint32_t http_response_status(uint8_t * http_response, uint32_t length)
{
  uint32_t status = 0;
  char string[4];

  if (memcmp(http_response, HTTP_HEADER, sizeof(HTTP_HEADER)-1) != 0)
  {
    return 0; /* incorrect buffer */
  }

  string[0] = http_response[9]; /* after HTTP/1.1<space> */
  string[1] = http_response[10];
  string[2] = http_response[11];
  string[3] = 0;

  status = atoi(string);

  return status;
}

/**
  * @brief  check response format
  * @arg    http_response: pointer to full HTTP response
  * @arg    len: HTTP response length.
  * @retval
  */
int http_response_validate(uint8_t * http_response, uint32_t length)
{
  uint32_t status = HTTP_OK;

  if (memcmp(http_response, HTTP_HEADER, sizeof(HTTP_HEADER)-1) != 0)
  {
    return HTTP_ERR; /* incorrect header */
  }

  return status;
}

uint8_t * http_find_headers(uint8_t * http_message, unsigned int len)
{
  uint8_t *p = NULL;
  unsigned int length = 0;

  for( p = http_message, length = len;
       (length > 0) && (*p != '\r') && (*p != '\n');
       p++, length-- )
  {
  }
  if (*p == '\r')
  {
    p++;
    if (*p == '\n')
    {
      p++;
    }
    return p;
  } else if (*p == '\n')
  {
    return p;
  } else {
    return NULL;
  }
}

char *http_find_header(char * header, uint8_t * buffer, uint32_t buffer_length)
{
  char *p = NULL;
  unsigned int remaining_length = 0;
  unsigned int header_length = 0;

  header_length = strlen(header);
  p = (char *)http_find_headers(buffer, buffer_length);
  remaining_length = buffer_length;
  for( ;
       (p != NULL) && (remaining_length > header_length);
       p++, remaining_length--)
  {
    if (memcmp(p, "\r\n\r\n", 4) == 0)
    {
      /* end of headers */
      return NULL;
    }
    if ((strncasecmp((char *)p, header, header_length) == 0)
        && (p[header_length] == ':')
        && (p[header_length+1] == ' ') )
    {
      return p + header_length + 2; /* "header: " */
    }
  }
  return NULL;
}

uint32_t http_content_length(uint8_t * buffer, uint32_t len)
{
  char *p = NULL;

  p = http_find_header("Content-Length", buffer, len);
  if (p != NULL)
  {
    return atoi((char*)p);
  }
  return 0;
}

int http_content_range(uint8_t * buffer, uint32_t len, uint32_t *pRangeStart, uint32_t *pRangeEnd, uint32_t *pRessourceSize)
{
  char *p = NULL;
  int ret = HTTP_ERR;

  p = http_find_header("content-range", buffer, len);
  if (p != NULL)
  {
    if (3 != sscanf((char const *)p, "bytes %lu-%lu/%lu", pRangeStart, pRangeEnd, pRessourceSize))
    {
      uint32_t resource_size = 0;
      if (1 != sscanf((char const *)p, "bytes */%lu", &resource_size))
      {
        ret = HTTP_ERR;
        msg_error("Could not parse the HTTP Content-range header\n");
      }
      else
      { /* Out of range request */
        msg_error("Out of range request: %lu-%lu / %lu\n", *pRangeStart, *pRangeEnd, resource_size);
        msg_error("Previous range status: %lu-%lu / %lu\n", *pRangeStart, *pRangeEnd, *pRessourceSize);
        ret = HTTP_ERR;
      }
    }
    else
    {
      msg_debug("returning with range status: %lu-%lu / %lu\n", *pRangeStart, *pRangeEnd, *pRessourceSize);
      ret = HTTP_OK;
    }
  }
  return ret;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
