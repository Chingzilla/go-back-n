tar_files = src/*.c src/*.h src/Makefile README.md

all:

tar: ${tar_files}
	tar -cvf ps02-gbn-ching-rana.tar.bz2 --xform 's|^|ps02-gbn/|' ${tar_files}

.PHONEY: all tar
