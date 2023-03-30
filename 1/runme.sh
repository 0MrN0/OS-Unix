if [ ! -d build ]; then
    mkdir build
fi
if [ ! -f build/A ]; then
    python3 create_sparse_file.py
fi
make
echo -e "\n"

./build/1 build/A build/B
gzip -k build/A
gzip -k build/B
gzip -cd build/B.gz | ./build/1 build/C
./build/1 build/A build/D -b 100

stat build/A
echo -e "\n"
stat build/A.gz
echo -e "\nОжидаемое количество блоков - 24 (по 8 блоков на каждую 1 в файле А)."
echo -e "Ожидаемый размер файла В - такой же, как у файла А."
stat build/B
echo -e "\nИ размер, и количество блоков должны быть такими же, как и у A.gz"
stat build/B.gz
echo -e "\nИ размер, и количество блоков должны быть такими же, как и у В"
stat build/C
echo -e "\nКоличество блоков на 8 больше из-за нестандартного размера буфера (не равного степени 2)"
stat build/D
