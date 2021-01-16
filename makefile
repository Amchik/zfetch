src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS += -Wall

zfetch: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) zfetch

.PHONY: check
check: zfetch
	mkdir -p .home/
	HOME=$(PWD)/.home ./zfetch --regenerate-all
	HOME=$(PWD)/.home ./zfetch
	rm -r .home/
