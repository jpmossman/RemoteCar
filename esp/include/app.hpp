#ifndef APP_H_
#define APP_H_

#define CAMERA_MODEL_AI_THINKER // Our camera model
#include <Arduino.h>
#include "camera_pins.hpp"
#include "control.hpp"
#include "../include/frontend.h"
#include "esp_camera.h"
#include "esp_http_server.h"

// Some sent strings need this extra info, tedious to re-type
#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

// 
httpd_handle_t web_httpd = NULL;
httpd_handle_t stream_httpd = NULL;

// Test handler for server
static esp_err_t test_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/plain");
  return httpd_resp_send(req, "Test handler part 1",19);
}

// index handler for server
static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)index_html_gz, index_html_gz_len);
}

// js handler for server
static esp_err_t js_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "application/javascript");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)index_js_gz, index_js_gz_len);
}

// style handler for server
static esp_err_t css_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/css");
  httpd_resp_set_hdr(req, "Content-Encoding", "gzip");
  return httpd_resp_send(req, (const char *)style_css_gz, style_css_gz_len);
}

// Stream handler for server
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  size_t _jpg_buf_len = 0;
  uint8_t *_jpg_buf = NULL;
  char *part_buf[64];
  // dl_matrix3du_t *image_matrix = NULL;

  // Cut out the weird if statement from the example, SHOULDNT cause issues
  static int64_t last_frame = 0;
  if (!last_frame) last_frame = esp_timer_get_time();

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  // Main streaming loop-
  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      if (true /*!detection_enabled, but we aren't detecting*/) {
        if (fb->format != PIXFORMAT_JPEG) {
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
          esp_camera_fb_return(fb);
          fb = NULL;
          if (!jpeg_converted) {
            Serial.println("JPEG compression failed");
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      } else {
        // Do the detecting stuff
      }
    }
    // Send the stream boundary
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }
    // Send part buffer
    if (res == ESP_OK) {
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
    }
    // Send jpg buffer
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
    }
    // Get fb
    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      _jpg_buf = NULL;
    } else if (_jpg_buf) {
      free(_jpg_buf);
      _jpg_buf = NULL;
    }
    // Break if we're not OK
    if (res != ESP_OK) {
      break;
    }

    /* There's some frame time stuff here in the template. Seemed unnecessary so I omitted it.*/
    int64_t fr_end = esp_timer_get_time();
    int64_t frame_time = fr_end - last_frame;
    last_frame = fr_end;
    frame_time /= 1000;
    // Serial.printf("MJPG: %uB %ums (%.1ffps)\n", 
    //   (uint32_t)(_jpg_buf_len),
    //   (uint32_t)frame_time, 1000.0 / (uint32_t)frame_time
    // );
  }

  last_frame = 0;
  return res;
  
}

void startServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();

  httpd_uri_t index_uri = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = index_handler,
    .user_ctx = NULL
  };

  httpd_uri_t js_uri = {
    .uri      = "/index.js",
    .method   = HTTP_GET,
    .handler  = js_handler,
    .user_ctx = NULL
  };

  httpd_uri_t css_uri = {
    .uri      = "/style.css",
    .method   = HTTP_GET,
    .handler  = css_handler,
    .user_ctx = NULL
  };  

  httpd_uri_t test_uri = {
    .uri      = "/test",
    .method   = HTTP_GET,
    .handler  = test_handler,
    .user_ctx = NULL
  };  

  Serial.printf("Starting web server on port: '%d'\n", config.server_port);
  if (httpd_start(&web_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(web_httpd, &index_uri);
    httpd_register_uri_handler(web_httpd, &test_uri);
    httpd_register_uri_handler(web_httpd, &js_uri);
    httpd_register_uri_handler(web_httpd, &css_uri);
  }
}

void startStream() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.server_port += 1;
  config.ctrl_port   += 1;

  httpd_uri_t stream_uri = {
    .uri      = "/stream",
    .method   = HTTP_GET,
    .handler  = stream_handler,
    .user_ctx = NULL
  };

  Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

void app_init_camera() {
  // Make the camera config
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // If PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
    Serial.println("PID is OV3660");
  }
  else Serial.println("PID is NOT OV3660");
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);
}


#endif /* APP_H_ */