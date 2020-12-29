src = $(wildcard *.c)
obj = $(src:.c=.o)

CFLAGS += -Wall

zfetch: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) zfetch
