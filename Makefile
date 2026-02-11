all:
	gcc -DXA_DEBUG -g -o test_xarray test_xarray.c xarray.c bitmap.c -I ./ -lpthread
clean:
	rm -f test_xarray
