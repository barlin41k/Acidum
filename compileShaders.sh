#!/bin/bash

echo "======================================="
echo "       VULKAN SHADERS TO SPIR-V        "
echo "======================================="

if ! command -v glslc &> /dev/null
then
    echo "Ошибка: glslc не найден!"
    exit 1
fi

for file in *.vert *.frag; do
    [ -f "$file" ] || continue
    
    echo "Compiling: $file -> ${file%.*}.spv"
    glslc "$file" -o "${file%.*}.spv"
    
    if [ $? -ne 0 ]; then
        echo "Ошибка компиляции в файле $file"
        exit 1
    fi
done

echo "======================================="
echo "               SUCCESS!                "
echo "======================================="