#include "esp_http_client.h"

void post_feature(dl_matrix3du_t* aligned_face);
void post_image(char *data, int len);
void post_image_feature(char *image, int image_len, dl_matrix3d_t* face_id, float* face_box, float *landmark);