Artur Grudkowski
SOI
Lab6 – system plików

1.Koncepcja rozwiązania
Proponowany przeze mnie system plików będzie się składał z bloków. Wyróżniam trzy rodzaje bloków:
• Super-blok
• Węzeł
• Blok z danymi
Super-blok(istnieje tylko jeden na początku systemu plików) będzie zawierał podstawowe informacje od dysku m.in.. jego nazwę i rozmiar zarówno ten maksymalny dostępny do użytku jak i aktualny rozmiar wolnej pamięci podczas normalnego procesu użytkowego.

Węzły będą zawierały nazwę aktualnie przechowywanego pliku (ograniczona dł. nazwy do 256 znaków), flagi określające zajętość pliku i czy dany blok pamięci, który odpowiada danemu węzłowi jest kontynuacją poprzedniego bloku czy nie( może wystąpić sytuacja, że plik nie zmieści się w danym bloku i będzie trzeba użyć co najmniej jednego bloku więcej), poza tym w węźle będziemy przechowywali rozmiar jaki jest używany przez dany plik w bloku pamięci odpowiadającemu danemu węzłowi. Przewiduję jeszcze, że będę przechowywał numer następnego węzła, ponieważ może się zdarzyć sytuacja, że początek pliku zostanie wpisany do pewnego węzła, cały plik nie zmieści się w danym bloku pamięci a następny z kolei blok będzie zajęty przez inny plik( oczywiście nie wystąpi sytuacja że plik w ogóle nie zmieści się na naszym wirtualnym dysku bo to będzie sprawdzane wcześniej ) wtedy bloki pamięci będą od siebie oddzielone.

Blok z danymi będzie przechowywał dane, będzie miał numer/indeks odpowiadający opisującemu go węzłowi.
Oczywiście na systemie plików będzie możliwość wykonywania wszystkich działań określonych w wymaganiach m.in. tworzenie/usuwanie dysku, otwieranie/zamykanie, kopiowanie w dwie strony itp.
Plik, które powstaną poza main’em i biblioteką obsługującą system plików to pliki odpowiedzialne za używanie narzędzi(podobnie jak w klasycznym unix’ie) m.in. cp, rm.

2.Testowanie
Tak jak napisano w instrukcji na stronie, zostaną stworzone dwa pliki, na których zostaną zaprezentowane operacje dostępne w systemie plików.
