#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <windows.h>



int heartbeat(char *hostname);

int main(void) {

    char hostname[1024];
    DWORD size = sizeof(hostname); /* size in characters */
    if (!GetComputerNameA(hostname, &size)) {
        fprintf(stderr, "GetComputerNameA failed: %lu\n", GetLastError());
        strcpy(hostname, "unknown");
    }

    printf("Hostname: %s\n", hostname);
    heartbeat(hostname);
    return 0;
}




int heartbeat(char *hostname){

    CURL *curl;
    CURLcode res;
    struct curl_slist *headers = NULL;

    /*Inits the winsock*/
    res=curl_global_init(CURL_GLOBAL_DEFAULT);
    
    if(res!=CURLE_OK){
        fprintf(stderr,"curl_global_init() failed: %s\n",curl_easy_strerror(res));
        exit(EXIT_FAILURE);
    }

    /*Build json hostname for POST*/
    char json_payload[1024];
    snprintf(json_payload, sizeof(json_payload), "{\"hostname\":\"%s\"}", hostname);

    /*Inits a curl session*/
    curl=curl_easy_init();
    if(curl){
        /*Set the URL for the operation*/
        curl_easy_setopt(curl,CURLOPT_URL,"http://127.0.0.1:8000/heartbeat");
        /*Enable POST*/
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        headers = curl_slist_append(headers, "Content-Type: application/json");
        
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(json_payload));

        /*Set the hostname in the post field*/
        
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
         curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
         curl_easy_setopt(curl, CURLOPT_WRITEDATA, stdout);  //added to see server response

        /*Perform the request, res will get the return code*/
        res=curl_easy_perform(curl);



        printf("Heartbeat sent\n");

        /*Check for errors*/
        if(res!=CURLE_OK){
            fprintf(stderr,"curl_easy_perform() failed: %s\n",curl_easy_strerror(res));
            return -1;
        }

        /*Cleanup*/
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return 0;
    }


    
}

