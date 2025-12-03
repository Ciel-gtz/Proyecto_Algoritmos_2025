##### AUN NO TERMINO ESTE POR COMPLETO


# Verificar que se ingresen exactamente 2 argumentos
if [ $# -ne 2 ]; then
    echo "Uso: bash $0 FILENAME LIMITE"
    exit 1
fi

source="$1"
limit="$2"
outfile="${source}_CLEAN"

# Verificar que el límite sea un número entero
if ! [[ "$limit" =~ ^[0-9]+$ ]]; then
    echo "Error: LIMITE debe ser un número entero."
    exit 1
fi




### CAMBIAR ESTO DE ABAJO DSP

# No muestra(-v) cualquier linea que inicie(^) con '>' del archivo a elección
grep -v '^>' "${source}.fna" \

# Cambia las minusculas por mayusculas
| tr 'atcg' 'ATCG' \

# Elimina los nucleotidos ambiguos (N)
| tr -d 'Nn'  \

# Coloca los cambios en un archivo nuevo
> "${outfile}.fna"

# De ese archivo nuevo se crea una versión acotada a ciertos caracteres
head -c 30 "${outfile}.fna" > "${outfile}-SHORT.fna"

# Crea archivo si no existe (-p no da mensaje de error si es que ya está)
mkdir -p FASTAS

# Mueve los archivos utilizados/creados a la carpeta FASTAS
mv "${source}.fna" "${outfile}.fna" FASTAS

# Deja "${outfile}-SHORT.fna" en el directorio actual

