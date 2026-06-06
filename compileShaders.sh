#!/bin/bash

OUTPUT_DIR=${1:-"shaders"}

echo "======================================="
echo "       VULKAN SHADERS TO SPIR-V        "
echo "======================================="
echo "Output directory: $OUTPUT_DIR"

if ! command -v glslc &> /dev/null
then
    echo "Ошибка: glslc не найден!"
    exit 1
fi

mkdir -p "$OUTPUT_DIR"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
SHADERS_DIR="$SCRIPT_DIR/shaders"

for file in "$SHADERS_DIR"/*.vert "$SHADERS_DIR"/*.frag; do
    [ -f "$file" ] || continue
    
    filename=$(basename "$file")
    name_no_ext="${filename%.*}"
    outfile="$OUTPUT_DIR/${name_no_ext}.spv"
    
    echo "Compiling: $filename -> $outfile"
    glslc "$file" -o "$outfile"
    
    if [ $? -ne 0 ]; then
        echo "Ошибка компиляции в файле $filename"
        exit 1
    fi
done

echo "======================================="
echo "               SUCCESS!                "
echo "======================================="