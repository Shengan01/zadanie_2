#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080


// Funkcja log_info wypisuje informacje o uruchomieniu serwera
void log_info() {
    time_t now = time(NULL);
    printf("Serwer uruchomiony przez: Krystian Krukowski\n");
    printf("Data uruchomienia: %s", ctime(&now));
    printf("Nasluchiwanie na porcie: %d\n", PORT);
}

// Funkcja hande_client obslugujaca polaczenie z klientem
void handle_client(int client_socket) {
    char buffer[1024] = {0};
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    getpeername(client_socket, (struct sockaddr *)&addr, &addr_size);
    char *client_ip = inet_ntoa(addr.sin_addr);

    time_t now = time(NULL);
    char *time_str = ctime(&now);

    char response[1024];
    snprintf(response, sizeof(response), "Adres IP klienta: %sAktualna data i godzina w Twojej strefie czasowej: %s", client_ip, time_str);
    
    char http_header[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    // Wyslanie naglowka HTTP
    if (send(client_socket, http_header, strlen(http_header), 0) < 0) {
        perror("send header failed");
        close(client_socket);
        return;
    }
    // Wyslanie odpowiedzi HTTP
    if (send(client_socket, response, strlen(response), 0) < 0) {
        perror("send response failed");
    }
    close(client_socket);
}

int main() {
    // Ustawienie buforowania wyjscia na NULL aby zapewnic niebuforowane wyjscie (Bez tego niemozliwym bylo odczytanie logow serwera)
    setbuf(stdout, NULL);
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    
    // Ustawienie socketu serwera
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Ustawienie opcji socketu
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    // Ustawienie parametrow adresu serwera
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // Powiazanie socketu z adresem i portem
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // Nasluchiwanie na polaczenia przychodzace
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Wypisanie informacji o uruchomieniu serwera
    log_info();

    // Petla obslugujaca polaczenia
    while (1) {
        // Akceptowanie polaczenia z klientem
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            continue;
        }
        // Obsluga polaczenia
        handle_client(new_socket);
    }
    
    return 0;
}
