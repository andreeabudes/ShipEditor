**ShipEditor** este un editor de nave bazat pe un grid, dezvoltat în C++ folosind framework-ul OpenGL. Acesta permite utilizatorilor să proiecteze nave complexe prin **Drag & Drop** de elemente auxiliare, respectând în același timp un set riguros de constrângeri structurale și logice.

## Caracteristici Principale

### 1. Editor Vizual cu Drag & Drop
* **Interfață Intuitivă:** Panou lateral pentru selecția blocurilor și o grilă de construcție de 17x9.
* **Feedback în Timp Real:** Previzualizarea blocului atașat de cursor în timpul deplasării.
* **Gestionare Resurse:** Sistem de "vieți" (inimi) care limitează construcția la maxim 10 blocuri.

### 2. Tipuri de Blocuri Disponibile
Fiecare bloc are proprietăți vizuale și funcționale unice, randate prin geometrie procedurală (triunghiuri, cercuri, semicercuri):
* **Solid:** bloc structural de bază.
* **Tun:** cu o țeavă detaliată și bază semi-circulară.
* **Motor:** cu un efect de flacără dinamică formată din 4 triunghiuri.
* **Bumper:** formă arcuită


## Sistemul de Validare (Constrângeri)
Pentru a porni jocul, nava trebuie să fie validă. Butonul de **START** (steguletul) devine galben doar dacă:
1.  **Conexitate:** Toate blocurile trebuie să fie legate între ele (verificare prin algoritm **BFS**).
2.  **Integritate Motor:** Nu se pot plasa blocuri sub motor (pentru a lăsa loc flăcării).
3.  **Integritate Tun:** Nu se pot plasa blocuri deasupra tunului (linia de tragere liberă).
4.  **Siguranță Bumper:** Distanță minimă față de tunuri și interzicerea plasării blocurilor deasupra zonei de impact a bumperului.
5.  **Limită de Masă:** Maxim 10 blocuri per navă.
