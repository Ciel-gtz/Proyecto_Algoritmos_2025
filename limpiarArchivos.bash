#!/usr/bin/env bash

# ─────────────| Sobre la entrada |─────────────¬

# Verificar que se ingrese exactamente 1 argumento
if [ $# -ne 1 ]; then
    echo "Uso: bash $0 NOMBRE_ARCHIVO"
    exit 1
fi

# Asignar argumentos a variables, elimina .fna de $1 si está presente
source="${1%.fna}"
limit="$2"

# Mostrar archivo seleccionado
echo "Archivo seleccionado: '${source}.fna'"

# Verifica si el archivo fuente existe y que se pueda reescribir
if [[ ! -r "${source}.fna" ]]; then
    echo "Error: No se encuentra el archivo '${source}.fna' o no es legible."
    exit 1
fi

# Verifica si el archivo ya fue simplificado (solo utilizando el nombre)
if [[ "$source" =~ _CLEAN-SHORT ]]; then
    echo "Aviso: Se seleccionó archivo ya limpiado (con formato '_CLEAN-SHORT')."
    exit 2
fi


# Verificar que el límite sea un entero
while true; do
    # Pedir límite al usuario 
    echo "Ingresa la cantidad de caracteres deseados para su cadena"
    read -p "Escriba '0' si quiere que se lea todo el archivo: " limit

    # Validar entero
    if [[ "$limit" =~ ^[0-9]+$ ]]; then
        break
    fi

    echo "Error: Debes ingresar un número entero."
done


# Después de las verificaciones, define nombres de archivos de salida
outfile="${source}_CLEAN"
limit_applied="${outfile}-SHORT"


# ─────────────| Sobre outfile |─────────────¬

# No muestra(-v) cualquier linea que inicie(^) con '>' del archivo a elección
grep -v '^>' "${source}.fna" > "${outfile}.fna"

# Convertir a mayúsculas el archivo
tr 'atcg' 'ATCG' < "${outfile}.fna" > temp && mv temp "${outfile}.fna"

# Elimina cualquier caracter que no sea A, T, C o G (ej: nucleotidos ambiguos)
tr -cd 'ATCG' < "${outfile}.fna" > temp && mv temp "${outfile}.fna"


# ─────────────| Sobre aplicar 'limit' |─────────────¬

if [[ "$limit" -eq 0 ]]; then
    # No aplicar límite, usar archivo completo
    cp "${outfile}.fna" "${limit_applied}.fna"
else
    # Se crea una versión acotada a 'limite' caracteres
    head -c "$limit" "${outfile}.fna" > "${limit_applied}.fna"
fi



# ─────────────| Sobre la carpeta 'FASTAS' |─────────────¬

# Crea archivo si no existe (-p no da mensaje de error si es que ya está)
mkdir -p FASTAS

# Mueve el archivo source y el archivo outfile a la carpeta FASTAS
mv "${limit_applied}.fna"  "${outfile}.fna" FASTAS
echo ""

# "${source}.fna" se mantiene en el directorio actual