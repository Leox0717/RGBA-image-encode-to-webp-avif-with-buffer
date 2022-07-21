rm ./avif_example_encode
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 

#gcc -o avif_example_encode avif_example_encode.c -L /usr/local/lib/ -l aom avif

gcc -o avif_example_encode avif_example_encode.c /usr/local/lib/libavif.a /usr/local/lib/libaom.a -lm -lpthread

./avif_example_encode 0 output.avif
