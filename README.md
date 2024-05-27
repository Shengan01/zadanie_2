Plik workflow opisuje automatyczny proces budowania, tagowania i publikowania obrazu Docker na DockerHub oraz skanowania tego obrazu pod kątem podatności, a następnie weryfikacji wyników skanowania. Jeśli obraz nie zawiera krytycznych ani wysokich podatności, jest również publikowany w rejestrze GHCR

``` yaml
- name: Docker Scout vulnerability scan
  id: scout
  uses: docker/scout-action@v1
  with:
    command: cves
    image: ${{ vars.DOCKERHUB_USERNAME }}/zadanie_2:${{ steps.meta.outputs.version }}
    sarif-file: sarif-output.json
    summary: true
```
Skanuje obraz Docker w poszukiwaniu podatności i zapisuje wyniki w formacie SARIF w pliku sarif-output.json.


``` yaml
- name: Install jq
  run: sudo apt-get install -y jq
```
Instalacja narzędzia jq do przetwarzania plików JSON

``` yaml
- name: Parse SARIF file
  id: parse-sarif
  run: |
    jq '.runs[].results' < sarif-output.json > scanning-results.json
```
Parsowanie wyników skanowania zapisanych w pliku sarif-output.json i zapisanie ich do scanning-results.json.

``` yaml
- name: Check vulnerability scan results
  id: check-scan
  run: |
    critical=$(jq '.[] | select(.level == "error") | .message' < scanning-results.json | wc -l)
    high=$(jq '.[] | select(.level == "warning") | .message' < scanning-results.json | wc -l)
    if [ "$critical" -ne 0 ] || [ "$high" -ne 0 ]; then
      echo "Image contains critical or high vulnerabilities. Skipping push to GHCR."
      exit 1
    else
      echo "No critical or high vulnerabilities found. Pushing image to GHCR."
      echo "${{ secrets.GITHUB_TOKEN }}" | docker login ghcr.io -u ${{ github.repository_owner}} --password-stdin
      docker pull shengan/zadanie_2:${{ steps.meta.outputs.version }}
      docker tag shengan/zadanie_2:${{ steps.meta.outputs.version }} ghcr.io/${{ vars.GH_USERNAME }}/zadanie_2:${{ github.sha }}
      docker push ghcr.io/${{ vars.GH_USERNAME }}/zadanie_2:${{ github.sha }}
    fi
```
Przy użyciu jq sprawdza liczbę krytycznych (error) i wysokich (warning) podatności w wynikach skanowania.
Jeśli znalezione są jakiekolwiek krytyczne lub wysokie podatności, proces publikacji obrazu do GHCR jest zatrzymany (exit 1).
Jeśli nie znaleziono żadnych krytycznych ani wysokich podatności, obraz jest logowany i publikowany do GHCR.
