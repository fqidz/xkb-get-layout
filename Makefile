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

.DEFAULT_GOAL = xkb-layout-get

out:
	mkdir out

xkb-layout-get: out
	$(CC) $(ARGS) xkb-layout-get.c -o out/xkb-layout-get

clean:
	rm xkb-layout-get
