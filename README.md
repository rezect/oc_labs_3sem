Для подключения во время линковки (лучший вариант)
```bash
g++ -fpic -shared src/libgeometry_a.cpp -o libs/libgeometry_a.so
g++ -fpic -shared src/libgeometry_b.cpp -o libs/libgeometry_b.so
# Для первой библиотеки
g++ linktime.cpp -o linktime -Llibs/ -lgeometry_a -Wl,-rpath=libs/
./linktime
# Для второй библиотеки
g++ linktime.cpp -o linktime -Llibs/ -lgeometry_b -Wl,-rpath=libs/
./linktime
```

Для подключения во время исполнения
```bash
g++ -shared -fPIC -o libs/libgeometry_a.so src/libgeometry_a.cpp
g++ -shared -fPIC -o libs/libgeometry_b.so src/libgeometry_b.cpp
g++ runtime.cpp -o runtime
./runtime
```