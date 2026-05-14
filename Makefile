CC = clang
ARGS = -xc\
	   -std=c99\
	   -O3\
	   -ferror-limit=0\
	   -Weverything\
	   -Wno-used-but-marked-unused\
	   -Wno-padded\
	   -Wno-declaration-after-statement\
	   -Wno-covered-switch-default\
	   -Wno-unsafe-buffer-usage\
	   -Wno-missing-prototypes\
	   -Wno-disabled-macro-expansion

.DEFAULT_GOAL = xkb-get-layout

xkb-get-layout: out
	$(CC) $(ARGS) xkb-get-layout.c -o out/xkb-get-layout

clean:
	rm xkb-get-layout
