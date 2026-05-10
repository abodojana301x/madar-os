#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

int http_client_get(const char *url, char *out, unsigned long max_len);
int http_client_selftest(void);

#endif
