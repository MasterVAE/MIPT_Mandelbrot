g++ src/main.cpp src/render_v1.cpp -O2 -Iinclude -DNDEBUG -march=native -lraylib -lm -o build/v1.out
g++ src/main.cpp src/render_v2.cpp -O2 -Iinclude -DNDEBUG -march=native -lraylib -lm -o build/v2.out
g++ src/main.cpp src/render_v3.cpp -O2 -Iinclude -DNDEBUG -march=native -lraylib -lm -o build/v3.out