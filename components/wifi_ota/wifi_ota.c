/*
 * wifi_ota.c
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */
#include "wifi_ota.h"
#include "../../sys_conf.h"
#include <esp_http_server.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"

#define FW_BUF_SIZE         5140 //5KB 
#define BUFF_SIZE           1500
#define TEXT_BUFFSIZE       1024

typedef enum ota_status {
  OTA_FW_WAITING,
  OTA_FW_UPLOADING,
  OTA_FW_UPLOAD_COMPLETE,
  OTA_REBOOTING,
  OTA_STATUS_NUM
}ota_status_t;

const char* ota_status_str[OTA_STATUS_NUM] = {
  "Waiting",
  "Uploading",
  "Upload Complete",
  "Rebooting",
};

typedef enum ota_state {
  OTA_INIT,
  OTA_PREPARE,
  OTA_START,
  OTA_RECVED,
  OTA_FINISH,
  OTA_STATE_NUM
}ota_state_t;

typedef struct ota_firm {
    uint8_t             ota_num;
    uint8_t             update_ota_num;
    ota_state_t         state;
    size_t              content_len;
    size_t              read_bytes;
    size_t              write_bytes;
    size_t              ota_size;
    size_t              ota_offset;
    const char          *buf;
    size_t              bytes;
}ota_firm_t;

extern char index_html_start[]                asm("_binary_index_html_start");
extern char index_html_end[]                  asm("_binary_index_html_end");
static httpd_handle_t   _server           = NULL;
static ota_status_t     _status           = OTA_FW_WAITING;
static const char*      _tag              = "WiFi OTA";

static void run_ota_task(void* arg);
static httpd_handle_t start_webserver(void);
static void stop_webserver(httpd_handle_t server);
static void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static esp_err_t root_handler(httpd_req_t* req);
static esp_err_t get_handler(httpd_req_t* req);
static esp_err_t put_handler(httpd_req_t* req);
static esp_err_t post_handler(httpd_req_t* req);

static inline size_t index_html_size(void) { 
  ptrdiff_t bin_size = index_html_end - index_html_start;
  return (size_t) bin_size;
}

static inline int read_until(const char* buffer, char delim, int len) {
  int i = 0;
  while(buffer[i] != delim && i < len) ++i;
  return i + 1;
}

void wifi_ota_start(void) {
  xTaskCreate(run_ota_task, "OTA Task", 4000, NULL, PRIORITY_OTA, NULL);
}

void run_ota_task(void* arg) {
  printf("WiFi OTA Task Started\n");
  // Start HTTP WebServer on port 80
  if(_server == NULL) {
    printf("WiFi OTA: Starting Webserver\n");
    _server = start_webserver();
  }
  esp_err_t                             err;
  esp_ota_handle_t                      update_handle = 0;
  const esp_partition_t*                update_partition = NULL;
  const esp_partition_t*                configured = esp_ota_get_boot_partition();
  const esp_partition_t*                running = esp_ota_get_running_partition();
  if(configured != running) {
    ESP_LOGW(_tag, "Configured OTA boot parition at offset 0x%08x, but running from offse 0x%08x",
                configured->address, running->address);
  }
  ESP_LOGI(_tag, "Running partition type %d subtype %d (offset 0x%08x)",
              running->type, running->subtype, running->address);
  ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &_server)); 
  for(;;) { 
    sys_delay(1);
  }
}

esp_err_t root_handler(httpd_req_t* req) {
  char* buf = (char*) index_html_start;
  size_t buf_size = (size_t) index_html_size();
// printf("root_handler: index_html_start(%p) index_html_size(%zu)\n", index_html_start, index_html_size());
//  for(size_t i = 0; i < buf_size; ++i)
//    printf("%c", index_html_start[i]);
//  printf("\nDone\n");
  int ret = httpd_resp_send(req, buf, buf_size);
  return ret;
}

esp_err_t get_handler(httpd_req_t* req) {
  char* buf;
  const char* resp = ota_status_str[_status];
  httpd_resp_send(req, resp, strlen(resp));
  return ESP_OK;
} 

esp_err_t put_handler(httpd_req_t* req) {
  char buf;
  int ret;
  if(( ret = httpd_req_recv(req, &buf, 1)) <= 0) {
    if(ret == HTTPD_SOCK_ERR_TIMEOUT) {
      httpd_resp_send_408(req);
    }
    return ESP_FAIL;
  }
  if( buf != '0' ) {
    printf("buf %c \n", buf);
  }
  httpd_resp_send(req, NULL, 0);
  return ESP_OK;
}

esp_err_t post_handler(httpd_req_t* req) {
  int ret, remaining = req->content_len;
  char *buf, *resp;
  const long int free_heap_size = esp_get_free_heap_size();
  printf("WiFi OTA: Incoming upload bin size (%d), heap (%lu)\n", remaining, free_heap_size);
  if(free_heap_size < FW_BUF_SIZE) {
    resp = "Cannot allocate enough memory for FW upload";
    printf("%s Only %ld bytes available %d needed.\n", resp, free_heap_size, FW_BUF_SIZE);
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_FAIL;
  }
  buf = (char*) malloc(sizeof(char) * FW_BUF_SIZE);
  if(buf == NULL) {
    printf("Memory allocation failed\n");
    free(buf);
    return ESP_FAIL;
  }
  _status = OTA_FW_UPLOADING;
  while(remaining > 0) {
    if((ret = httpd_req_recv(req, buf, remaining)) <= 0) {
      if(ret == HTTPD_SOCK_ERR_TIMEOUT) {
        continue;
      }
      free(buf);
      return ESP_FAIL;
    }
    // Send for OTA
    printf("POST: Received %d Remaining %d\n", ret, remaining);
    remaining -= ret;
  }
  _status = OTA_FW_UPLOAD_COMPLETE;
  printf("WiFi OTA: FW Upload Complete\n");
  httpd_resp_send(req, NULL, 0);
  _status = OTA_REBOOTING;
  // Send Reboot event
  free(buf);
  return ESP_OK;
}

httpd_uri_t root_uri = {
  .uri      = "/",
  .method   = HTTP_GET,
  .handler  = root_handler,
  .user_ctx = NULL
};

httpd_uri_t get_uri = {
  .uri      = "/upload_status",
  .method   = HTTP_GET,
  .handler  = get_handler,
  .user_ctx = NULL
};

httpd_uri_t put_uri = {
  .uri      = "/upload_config",
  .method   = HTTP_PUT,
  .handler  = put_handler,
  .user_ctx = NULL
};

httpd_uri_t post_uri = {
  .uri      = "/upload",
  .method   = HTTP_POST,
  .handler  = post_handler,
  .user_ctx = NULL
};

httpd_handle_t start_webserver(void)
{
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  printf("WiFi OTA: Starting server on port: %d\n", config.server_port);
  if(httpd_start(&server, &config) == ESP_OK) {
    httpd_register_uri_handler(server, &root_uri);
    httpd_register_uri_handler(server, &get_uri);
    httpd_register_uri_handler(server, &put_uri);
    httpd_register_uri_handler(server, &post_uri);
    return server;
  }
  printf("WiFi OTA: Error Starting WebServer\n");
  return NULL;
}

void stop_webserver(httpd_handle_t server) {
  httpd_stop(server);
}

void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
  httpd_handle_t* server = (httpd_handle_t*) arg;
  if(*server) {
    printf("WiFi OTA: Stopping Webserver\n");
    stop_webserver(*server);
    *server = NULL;
  }
}
