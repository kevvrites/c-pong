CC = emcc
INCLUDE_PATH = -I/Users/kevinliu/Desktop/raylib/src
LIBRARY_PATH = -L/Users/kevinliu/Desktop/raylib/src/web
LIBS = -lraylib -sUSE_GLFW=3 -sWASM=1 -sASYNCIFY
SRC = pong.c
OUT = pong.html
CFLAGS = -Os -Wall -std=c99 -D_DEFAULT_SOURCE -DPLATFORM_WEB

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(INCLUDE_PATH) $(LIBRARY_PATH) $(SRC) -o $(OUT) --shell-file custom_shell.html $(LIBS)

clean:
	rm -f $(OUT) pong.js pong.wasm
