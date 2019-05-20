#!/usr/bin/env python3

from threading import Thread
import subprocess

num_threads = 4

def func(x):
	for i in range(1, 201):
		if i % num_threads == x:
			print("./qbfpt.out " +  str(i))
			subprocess.call(["./qbfpt.out", str(i)])

threads = [] 
for i in range(num_threads):
    thread = Thread(target=func, args=(i,))
    threads.append(thread) 
    thread.start() 
 
for x in threads: 
    x.join()