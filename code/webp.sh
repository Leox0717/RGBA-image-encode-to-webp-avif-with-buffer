rm ./rgba2webp
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 

gcc -o rgba2webp rgba2webp.c /usr/local/lib/libwebp.so

./rgba2webp
