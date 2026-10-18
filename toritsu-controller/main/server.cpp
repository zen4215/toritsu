#include "main.hpp"

#include <esp_https_server.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

// ws frame binary format
// null arg command | type: u8 = 0 | command: 16 | stop 0 reset 1 ping 2 pong 3
// notice | type: u8 = 1 | enum: 16 |
// pitch +-60 deg map in i16 | type: u8 = 0x10 | pitch: i16 |
// yaw +-1.0 duty map in i16 | type: u8 = 0x11| pitch: i16 |
// duty cycle +-1.0 map in i16 | type: u8 = 0x12 | duty: i16 |

static uint8_t ws_recv_buffer[16];

httpd_handle_t start_server();
void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                     void* data);
void disconnect_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* data);

void server_task(void*) {
  ESP_ERROR_CHECK(nvs_flash_init());
  ESP_ERROR_CHECK(esp_netif_init());

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  const esp_netif_t* ap_netif = esp_netif_create_default_wifi_ap();

  const wifi_init_config_t wifi_init_config_default =
      WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config_default));

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

  wifi_config_t wifi_config = {};
  memcpy(wifi_config.ap.ssid, WIFI_AP_SSID, strlen(WIFI_AP_SSID));
  wifi_config.ap.ssid_len = static_cast<uint8_t>(strlen(WIFI_AP_SSID));
  wifi_config.ap.ssid_hidden = 0;
  wifi_config.ap.channel = 1;
  wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  wifi_config.ap.max_connection = 4;
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  httpd_handle_t server;

  // esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, connect_handler,
  //                            &server);
  // esp_event_handler_register(IP_EVENT, WIFI_EVENT_STA_DISCONNECTED,
  //                            disconnect_handler, &server);

  server = start_server();

  while(1) {
    vTaskDelay(1000);
  }
}

esp_err_t ws_handler(httpd_req_t* req) {
  httpd_ws_frame_t ws_packet;

  ws_packet.type = HTTPD_WS_TYPE_BINARY;
  esp_err_t ret = httpd_ws_recv_frame(req, &ws_packet, 0); // read length
  if(ret != ESP_OK) {
    return ret;
  }

  if(ws_packet.len) {
    if(ws_packet.len > sizeof(ws_recv_buffer)) {
      return ESP_ERR_INVALID_SIZE;
    }

    ws_packet.payload = ws_recv_buffer;
    ret = httpd_ws_recv_frame(req, &ws_packet, ws_packet.len); // receive frame
    if(ret != ESP_OK) {
      return ret;
    }
  }

  if(ws_packet.type == HTTPD_WS_TYPE_BINARY && ws_packet.payload != nullptr &&
     ws_packet.len == 3) {

    httpd_ws_frame_t frame;
    uint8_t paylaod[3];

    printf("ws received: %d - %x,%x,%x\n", frame.len, ws_recv_buffer[0],
           ws_recv_buffer[1], ws_recv_buffer[2]);

    switch(ws_packet.payload[0]) {
      case 0:
        switch((int16_t)(ws_packet.payload[1] << 8) | ws_packet.payload[2]) {
          case 0:
            stopped = true;
            break;

          case 1:
            esp_restart();
            break;

          case 2:
            frame.type = HTTPD_WS_TYPE_BINARY;
            paylaod[0] = 0;
            paylaod[1] = 0;
            paylaod[2] = 3;
            frame.payload = paylaod;
            frame.len = 3;
            httpd_ws_send_frame(req, &frame);
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
  }

  return 0;
}

constexpr httpd_uri_t ws = {
    .uri = "/ws",
    .method = HTTP_GET,
    .handler = ws_handler,
    .user_ctx = nullptr,
    .is_websocket = true,
    .handle_ws_control_frames = false,
    .supported_subprotocol = nullptr,
};

httpd_uri_t example_uri = {
    .uri = "/example",
    .method = HTTP_GET,
    .handler = [](httpd_req_t* req) -> esp_err_t {
      const char* example_page =
          R"***(<!DOCTYPE html>
  <html>
    <body>
      <h1>Hello from ESP32</h1>
      <button id="appPing">WebSocket Ping by App</button>

      <script>
        const ws = new WebSocket("ws://192.168.4.1/ws");

        ws.onopen = () => {
          console.log("ws open");
        };

        ws.onclose = () => {
          console.log("ws close");
        };

        ws.onmessage = () => {
          console.log("ws receive");
        };

        const button = document.getElementById("appPing");

        button.onclick = () => {
          const ping = new Uint8Array([0x00, 0x00, 0x02]);

          ws.send(ping);
          console.log("ws send");
        }
      </script>
    </body>
  </html>
)***";

      httpd_resp_set_type(req, "text/html");

      return httpd_resp_send(req, example_page, HTTPD_RESP_USE_STRLEN);
    },
    .user_ctx = nullptr,
    .is_websocket = false,
    .handle_ws_control_frames = {},
    .supported_subprotocol = nullptr,
};

httpd_handle_t start_server() {
  httpd_handle_t server;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  if(httpd_start(&server, &config) != ESP_OK) {
    return nullptr;
  } else {
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &ws));
    ESP_ERROR_CHECK(httpd_register_uri_handler(server, &example_uri));
    return server;
  }
}

void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id,
                     void* data) {}
void disconnect_handler(void* arg, esp_event_base_t event_base,
                        int32_t event_id, void* data) {}
