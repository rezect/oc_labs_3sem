```bash
g++ src/new_computing.cpp -o obj/computing_process -lzmq -lfmt
g++ src/new_control.cpp -o obj/control_process -lzmq -lfmt
./obj/control_process
```