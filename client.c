#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include <ctype.h>      /* isdigit */
#include "helpers.h"
#include "requests.h"

void command_POST(char* command, char** input, int nr_input, char** request,
							char* host, char* url, char* application, char* token) {
    char path[100];
    if ( strncmp(command, "register", 8) == 0 ) 
        strcpy(path, "auth/register");
    if ( strncmp(command, "login", 5) == 0 )
        strcpy(path, "auth/login");
    if ( strncmp(command, "add_book", 8) == 0 ) 
        strcpy(path, "library/books");
    
    char full_url[100];
    strcpy(full_url, url);
    strcat(full_url, path);

    *request = compute_post_request(host, full_url, application, input,
									nr_input, NULL, 0, token);
}


//comenzi care dau un get request sau delete
void command_GET(char* command, char** request, char* token , char* cookie, char* host,
							char* url, char* application, char* id) {
    char* type;
    char path[100];
    if ( strncmp(command, "get_books", 9) == 0 ) {
        strcpy(path, "library/books/");
    } else {
        if ( strncmp(command, "get_book", 8) == 0 ) {
            strcpy(path, "library/books/");
            strcat(path, id);
        }
    }
    if ( strncmp(command, "logout", 6) == 0 ) 
        strcpy(path, "auth/logout");
    if ( strncmp(command, "enter_library", 13) == 0 ) 
        strcpy(path, "library/access");

    if ( strncmp(command, "delete_book", 11) == 0 ) {
        strcpy(path, "library/books/");
        strcat(path, id);
        type = "DELETE";
    } else {
        type = "GET";
    }
    
    char full_url[100];
    strcpy(full_url, url);
    strcat(full_url, path);

    *request = compute_get_request(type, host, full_url, cookie, token);
    
}
//1 valid - 0 invalid
int valid_command (char* command) {
    if( strncmp(command, "exit", 4) == 0 ) {
        if ( strlen(command) == 4 ) 
            return 1;
    }
    if( strncmp(command, "register", 8) == 0 ) {
        if ( strlen(command) == 8 ) 
            return 1;
    }
    if( strncmp(command, "login", 5) == 0 ) {
        if ( strlen(command) == 5 ) 
            return 1;
    }
    if( strncmp(command, "logout", 6) == 0 ) {
        if ( strlen(command) == 6 ) 
            return 1;
    }
    if ( strncmp(command, "enter_library", 13 ) == 0) {
        if ( strlen(command) == 13)
            return 1;
    }
    if ( strncmp(command, "get_books", 9) == 0) {
        if  (strlen(command) == 9)
            return 1;
    } else {
        if ( strncmp(command, "get_book", 8) == 0 ) {
            if (strlen(command) == 8) 
                return 1;
        }
    }
    if ( strncmp(command, "add_book", 8) == 0) {
        if ( strlen(command) == 8 )
            return 1;
    }
    if ( strncmp(command, "delete_book", 11) == 0) {
        if ( strlen(command) == 11 )
            return 1;
    }

    return 0;
}

//pt sirul de nr apelez is digit vf daca e cifra fiecare elem
int is_number(char* nr) {
    for (int i = 0; i < strlen(nr); i++) {
        if ( isdigit(nr[i]) == 0 )
            return 0;
    }
    return 1;
}

//constarngeri input;
int check_string(char input[150], char* special_ch) {
    for ( int i = 0; i < strlen(input); i++ ) {
        if ( strchr(special_ch, input[i]) != NULL ) 
            return 0; 
    }
    return 1;
}

int check_input(char** input, int nr_input ) {
    if ( nr_input == 2 ) {
        int user = check_string(input[0], " ~<>`!@#$%^&*()+={}[]|\\;:?'/.,\"");
        if  ( user == 0 ) {
            printf("Username can contain: letters, _ , - only!\n\n");
        }
        return user;
    }

    if ( nr_input == 5 ) {
        int title =  check_string(input[0], "~<>@#$%^&*()+={}[]|\\;/");
        int author = check_string(input[1],"1234567890~<>`!@#$%^&*()+={}[]|\\;:?/.\"");
        int genre = check_string(input[2],"1234567890~<>`!@#$%^&*()+={}[]|\\;:?/.\"");
        int page_count = is_number(input[3]);
        int publisher = check_string(input[4],"1234567890~<>`!@#$%^&*()+={}[]|\\;:?/.\"");
        if ( title == 0 ) {
            printf("Title can contain: letters, \", ' ! ? : ` , white spaces and numbers only!\n\n");
            return 0;
        }
        if ( author == 0 ) {
            printf("Author can contain: letters ' , and white spaces only!\n\n");
            return 0;
        }
        if ( genre == 0 ) {
            printf("Genre can contain: letters ' , and white spaces only!\n\n");
            return 0;
        }
        if ( page_count == 0 ) {
            printf("Page count must be a valid number!\n\n");
            return 0;
        }
        if ( publisher == 0 ) {
            printf("Publisher can contain: letters ' , and white spaces only!\n\n");
            return 0;
        }
    }

    return 1;
}

void show_response(char* response, char* command) {
    if ( response == NULL ) 
        return;    
    int code_err = error_code(response);
    if ( code_err == 200 ) 
        printf("HTTP/1.1 200 OK\n");
    if ( code_err == 201 ) 
        printf("HTTP/1.1 201 Created\n");
    if ( code_err == 204 ) 
        printf("HTTP/1.1 204 No Content\n");
    if ( code_err == 400 ) 
        printf("HTTP/1.1 400 Bad Request\n");
    if ( code_err == 401 ) 
        printf("HTTP/1.1 401 Unauthorized\n");
    if ( code_err == 403 ) 
        printf("HTTP/1.1 403 Forbidden\n");
    if ( code_err == 404 )
        printf("HTTP/1.1 404 Resource Not Found\n");
    if ( code_err == 500 ) 
        printf("HTTP/1.1 500 Internal Server Error\n");

    char* start = strchr(response, '{');
    char* end = strrchr(response, '}');
    if ( start == NULL || end == NULL || end <= start ) 
        return; 
    int length = end - start + 1;
    char* string = (char*)calloc((length + 1), sizeof(char));
    strncpy(string, start, length);
    string[length] = '\0';

    if ( strncmp(command, "get_books", 9) == 0) {   
        char array_json[3+strlen(string)]; 
        sprintf(array_json, "[%s]", string);
        extractJSONObjects(array_json);
        return;
    }
    extractJSON(string);
}

int main(int argc, char *argv[]) {
    char command[50];
    char type_request[7]; //POST GET SAU DELETE
    int  need_login = -1;
    int  logged = 0;

    char url[100];
    strcpy(url,"/api/v1/tema/");
    char host [14];
    strcpy(host,"34.254.242.81");
    char application[17];
    strcpy(application,"application/json");
    char *cookie = NULL;
    char *token = NULL;
    int response_code = 0;

    char *request = NULL;
    char *response = NULL;
    int sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);

    while(1) {
        printf("Enter command: ");
        fgets(command, 50, stdin);
        command[strlen(command)-1] = '\0';

        ////////////////////Pas1: comenzi si tipul lor//////////////////////

        if ( valid_command(command) == 0 ) {
            printf("Invalid command!\n");
            continue;
        }
            
        if( strncmp(command, "exit", 4) == 0 ) 
            break;
        
        if( strncmp(command, "register", 8) == 0 ) {
            strcpy(type_request, "POST");
            need_login = -1;
        }

        if( strncmp(command, "login", 5) == 0 ) {
            strcpy(type_request, "POST");
            need_login = 0;
        }

        if( strncmp(command, "logout", 6) == 0 ) {
            strcpy(type_request, "GET");
            need_login = 1;
        }

        if ( strncmp(command, "enter_library", 13 ) == 0) {
            strcpy(type_request, "GET");
            need_login = 1;
        }

        //get_book sau books:
        if ( strncmp(command, "get_book", 8) == 0) {
            strcpy(type_request, "GET");
            need_login = 1;
        }

        if ( strncmp(command, "add_book", 8) == 0 ) {
            strcpy(type_request, "POST");
            need_login = 1;
        }

        if ( strncmp(command, "delete_book", 11) == 0) {
            strcpy(type_request, "DELETE");
            need_login = 1;
        }


        ////////////////////Pas2: vf login://////////////////////

        //am nev de login: vf daca sunt logat
        if ( need_login == 1 ) {
            if( logged == 0 ) {
                printf("You are not logged in!\n");
                continue;
            }
        }
        //nu am nevoie de login: vf sa nu fiu deja logat
        if ( need_login == 0 ) {
            if( logged == 1 ) {
                printf("You are already logged in!\n");
                continue;
            }
        }

        ////////////////////Pas3: input + vf//////////////////////

        char** input = (char**) calloc(5, sizeof(char*));
        for ( int i = 0; i < 5; i++ ) {
            input[i] = (char*) calloc(150, sizeof(char));
        }
        int  nr_input = 0;

        if( strncmp(command, "register", 8) == 0 || strncmp(command, "login", 5) == 0 ) {
            printf("username = ");
            fgets(input[0], 150, stdin);
            input[0][strlen(input[0])-1] = '\0';
            printf("password = ");
            fgets(input[1], 150, stdin);
            input[1][strlen(input[1])-1] = '\0';  
            nr_input = 2;

            if ( check_input(input, nr_input) == 0 )
                continue;
        }

        //id pt get book sau delete 
        char id[150];
        if ( strncmp(command, "get_book", 8) == 0) {
            if ( strlen(command) == 8 ) {
                printf("id= ");
                fgets(id, 150, stdin);
                id[strlen(id)-1] = '\0';
                if ( strlen(id) >= 10 ) {
                    printf("Number too big, invaid id!\n");
                    continue;
                }
                if ( is_number(id) == 0 ) {
                    printf("The id must be a valid number!\n");
                    continue;
                }
            }           
        }

        //carti pt add
        if ( strncmp(command, "add_book", 8) == 0 ) {
            printf("title= ");
            fgets(input[0], 150, stdin);
            input[0][strlen(input[0])-1] = '\0';
            printf("author= ");
            fgets(input[1], 150, stdin);
            input[1][strlen(input[1])-1] = '\0';
            printf("genre= ");
            fgets(input[2], 150, stdin);
            input[2][strlen(input[2])-1] = '\0';
            printf("page_count= ");
            fgets(input[3], 150, stdin);
            input[3][strlen(input[3])-1] = '\0';
            printf("publisher= ");
            fgets(input[4], 150, stdin);
            input[4][strlen(input[4])-1] = '\0';
            nr_input = 5;

            if ( check_input(input, nr_input) == 0 )
                continue;
            
        }

        if ( strncmp(command, "delete_book", 11) == 0) {
            printf("id= ");
            fgets(id, 150, stdin);
            id[strlen(id)-1] = '\0';
            if ( strlen(id) >= 10 ) {
                printf("Number too big, invaid id!\n");
                continue;
            }
            if ( is_number(id) == 0 ) {
                printf("The id must be a valid number!\n");
                continue;
            }
        }

        ////////////////////Pas4: construire request POST sau GET//////////////////////

        if ( strncmp(type_request, "POST", 4) == 0 ) 
            command_POST(command, input, nr_input, &request, host, url, application, token);
        if ( strncmp(type_request, "GET", 3) == 0 )
            command_GET(command, &request, token, cookie, host, url, application, id);
        if ( strncmp(type_request, "DELETE", 6) == 0 )
            command_GET(command, &request, token, cookie, host, url, application, id);


         ////////////////////Pas5: aflare raspuns server//////////////////////

        //printf("\n--- REQUEST ---\n");
        //puts(request);
        send_to_server(sockfd, request);
        response = receive_from_server(sockfd);
        //restabilire conexiune daca raspunsul e gol
        if(strlen(response) == 0) {
            free(response);
            sockfd = open_connection(host, 8080, AF_INET, SOCK_STREAM, 0);
            send_to_server(sockfd, request);
            response = receive_from_server(sockfd);
        }
        printf("--- RESPONSE: --- \n");
        show_response(response, command);
        // puts(response);
        
        ////////////////////Pas6: cookie/token din raspuns //////////////////////

        if( strncmp(command, "login", 5) == 0 ) {
            //verificare login cu succes si setare cookie
            response_code = error_code(response);
            if( response_code == 200 ) {
                logged = 1;
                // eliberare cookie de la seseiunea trecuta si salvare cookie
                free(cookie);       
                cookie = get_cookie_or_jwt(response, "cookie");

                //afisez cookie ul de sesiune
                printf("Cookie: %s\n", cookie);
            } 
        }

        if ( strncmp(command, "logout", 6) == 0 ) {
            //verificare succes logout + eliberare cookie
            response_code = error_code(response);
            if( response_code == 200 ) {
                logged = 0;
                free(cookie);
                cookie = NULL;
            
                //eliberare token de autentificare 
                free(token);
                token = NULL;
            } 
        }

        if ( strncmp(command, "enter_library", 13 ) == 0) {
         //verificare succes autentificare + parsare token
            response_code = error_code(response);
            if( response_code == 200 ) {
                free(token);
                token = NULL;
                token = get_cookie_or_jwt(response, "jwt");
            }
        }

        ////////////////////Pas7: eliberare memorie//////////////////////
        if ( response != NULL ) 
            free(response);
        if ( request != NULL )
            free(request);
        if ( input != NULL ) {
            for (int i = 0; i < 5; i++ ) {
                if (input[i] != NULL )
                    free(input[i]);
            }
            free(input);
        }
        printf("\n");
    }

    return 0;
}
