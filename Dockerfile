# Uzycie obrazu gcc jako etapu budowania
FROM gcc AS builder

# Ustawienie etykiety autora
LABEL author = "Krystian Krukowski"

# Ustawienie katalogu roboczego w kontenerze
WORKDIR /app

# Skopiowanie plku server.c i binarnego pliku curl do kontenera
COPY server.c .
COPY curl .

# Statyczna kompilacja pliku server.c i utworzenie jego binarnej wersji
RUN gcc -static server.c -o server

# Nowy etap z pustym obrazem scratch
FROM scratch

# Skopiowanie binarnych plikow z etapu budowania
COPY --from=builder /app/ /

# Ustawienie Healthchecka
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 CMD /curl -fs http://localhost:8080 || exit 1

# Uruchomienie serwera po uruchomieniu kontenera
CMD ["/server"]