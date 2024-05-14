#! /bin/bash
[ -e ./calico_log.txt ] && rm ./calico_log.txt
[ -e ./valgrind_log.txt ] && rm ./valgrind_log.txt

[ -e ../src/main ] && rm ../src/main

calico ../src/test.yaml |& tee calico_log.txt

cd ../src

[ -e ./main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 1 |& tee -a ../test/valgrind_log.txt
[ -e ./main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 2 |& tee -a ../test/valgrind_log.txt
[ -e ./main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 3 |& tee -a ../test/valgrind_log.txt