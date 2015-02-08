all: dz1z1 dz1z2 dz1z3 dz1z4 dz1z5 \
	dz2z1 dz2z2 dz2z3 dz2z4 dz2z5 \
	dz3z1 dz3z2 dz3z3 dz3z4 \
	dz3z6 dz3z7 dz3z8 dz3z9 \
	dz5z1 dz5z2 dz5z3 dz5z4 dz5z5 \
	dz6z1 dz6z2 dz6z3 dz6z4 dz6z5

CC = gcc
NVCC = nvcc
MPICC = mpicc
DEBUG = -g
CFLAGS = -Wint-to-pointer-cast $(DEBUG)
LFLAGS = -lpthread -lm

dz1z1: dz1z1.c
	$(CC) $(CFLAGS) dz1z1.c $(LFLAGS) -o dz1z1.exe

dz1z2: dz1z2.c
	$(CC) $(CFLAGS) dz1z2.c $(LFLAGS) -o dz1z2.exe

dz1z3: dz1z3.c
	$(CC) $(CFLAGS) dz1z3.c $(LFLAGS) -o dz1z3.exe

dz1z4: dz1z4.c
	$(CC) $(CFLAGS) dz1z4.c $(LFLAGS) -o dz1z4.exe

dz1z5: dz1z5.c
	$(CC) $(CFLAGS) dz1z5.c $(LFLAGS) -o dz1z5.exe

dz2z1: dz2z1.c
	$(CC) $(CFLAGS) dz2z1.c $(LFLAGS) -o dz2z1.exe

dz2z2: dz2z2.c
	$(CC) $(CFLAGS) dz2z2.c $(LFLAGS) -o dz2z2.exe

dz2z3: dz2z3.c
	$(CC) $(CFLAGS) dz2z3.c $(LFLAGS) -o dz2z3.exe

dz2z4: dz2z4.c
	$(CC) $(CFLAGS) dz2z4.c $(LFLAGS) -o dz2z4.exe

dz2z5: dz2z5.c
	$(CC) $(CFLAGS) dz2z5.c $(LFLAGS) -o dz2z5.exe

dz3z1: dz3z1.c
	$(MPICC) $(CFLAGS) dz3z1.c $(LFLAGS) -o dz3z1.exe

dz3z2: dz3z2.c
	$(MPICC) $(CFLAGS) dz3z2.c $(LFLAGS) -o dz3z2.exe

dz3z3: dz3z3.c
	$(MPICC) $(CFLAGS) dz3z3.c $(LFLAGS) -o dz3z3.exe

dz3z4: dz3z4.c
	$(MPICC) $(CFLAGS) dz3z4.c $(LFLAGS) -o dz3z4.exe

dz3z5: dz3z5.c
	$(MPICC) $(CFLAGS) dz3z5.c $(LFLAGS) -o dz3z5.exe

dz3z6: dz3z6.c
	$(MPICC) $(CFLAGS) dz3z6.c $(LFLAGS) -o dz3z6.exe

dz3z7: dz3z7.c
	$(MPICC) $(CFLAGS) dz3z7.c $(LFLAGS) -o dz3z7.exe

dz3z8: dz3z8.c
	$(MPICC) $(CFLAGS) dz3z8.c $(LFLAGS) -o dz3z8.exe

dz3z9: dz3z9.c
	$(MPICC) $(CFLAGS) dz3z9.c $(LFLAGS) -o dz3z9.exe

dz5z1: dz5z1.c
	$(MPICC) $(CFLAGS) dz5z1.c $(LFLAGS) -o dz5z1.exe

dz5z2: dz5z2.c
	$(MPICC) $(CFLAGS) dz5z2.c $(LFLAGS) -o dz5z2.exe

dz5z3: dz5z3.c
	$(MPICC) $(CFLAGS) dz5z3.c $(LFLAGS) -o dz5z3.exe

dz5z4: dz5z4.c
	$(MPICC) $(CFLAGS) dz5z4.c $(LFLAGS) -o dz5z4.exe

dz5z5: dz5z5.c
	$(MPICC) $(CFLAGS) dz5z5.c $(LFLAGS) -o dz5z5.exe

dz6z1: dz6z1.cu
	$(NVCC) dz6z1.cu -o dz6z1.exe

dz6z2: dz6z2.cu
	$(NVCC) dz6z2.cu -o dz6z2.exe

dz6z3: dz6z3.cu
	$(NVCC) dz6z3.cu -o dz6z3.exe

dz6z4: dz6z4.cu
	$(NVCC) dz6z4.cu -o dz6z4.exe

dz6z5: dz6z5.cu
	$(NVCC) dz6z5.cu -o dz6z5.exe

clean:
	rm -f *.exe
