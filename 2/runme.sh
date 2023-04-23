make clean
make
rm -f results.txt

for i in {1..10}; do
    ./build/2 ./build/myfile.txt &
    pids[${i}]=$!
done

sleep 5m

for pid in ${pids[*]}; do
    kill -s 2 $pid
done
