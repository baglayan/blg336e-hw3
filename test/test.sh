#! /bin/bash
[ -e ./calico_log.txt ] && rm ./calico_log.txt
[ -e ./valgrind_log.txt ] && rm ./valgrind_log.txt

[ -e ../src/main ] && rm ../src/main

cd ../src
calico test.yaml |& tee ../test/calico_log.txt


[ -e ./main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 1 |& tee -a ../test/valgrind_log.txt
[ -e ./main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 2 |& tee -a ../test/valgrind_log.txt
[ -e ./main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ./main 3 |& tee -a ../test/valgrind_log.txt