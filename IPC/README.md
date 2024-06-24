# Ipc</div>
Task 2 : Linux practice - Operating systems course.</div>

## Project Description:</div>
The task is made of 2 parts: stnc (st for students, nc for network communication), network performance test utility.</div>

### **Part A : stnc**</div>

We implemented chat cmd tool:</div>

1. **stnc -** The tool ("chat" cmd) that can send messages over the network, to the same tool ,listening on the
other side, and get the response, so there will be 2 sides communication, simultaniosly communication will be done using IPv4 TCP protocol.</div>
</div>

### **Part B : network performance test utility**</div>
We implemented two coding libraries:</div>
1. Generate a chunk of data, with 100MB size. </div>
2.  Generate a checksum (hash) for the data above. </div>
3.  Transmit the data with selected communication style, while measuring the time it takes.</div>
4.  Report the result to stdOut.</div>

## Building</div>
1. Cloning the repo to local machine: ` git clone` https://github.com/DorHarizi/Task2_Ipc.git </div>
2. Building all the necessary files & the main programs:  `make` </div>


## communications styles are:</div>
1. ipv4 tcp </div>
2. ipv4 udp </div>
3. ipv6 tcp </div>
4. ipv6 udp </div> 
5. uds dgram </div>
6. uds stream </div> 
7. mmap filename </div>
8. pipe filename </div>

## How to run? </div>
- Run the chat tool server: `./stnc -s PORT` </div>
- Run the chat tool client : `./stnc -c IP PORT` </div>
## for performance test server we have flag p and q to quiet:
- Run the performance test server : `./stnc -s port -p (p for performance test) -q (q for quiet)` </div>
## for performance test client we have flag p, type(ipv4, ipv6, mmap, pipe,uds), param(udp, tcp, dgram, stream, file name):
- Run the performance test client :` ./stnc -c IP PORT -p <type> <param>`  </div>
</div>

## Author: </div>
[Dor Yanay](https://github.com/DorYanay "Dor Yanay") **&** [DorHarizi](https://github.com/DorHarizi "DorHarizi")
