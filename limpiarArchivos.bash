#!/usr/bin/env bash

# ─────────────| Sobre la entrada |─────────────¬

# Verificar que se ingresen exactamente 2 argumentos
if [ $# -ne 2 ]; then
    echo "Uso: bash $0 NOMBRE_ARCHIVO LIMITE"
    exit 1
fi

# Asignar argumentos a variables, elimina .fna de $1 si está presente
source="${1%.fna}"
limit="$2"

outfile="${source}_CLEAN"
limit_applied="${outfile}-SHORT"

# Verificar que el límite sea un entero
if ! [[ "$limit" =~ ^[0-9]+$ ]]; then
    echo "Error: LIMITE debe ser un numero entero."
    exit 1
fi

# Verifica si el archivo ya fue simplificado (solo utilizando el nombre)
if [[ "$source" =~ -SHORT ]]; then
    echo "Aviso: Archivo -SHORT ya existe."
    exit 0
fi



# ─────────────| Sobre outfile |─────────────¬

# No muestra(-v) cualquier linea que inicie(^) con '>' del archivo a elección
grep -v '^>' "${source}.fna" \

# Borra los nucleotidos ambiguos
| tr -d 'Nn' \

# Coloca los cambios en un archivo nuevo
> "${outfile}.fna"



# ─────────────| Sobre aplicar 'limit' |─────────────¬

# Se crea una versión acotada a 'limite' caracteres
head -c "$limit" "${outfile}.fna" > "${limit_applied}.fna"

# Convertir a mayúsculas el archivo con límite aplicado (utilizando archivo temporal)
tr 'atcg' 'ATCG' < "${limit_applied}.fna" > temp && mv temp "${limit_applied}.fna"



# ─────────────| Sobre la carpeta 'FASTAS' |─────────────¬

# Crea archivo si no existe (-p no da mensaje de error si es que ya está)
mkdir -p FASTAS

# Mueve los archivos utilizados/creados a la carpeta FASTAS
mv "${source}.fna" "${outfile}.fna" FASTAS

# "${limit_applied}.fna" se mantiene en el directorio actual

