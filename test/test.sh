#! /bin/bash
[ -e ./calico_log.txt ] && rm ./calico_log.txt
[ -e ./valgrind_log.txt ] && rm ./valgrind_log.txt

[ -e ../src/main ] && rm ../src/main

calico test.yaml |& tee calico_log.txt

[ -e ../src/main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ../src/main 1 |& tee -a valgrind_log.txt
[ -e ../src/main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ../src/main 2 |& tee -a valgrind_log.txt
[ -e ../src/main ] && valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all ../src/main 3 |& tee -a valgrind_log.txt