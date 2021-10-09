ring: ring.c
	musl-gcc -static ring.c -o ring

clean:
	rm ring || true
