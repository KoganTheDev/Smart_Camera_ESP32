#include "turret_server.h"
#include <Arduino.h>
#include <esp_log.h>

#include "esp_camera.h"
#include <index_html.h>

#define _STREAM_CONTENT_TYPE                                                   \
  "multipart/x-mixed-replace;boundary=123456789000000000000987654321"

static const char *_STREAM_BOUNDARY =
    "\r\n--123456789000000000000987654321\r\n";
static const char *_STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

Camera *HttpServer::_camera_instance = nullptr; // Init static pointer

bool HttpServer::start(Camera *camera) {
  this->_camera_instance = camera;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  if (httpd_start(&this->_server_handle, &config) == ESP_OK) {
    // Define Root Route
    httpd_uri_t index_uri = {.uri = "/",
                             .method = HTTP_GET,
                             .handler = index_handler,
                             .user_ctx = NULL};
    httpd_register_uri_handler(this->_server_handle, &index_uri);

    // Define the Video Stream Route
    httpd_uri_t stream_uri = {.uri = "/stream",
                              .method = HTTP_GET,
                              .handler = stream_handler,
                              .user_ctx = NULL};
    httpd_register_uri_handler(this->_server_handle, &stream_uri);

    // Define the Motor Command Route
    httpd_uri_t cmd_uri = {.uri = "/move",
                           .method = HTTP_GET,
                           .handler = cmd_handler,
                           .user_ctx = NULL};
    httpd_register_uri_handler(this->_server_handle, &cmd_uri);

    return true;
  }
  return false;
}

void HttpServer::stop() {
  if (this->_server_handle) {
    httpd_stop(this->_server_handle);
    this->_server_handle = NULL;
  }
}

esp_err_t HttpServer::index_handler(httpd_req_t *req) {
  return httpd_resp_send(req, HTML_PAGE, HTTPD_RESP_USE_STRLEN);
}

esp_err_t HttpServer::stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];

  // 1. Safety Check: Ensure camera and request are valid
  if (req == NULL) {
    return ESP_ERR_INVALID_ARG;
  }

  if (HttpServer::_camera_instance == nullptr) {
    Serial.println("Stream Error: Camera instance is null");
    return httpd_resp_send_500(req); // 500: internal server error
  }

  // 2. Set the HTTP Response Header to Multipart
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }

  Serial.println("Client connected to stream");

  // 3. Start the Infinite Streaming Loop
  while (true) {
    // Grab frame
    fb = HttpServer::_camera_instance->capture();

    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      _jpg_buf_len = fb->len;
      _jpg_buf = fb->buf;
    }

    if (res == ESP_OK) {
      // Send the boundary separator
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY,
                                  strlen(_STREAM_BOUNDARY));

      // Send the part header (Content-Type and Content-Length)
      if (res == ESP_OK) {
        size_t hlen =
            snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
        res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
      }

      // Send the actual JPEG binary data
      if (res == ESP_OK) {
        res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
      }
    }

    // 4. Release the frame buffer back to the camera driver
    if (fb) {
      HttpServer::_camera_instance->release(fb);
      fb = NULL;
      _jpg_buf = NULL;
    }

    // 5. Check if the client closed the tab/browser
    if (res != ESP_OK) {
      Serial.println("Stream stopped: Client disconnected");
      break;
    }

    vTaskDelay(1);
  }

  return res;
}

esp_err_t HttpServer::cmd_handler(httpd_req_t *req) {
  char buf[128];

  if (req == nullptr) {
    return ESP_ERR_INVALID_ARG;
  }

  size_t buf_len = httpd_req_get_url_query_len(req) + 1;

  if (buf_len > 1) {
    if (httpd_req_get_url_query_str(req, buf, buf_len) == ESP_OK) {
      char param[32];

      // Extract "pan" value
      if (httpd_query_key_value(buf, "pan", param, sizeof(param)) == ESP_OK) {
        int pan_val = atoi(param);
        Serial.printf("Command Received: Pan to %d\n", pan_val);
        // TODO: Call Servo/Motor function here: turret.movePan(pan_val);
      }

      // Extract "tilt" value
      if (httpd_query_key_value(buf, "tilt", param, sizeof(param)) == ESP_OK) {
        int tilt_val = atoi(param);
        Serial.printf("Command Received: Tilt to %d\n", tilt_val);
        // TODO: Call Servo/Motor function here: turret.moveTilt(tilt_val);
      }
    }
  }

  // Send a simple "OK" response back to the browser
  const char *resp_str = "Command Processed";
  httpd_resp_send(req, resp_str, strlen(resp_str));
  return ESP_OK;
}