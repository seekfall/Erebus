#include <stdio.h>
#include <curl/curl.h>

int main(void) {
    curl_version_info_data *vinfo = curl_version_info(CURLVERSION_NOW);
    printf("libcurl version: %s\n", vinfo->version);
    return 0;
}
