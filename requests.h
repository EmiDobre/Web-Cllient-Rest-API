#ifndef _REQUESTS_
#define _REQUESTS_
char *compute_get_request(char* type, char *host, char *url,
                            char *cookies, char *token);
char *compute_post_request(char *host, char *url, char* content_type, char **body_data,
							int body_data_fields_count, char** cookies, int cookies_count, char* token);
void parseJSON_body(int body_data_fields_count, char **body_data,
                                        char **body_request);
int extractJSON(char* json_string);
void extractJSONObjects(const char* string);
int  error_code (char *response);
char *get_cookie_or_jwt(char *response, char* type);
#endif
