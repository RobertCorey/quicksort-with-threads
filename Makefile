all: 
	gcc threadSort.c -pthread -std=c99 -o ThreadSort
clean:
	rm ThreadSort
	rm output.txt