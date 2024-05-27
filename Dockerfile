# Używamy oficjalnego obrazu Node.js w wersji 14 jako bazowego obrazu
FROM node:14

# Ustawiamy katalog roboczy w kontenerze
WORKDIR /app

# Kopiujemy pliki z projektu do katalogu roboczego w kontenerze
COPY package.json package-lock.json ./

# Instalujemy zależności
RUN npm install

# Kopiujemy resztę plików do katalogu roboczego w kontenerze
COPY . .

# Ustawiamy domyślny port, na którym będzie działać serwer
EXPOSE 3000

# Uruchamiamy serwer
CMD ["node", "server.js"]
