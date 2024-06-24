# Chat supports an unlimited members using Reactor Design pattern </div>
Task 3 : Linux practice - Operating systems course.</div>

## Project Description:</div>
The reactor of the design pattern according like that: </div>
a) you implement a data structure that will map between the function and the FD </div>
b) you implement the mechanism and perform FD to listen to all the (select or poll) that is responsible for the selector </div>
   reactor the operation listed in the table of the </div>

### API of the reactor:</div>

1. **void createReactor()** 
</div>

2. **void stopReactor(void this)** 
</div>

3. **void startReactor(void this)** 
</div>

4. **void addFd(void this,int fd, handler_t handler)** 
</div>

5. **void WaitFor(void this)** 
</div>

## Building</div>
1. Cloning the repo to local machine: ` git clone` [https://github.com/DorHarizi/Chat-using-ds-Reactor.git] </div>
2. Building all the necessary files & the main programs:  `make` </div>
3. Export shared libraries : `export LD_LIBRARY_PATH=.` </div>
</div>

## How to run? </div>
- Run the reactor server: `./react_server </div>

## Author: </div>
[Dor Yanay](https://github.com/DorYanay "Dor Yanay") **&** [DorHarizi](https://github.com/DorHarizi "DorHarizi")</div>
