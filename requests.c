#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.c"

char *compute_post_request(char *host, char *url, char* content_type, 
                            char** input,int nr_input, 
                            char **cookies, int cookies_count, char* token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_request = calloc(LINELEN, sizeof(char));

    //Headere
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);
    sprintf(line, "HOST: %s:8080", host);
    compute_message(message, line);
   
    //Token daca exista
    if ( token != NULL ) { 
        sprintf(line, "Authorization: Bearer %s",token);
        compute_message(message, line);
    }

    //Content types
    sprintf(line, "Content-Type: %s",content_type);
    compute_message(message, line);

    //Content Len: e nevoie de lungimea body ului
    int length = 0;  
    parseJSON_body(nr_input, input, &body_request);  
    length = strlen(body_request);
    sprintf(line, "Content-Length: %d",length );
    compute_message(message, line);

    //Cookie daca exista
    if ( cookies != NULL ) {
        for( int i = 0; i < cookies_count; i++ ) {
            sprintf(line, "Cookie: %s",cookies[i]);
            compute_message(message, line);
        }
    }
    //adaugare linie goala la finalul headerului
    compute_message(message, "");

    //adaugare body pt cerere 
    compute_message(message, body_request);

    free(line);
    free(body_request);
    return message;
}


char *compute_get_request(char*type, char *host, char *url,
                            char *cookies, char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    //Headere
    sprintf(line, "%s %s HTTP/1.1",type, url);
    compute_message(message, line);
    memset(line, 0, LINELEN);
    sprintf(line, "HOST: %s:8080", host);
    compute_message(message, line);

    //Cookie daca exista
    if ( cookies != NULL ) {
        sprintf(line, "Cookie: %s",cookies);
        compute_message(message, line);
    
    }

    //Token daca exista
    if ( token != NULL ) {
        sprintf(line, "Authorization: Bearer %s",token);
        compute_message(message, line);
    }

    //adaugare linie goala
    compute_message(message, "");
    free(line);
    return message;
}

//parsare json -> inputul devine obiect json pt body cerere
void parseJSON_body(int body_data_fields_count, char **body_data,
                                        char **body_request) {
	JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);
    char *serialized_string = NULL;

    if ( body_data_fields_count <= 2 ) {
        json_object_set_string(root_object, "username", body_data[0]);
        json_object_set_string(root_object, "password", body_data[1]);
    } else {
        json_object_set_string(root_object, "title", body_data[0]);
        json_object_set_string(root_object, "author", body_data[1]);
        json_object_set_string(root_object, "genre", body_data[2]);
        json_object_set_number(root_object, "page_count", atoi(body_data[3]));
        json_object_set_string(root_object, "publisher", body_data[4]);
    }
    
    serialized_string = json_serialize_to_string_pretty(root_value);
    strcpy(*body_request, serialized_string);
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
}

//parsareJSON invers: dintr-un string sub forma json extrag campurile
int extractJSON(char* json_string) {
    JSON_Value* root_value = json_parse_string(json_string);
    JSON_Object* root_object = json_value_get_object(root_value);

    //raspuns cu o carte: 
    //il pun inainte de vf eroar esau token pt ca poate am
    //o carte ce contine cuvantul error sau token si nu s-ar afisa bine
    if ( strstr(json_string, "publisher") != NULL ) {
        int id = json_object_get_number(root_object, "id");
        const char* author = json_object_get_string(root_object, "author");
        const char* publisher = json_object_get_string(root_object, "publisher");
        const char* title = json_object_get_string(root_object, "title");
        const char* genre = json_object_get_string(root_object, "genre");
        int page_count = json_object_get_number(root_object, "page_count");

        printf("id: %d\n", id);
        printf("title: %s\n", title);
        printf("author: %s\n", author);
        printf("genre: %s\n", genre);
        printf("page_count: %d\n", page_count);
        printf("publisher: %s\n", publisher);

        json_value_free(root_value);
        return 0;
    }

    //pot avea raspuns cu eroare:
    if ( strstr(json_string, "error") != NULL ) {
        const char* error = json_object_get_string(root_object, "error");
        printf("error: %s\n", error);
        json_value_free(root_value);
        return 1;
    }

    //raspuns cu token:
    if ( strstr(json_string, "token") != NULL ) {
        const char* token = json_object_get_string(root_object, "token");
        printf("token: %s\n", token);
        json_value_free(root_value);
        return 1;
    }

    //altfel scot o carte format get_books: doar id si titlu
    int id = json_object_get_number(root_object, "id");
    const char* title = json_object_get_string(root_object, "title");
    printf("id: %d\n", id);
    printf("title: %s\n", title);

    json_value_free(root_value);
    return 0;
}   

//stringul este sub forma unui array de obiecte json 
void extractJSONObjects(const char* string) {
    JSON_Value* root = json_parse_string(string);
    JSON_Array* jsonArray = json_value_get_array(root);
    int count = json_array_get_count(jsonArray);
    for (int i = 0; i < count; ++i) {
        JSON_Value* jsonValue = json_array_get_value(jsonArray, i);
        char* jsonString = json_serialize_to_string(jsonValue);
        extractJSON(jsonString);
        printf("\n");
        json_free_serialized_string(jsonString);
    }
    json_value_free(root);
}

//extrag codul de eroare din raspuns
//fiecare response are codul de eroare pe prima linie si se termina cu o linie goala
int error_code (char *response) {
    if ( strlen(response) == 0 ) 
        return 0;

    int start= 9; 
    int end = 12;
    char *error_code = calloc(3, sizeof(char));
    while ( start < end ) {
        error_code[start - 9] = response[start];
        start++;
    }
    int code = atoi(error_code);
    return code;
}


//cookie se gaseste dupa set cookie este connect.sid=.... pana in Path 
//tokenul se gasteste la {"token":" ...valoare token. "}
char* get_cookie_or_jwt(char *response, char* type) {
    char* header = NULL;
    char escape ;
    if ( strncmp(type, "jwt", 3 ) == 0) {
        header = "\"token\":\"";
        escape = '\"';
    } else {
        header = "Set-Cookie: ";
        escape = ';';
    }
    
    char* start = strstr(response, header);
    if ( start == NULL ) 
        return NULL;
    
    start = start + strlen(header);
    char* end = strchr(start, escape);
    if (end == NULL) 
        return NULL; 
    
    int length = end - start;
    char* value = calloc(length + 1, sizeof(char));
    strncpy(value, start, length);
    value[length] = '\0';
    return value;
}
