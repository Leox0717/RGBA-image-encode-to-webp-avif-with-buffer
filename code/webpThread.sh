rm ./webpThread
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 

gcc -o webpThread webpThread.c /usr/local/lib/libwebp.so -lpthread

./webpThread 40
