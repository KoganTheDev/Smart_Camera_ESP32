#pragma once

#include <esp_http_server.h>
#include "camera.h"

class HttpServer
{
private:
    httpd_handle_t _server_handle = NULL;
    static Camera* _camera_instance;

public:
    /**
     * @brief Starts the web server on port 80.
     * @return true if the server started successfully.
     */
    bool start(Camera* camera);

    /**
     * @brief Stops the server to free up memory.
     */
    void stop();

    /**
     * @brief Handler for the JPG video stream.
     */
    static esp_err_t stream_handler(httpd_req_t *req);
    
    /**
     * @brief Handler for motor commands (e.g., GET /move?pan=10).
     */
    static esp_err_t cmd_handler(httpd_req_t *req);
};