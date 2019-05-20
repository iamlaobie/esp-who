
#include "esp_http_client.h"
#include "esp_log.h"
#include "string.h"
#include "dl_lib.h"
#include "float_bytes.h"

const static char *TAG = "send";

esp_err_t _http_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
            printf("%.*s", evt->data_len, (char*)evt->data);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

esp_http_client_handle_t client = NULL;
void create_client () {
    if (client != NULL) return;
    esp_http_client_config_t config = {
        .url = "http://192.168.3.154:8888/esp-eye",
        .event_handler = _http_event_handle
    };
    client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "device-id", "87654321");
}

void post_feature ( dl_matrix3d_t *face_id ) {
    char *feature = float_array_to_bytes(face_id->item, face_id->c);
    if (!client) create_client();
    esp_http_client_set_header(client, "content-type", "text/plain");
    esp_http_client_set_header(client, "data-type", "feature");
    esp_http_client_set_post_field(client, feature, strlen(feature));
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    }
    free(feature);
}

void post_image_feature(char *image, int image_len, dl_matrix3d_t *face_id, float *face_box, float *landmark) {
    if (!client) create_client();
    char *feature = float_array_to_bytes(face_id->item, face_id->c);
    char *face_box_bytes = float_array_to_bytes(face_box, 4);
    char *landmark_bytes = float_array_to_bytes(landmark, 10);
    esp_http_client_set_header(client, "content-type", "text/plain");
    esp_http_client_set_header(client, "data-type", "image-feature");
    int feature_len = face_id->c * sizeof(float);
    int landmark_len = sizeof(float) * 10;
    int box_len = sizeof(float) * 4;
    // 图片、特征、人在图片中的位置组装成字节
    int len = image_len + feature_len + box_len + landmark_len;
    char *body = (char *)malloc(len);
    ESP_LOGI(TAG, "feature length:%u; image length:%u; total:%u; aligned_face->c:%u", feature_len, image_len, len, face_id->c);
    memcpy(body, feature, feature_len);
    memcpy(body + feature_len, face_box_bytes, box_len);
    memcpy(body + feature_len + box_len, landmark_bytes, landmark_len);
    memcpy(body + feature_len + box_len + landmark_len, image, image_len);
    esp_http_client_set_post_field(client, body, len);
    esp_err_t err = esp_http_client_perform(client);
    free(body);
    free(feature);
    ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));
}

void post_image (char *data, int len) {
    if (!client) create_client();
    esp_http_client_set_header(client, "content-type", "image/jpeg");
    esp_http_client_set_header(client, "data-type", "image");
    esp_http_client_set_post_field(client, (char *)data, len);
    esp_err_t err = esp_http_client_perform(client);

    ESP_LOGI(TAG, "Status = %d, content_length = %d",
            esp_http_client_get_status_code(client),
            esp_http_client_get_content_length(client));

}
