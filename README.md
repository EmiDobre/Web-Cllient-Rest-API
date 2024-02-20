DOBRE EMILIA ILIANA - 323 CB

---Flow program:--- 

1) Comenzi si validarea lor: 
    - se introduc comenzi de la tastatura si se verifica daca acestea sunt valide

    - comenzile date de user vor face o cerere la server de tip HOST, POST, sau DELETE. Pentru ca unele comenzi sa se execute este nevoie de login. 

    - astfel fiecare comanda are un tip de cerere pe care il face si are sau nu nevoie de login 


2) Verificare login:
    - pentru comenzile care au nevoie de login se verifica daca sunt deja logate, pentru cele care nu trebuie sa aiba login se verifica sa nu fie deja logat


3) Citire input:
    - unele comenzi au nevoie de date suplimentare de la stdin pe care le retin in vectorul de stringuri dinamic, input
    - pentru fiecare input verific daca aceste este valid: id-ul este numar, stringul nu are caractere fara sens


4) Construire cerere:
    - daca inputul este valid se construieste cererea in functie
    de tipul cerut 

    -> GET/DELETE:
        * se apeleaza command_GET si la tip GET si DELETE pentru ca este acelasi mesaj, singura linie care se modifica din cerere este prima linie

        * in functie de comanda se completeaza url ul si apoi se apeleaza functia din laborator compute_get_request care are in plus tipul de cerere: GET sau DELETE

    -> POST:
        * in functie de comanda se completeaza url ul si se apeleaza functia din lab compute_post_request cu inputul pe post de body_data

        * se completeaza liniile mesajului si se parseaza body ului lui folosind biblioteca parson.c in functia parseJSON_body care transforma inputul in obiect json 


5) Conexiunea cu serverul + Raspuns :
    - se trimite cererea construita anterior la server prin socket
    - se primeste raspunsul de la server si daca acesta este null este posibil sa fi pierdut conexiunea cu serverul, asa ca o restabilesc 

    - raspunsul este afisat prin functia care parseaza raspunsul: afiseaza codul de eroare/succes si daca exista unul sau mai multe obiecte json in raspuns le afiseaza prin functiile: 
            * extractJSON 
                        -> primeste un string sub forma de un singur
                            obiect JSON
                        -> extrage campurile obiectului JSON si le afiseaza 
            * extractJSONObjects 
                        -> primeste un string sub forma de lista de mai multe obiecte JSON[ obiect, obiect ...]
                        -> extrage fiecare obiect json din lista il retine intr-un array de json 
                        -> fiecare obiect json din array se serializeaza intr-un string pe care aplic functia extractJSON


6) Cookie/Token:
    - pentru unele comenzi, raspunsul contine cookie sau token astfel initializez campul de cookie sau token prin functia care parseaza raspunsul si ia tokenul sau cookie ul din raspuns get_cookie_or_jwt
    - cand se da logout trebuie sa eliberez cookie ul sesiunii trecute si tokenul


7) Eliberare memorie:
    - eliberez memoria folosita pentru raspuns, cerere si input
