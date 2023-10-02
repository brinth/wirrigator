/* * @file
 * @brief Handles FOTA (Firmware Over the Air) update using HTTP Webserver & WiFi Stack
 */
#include "wifi_ota.h"
#include "../../sys_conf.h"
#include <esp_http_server.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "cJSON.h"

typedef enum ota_status {
  OTA_FW_WAITING,
  OTA_FW_UPLOADING,
  OTA_FW_UPLOAD_ERROR,
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

extern char index_html_start[]                asm("_binary_index_html_start");
extern char index_html_end[]                  asm("_binary_index_html_end");
static httpd_handle_t   _server           = NULL;
static unsigned         _upload_progress  = 0;
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
static void update_status(const ota_status_t status);

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

static inline size_t index_html_size(void) { 
  ptrdiff_t bin_size = index_html_end - index_html_start;
  return (size_t) bin_size;
}

static inline int read_until(const char* buffer, char delim, int len) {
  int i = 0;
  while(buffer[i] != delim && i < len) ++i;
  return i + 1;
}

esp_err_t root_handler(httpd_req_t* req) {
  char* buf = (char*) index_html_start;
  size_t buf_size = (size_t) index_html_size();
  int ret = httpd_resp_send(req, buf, buf_size);
  return ret;
}

esp_err_t get_handler(httpd_req_t* req) {
  char resp[200];
  char buf[10];
  cJSON* root = cJSON_CreateObject();
  cJSON* status = cJSON_CreateString(ota_status_str[_status]);
  itoa(_upload_progress, buf, sizeof(buf));
  cJSON* progress = cJSON_CreateString(buf);
  cJSON_AddItemToObject(root, "status", status);
  cJSON_AddItemToObject(root, "progress", progress);
  cJSON_PrintPreallocated(root, resp, sizeof(resp), false);
  httpd_resp_send(req, resp, strlen(resp));
  cJSON_Delete(root);
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
  char                  *buf              = NULL;
  int                   ret               = 0;
  int                   remaining         = req->content_len;
  size_t                buf_size          = 0;
  const size_t          free_heap_size    = esp_get_free_heap_size();
  const esp_partition_t *update_partition = NULL;
  esp_ota_handle_t      update_handle     = 0;
  esp_err_t             err               = ESP_FAIL;
  printf("WiFi OTA: Incoming upload bin size (%d)\n", remaining);
  buf_size = (int) (free_heap_size * 0.5);
  printf("Allocating 50%% of freeheapsize(%d) bufsize(%d)\n", free_heap_size, buf_size);
  buf = (char*) malloc(sizeof(char) * buf_size);
  if(buf == NULL) {
    printf("Memory allocation failed\n");
    free(buf);
    return ESP_FAIL;
  }
  update_partition = esp_ota_get_next_update_partition(NULL);
  if(update_partition == NULL) {
    printf("Invalid update parition\n");
    free(buf);
    return ESP_FAIL;
  }
  ESP_LOGI(_tag, "Writing to parition subtype %d at offset 0x%x", update_partition->subtype, update_partition->address);
  err = esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle);
  update_status(OTA_FW_UPLOADING);
  httpd_unregister_uri(_server, "/upload");
  while(remaining > 0) {
    if((ret = httpd_req_recv(req, buf, remaining)) <= 0) {
      if(ret == HTTPD_SOCK_ERR_TIMEOUT) {
        continue;
      }
      goto post_fail;
    }
    if(ret > buf_size) {
      ESP_LOGE(_tag, "Error: buffer cannot hold payload ret=%d\n", ret);
      goto post_fail;
    }
    /*
    err = esp_ota_write(update_handle, (const void*) buf, ret);
    if(err != ESP_OK) {
      ESP_LOGE(_tag, "Error: esp_ota_write failed! err=0x%x", err);
      goto post_fail;
    }
    */
    httpd_resp_send_chunk(req, buf, ret);
    remaining -= ret;
    _upload_progress = ((req->content_len - remaining) / req->content_len) * 100;
  }
  if(esp_ota_end(update_handle) != ESP_OK) {
    ESP_LOGE(_tag, "esp_ota_end failed!");
    goto post_fail;
  }
  update_status(OTA_FW_UPLOAD_COMPLETE);
  printf("WiFi OTA: FW Upload Complete\n");
  httpd_resp_send(req, NULL, 0);
  update_status(OTA_REBOOTING);
  if((err = esp_ota_set_boot_partition(update_partition)) != ESP_OK) {
    ESP_LOGE(_tag, "esp_ota_set_boot_parition failed! err=0x%x", err);
    goto post_fail;
  }
  free(buf);
  ESP_LOGI(_tag, "Restarting ....");
  esp_restart();
  return ESP_OK;
post_fail:
  update_status(OTA_FW_UPLOAD_ERROR);
  httpd_register_uri_handler(_server, &post_uri);
  free(buf);
  return ESP_FAIL;
}

httpd_handle_t start_webserver(void)
{
  httpd_handle_t server = NULL;
  httpd_config_t config = {                        \
        .task_priority      = tskIDLE_PRIORITY+5,       \
        .stack_size         = 8192,                     \
        .server_port        = 80,                       \
        .ctrl_port          = 32768,                    \
        .max_open_sockets   = 7,                        \
        .max_uri_handlers   = 8,                        \
        .max_resp_headers   = 8,                        \
        .backlog_conn       = 1,                        \
        .lru_purge_enable   = false,                    \
        .recv_wait_timeout  = 5,                        \
        .send_wait_timeout  = 5,                        \
        .global_user_ctx = NULL,                        \
        .global_user_ctx_free_fn = NULL,                \
        .global_transport_ctx = NULL,                   \
        .global_transport_ctx_free_fn = NULL,           \
        .open_fn = NULL,                                \
        .close_fn = NULL,                               \
};

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

void update_status(const ota_status_t status) {
  _status = status;
  xEventGroupClearBits(_sys_events, EVENT_BIT(status));
  xEventGroupSetBits(_sys_events, EVENT_BIT(status));
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
