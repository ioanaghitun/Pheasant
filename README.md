# Pheasant
### Proiect la disciplina "Rețele de Calculatoare" (2017-2018)

### &#128194; Obiective:
* Să se realizeze o aplicație ce va implementa jocul fazan.
* Într-un fișier de configurare se va specifica numărul de jucători. Va exista un fișier de tip dicționar ce va fi folosit pentru validarea cuvintelor.
* Jucătorii vor fi aleși în ordinea conectării utilizatorilor la server.
* Primul jucător alege o literă și un cuvânt ce începe cu litera respectivă. Următorul utilizator va trebui să trimită un cuvânt ce va începe cu ultimele două litere din cuvântul jucătorului precedent.
* Când cuvântul trimis de un utilizator nu face parte din dicționar, jucătorul a pierdut și va fi deconectat. Utilizatorul care rămâne la sfarșit câștigâ jocul.

### &#9881; Tehnologiile utilizate:
* Server TCP concurent cu N clienți
* Multiplexare I/O via select()
* Dicționar via <http://www.bistritza.ro/isc.html>
