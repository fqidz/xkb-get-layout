CC = clang
ARGS = -xc\
	   -std=c99\
	   -O3\
	   -ferror-limit=0\
	   -Weverything\
	   -Werror\
	   -Wno-used-but-marked-unused\
	   -Wno-padded\
	   -Wno-declaration-after-statement\
	   -Wno-covered-switch-default\
	   -Wno-unsafe-buffer-usage\
	   -Wno-missing-prototypes\
	   -Wno-disabled-macro-expansion

.DEFAULT_GOAL = xkb-get-layout
OUTPUT_DIR = out

xkb-get-layout: $(OUTPUT_DIR)
	$(CC) $(ARGS) xkb-get-layout.c -o $(OUTPUT_DIR)/xkb-get-layout

clean:
	rm $(OUTPUT_DIR)
