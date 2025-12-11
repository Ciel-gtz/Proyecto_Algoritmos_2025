reference sequences from: 
https://www.ncbi.nlm.nih.gov/datasets/genome/GCF_000001405.40/
(comparing human to...)
https://www.ncbi.nlm.nih.gov/datasets/genome/GCF_036323735.1/
(common rat)

para correrlo fácil:
g++ needlemanWunsch.cpp -o needlemanWunsch && ./needlemanWunsch -C1 HUMAN.fna -C2 RAT.fna -U matrizPuntuacion.csv -V 2

`userDecision()` viene de combinar los códigos presentes en [esta página](https://stackoverflow.com/questions/43972500/how-to-only-accept-y-or-n-in-users-input-in-c),

para generar la imagen se necesita Imagemagick con pango, como instalarlo: