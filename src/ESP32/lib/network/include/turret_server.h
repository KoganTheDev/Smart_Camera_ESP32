/**
 * @file HttpServer.h
 * @brief Web server interface for MJPEG streaming and remote command handling.
 */

#pragma once

#include "camera.h"
#include <esp_http_server.h>

/**
 * @class HttpServer
 * @brief Manages the ESP32-CAM web interface and control API.
 * @details This class wraps the ESP-IDF HTTP server component. It provides
 * a video stream endpoint and a command endpoint for remote movement.
 */
class HttpServer {
private:
  httpd_handle_t _server_handle =
      NULL; ///< Internal handle for the ESP-IDF server instance.

  /**
   * @brief Static reference to the camera for use in C-style callbacks.
   */
  static Camera *_camera_instance;

public:
  /**
   * @brief Configures and launches the web server on port 80.
   * @param camera Pointer to the initialized Camera object for streaming.
   * @return true if the server was created and handlers were registered.
   */
  bool start(Camera *camera);

  /**
   * @brief Shut down the server and unregister all URI handlers.
   */
  void stop();

  /**
   * @brief HTTP handler for the root ("/") URI.
   * This method is called by the ESP HTTP server whenever a client accesses the
   * root IP address. It serves the primary web interface stored in Flash
   * memory.
   * @param req Pointer to the HTTP request object containing session data.
   * @return esp_err_t Returns ESP_OK if the response was sent successfully,
   * otherwise returns an error code from the underlying HTTP server.
   * @see index_html.h
   */
  static esp_err_t index_handler(httpd_req_t *req);

  /**
   * @brief HTTP GET Handler for the MJPEG stream.
   * @details Continuously captures frames from the camera and pushes them
   * to the client using "multipart/x-mixed-replace".
   * @param req Pointer to the HTTP request structure.
   * @return esp_err_t ESP_OK on success.
   */
  static esp_err_t stream_handler(httpd_req_t *req);

  /**
   * @brief HTTP GET Handler for control commands.
   * @details Parses URL parameters (e.g., ?pan=90&tilt=45) to drive motors.
   * @param req Pointer to the HTTP request structure.
   * @return esp_err_t ESP_OK on success.
   */
  static esp_err_t cmd_handler(httpd_req_t *req);
};
