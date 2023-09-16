/*
 * wifi_ota.c
 *
 *  Created on: 06-Sep-2023
 *      Author: bkhanna
 */
#include "wifi_ota.h"
#include "../../sys_conf.h"
#include <esp_http_server.h>

#define  FW_BUF_SIZE      5140 //5KB 

typedef enum ota_status {
  OTA_IDLE,
  OTA_UPLOAD_START,
  OTA_UPLOAD_COMPLETE,
  OTA_FW_WRITE_START,
  OTA_FW_WRITE_COMPLETE,
  OTA_REBOOT,
  OTA_STATUS_NUM
}ota_status_t;

extern char index_html_start[]                asm("_binary_index_html_start");
extern char index_html_end[]                  asm("_binary_index_html_end");
static httpd_handle_t   _server           = NULL;
static ota_status_t     _ota_status       = OTA_IDLE;

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
static const char* ota_status_str(const ota_status_t status) {
  const char* str = "Unknown Status";
  if(status < OTA_STATUS_NUM) return str; 
  switch(status){
    case OTA_IDLE: str = "Idle"; break;
    case OTA_UPLOAD_START: str = "OTA FW Uploading"; break;
    case OTA_UPLOAD_COMPLETE: str = "OTA FW Upload Completed"; break;
    case OTA_FW_WRITE_START: str = "OTA FW being written to flash"; break;
    case OTA_FW_WRITE_COMPLETE: str ="OTA FW written to flash"; break;
    case OTA_REBOOT: str = "OTA Reboot started"; break;
    default: return str;
  }
  return str;
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
  size_t buf_len;
  buf_len = httpd_req_get_hdr_value_len(req, "Host") + 1;
  if(buf_len > 1) {
    buf = malloc(buf_len);
    if(httpd_req_get_hdr_value_str(req, "Host", buf, buf_len) == ESP_OK) {
      printf("WiFI OTA: Found header --> host: %s\n", buf);
    }
    free(buf);
  }
  const char* resp = ota_status_str(_ota_status);
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
  if(free_heap_size < FW_BUF_SIZE)) {
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
  while(remaining > 0) {
    if((ret = httpd_req_recv(req, buf, remaining)) <= 0) {
      if(ret == HTTPD_SOCK_ERR_TIMEOUT) {
        continue;
      }
      free(buf);
      return ESP_FAIL;
    }
    remaining -= ret;
  }
  httpd_resp_send(req, NULL, 0);
  printf("WiFi OTA: FW Upload Complete\n");
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
