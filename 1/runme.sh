if [ ! -d build ]; then
    mkdir build
fi
if [ ! -f build/A ]; then
    python3 create_sparse_file.py
fi
make

./build/1 build/A build/B
gzip -k build/A
gzip -k build/B
gzip -cd build/B.gz | ./build/1 build/C
./build/1 build/A build/D -b 100

stat build/A
stat build/A.gz
stat build/B
stat build/B.gz
stat build/C
stat build/D
