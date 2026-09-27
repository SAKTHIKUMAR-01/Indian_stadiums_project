#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "curl_download.h"

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryBuffer *mem = (MemoryBuffer *)userp;

    char *ptr = realloc(mem->data, mem->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "[ERROR] Not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->data[mem->size] = '\0';

    return realsize;
}

int download_webpage(const char *url, MemoryBuffer *mem) {
    CURL *curl_handle;
    CURLcode res;

    mem->data = malloc(1);
    if (!mem->data) {
        fprintf(stderr, "[ERROR] Failed to allocate initial memory buffer\n");
        return -1;
    }
    mem->size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if (!curl_handle) {
        fprintf(stderr, "[ERROR] Failed to initialize libcurl handle\n");
        free(mem->data);
        mem->data = NULL;
        return -1;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)mem);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) StadiumSystem/1.0");
    curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 30L);

    printf("[libcurl] Downloading webpage from: %s\n", url);
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "[ERROR] libcurl download failed: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl_handle);
        curl_global_cleanup();
        free(mem->data);
        mem->data = NULL;
        return -1;
    }

    long response_code;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
    printf("[libcurl] Download successful! HTTP Response Code: %ld, Received: %zu bytes\n", response_code, mem->size);

    curl_easy_cleanup(curl_handle);
    curl_global_cleanup();

    if (response_code != 200) {
        fprintf(stderr, "[ERROR] HTTP server returned error code %ld\n", response_code);
        return -1;
    }

    return 0;
}

void free_memory_buffer(MemoryBuffer *mem) {
    if (mem && mem->data) {
        free(mem->data);
        mem->data = NULL;
        mem->size = 0;
    }
}
