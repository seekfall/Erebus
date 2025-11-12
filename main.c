#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>

int main(void) {
    curl_version_info_data *vinfo = curl_version_info(CURLVERSION_NOW);
    printf("libcurl version: %s\n", vinfo->version);
    return 0;
}



int heartbeat(){
    CURL *curl;
    CURLcode res;

    /*Inits the winsock*/
    res=curl_global_init(CURL_GLOBAL_DEFAULT);
    
    if(res!=CURLE_OK){
        fprintf(stderr,"curl_global_init() failed: %s\n",curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }

    
}