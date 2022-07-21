rm ./rgba2Jp2
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 

gcc -o rgba2Jp2 rgba2Jp2.c /usr/local/lib/libopenjp2.so

./rgba2Jp2
