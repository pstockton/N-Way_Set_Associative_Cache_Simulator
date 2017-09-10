# N-Way_Set_Associative_Cache_Simulator
This is a simple project based around a N-Way Set Associative Cache Simulator, using C++.

EE 5123 - Computer Architecture.

By Patrick Stockton.


---------------------README CONTENTS---------------------

Section 1

	a): Compiling Instructions
  
	b): Executions Instructions

Lite Version (for non-Window's OS based user)

Section 2

	a): Sample Results
  
	b): Implementation Descriptions



---------------------SECTION 1---------------------

There should be 2 C++ files enclosed with this README file.

To compile and successfully execute the program, please do the following:


1): Compile and run the C++ file "PatrickStockton_CacheSim_Win32.cpp" with any C++ compiler that you have.


  > For example, linux you can use "g++ -o CacheSim PatrickStockton_CacheSim_Win32.cpp" or any other IDE.


2): After compiling the program, run the executable


3): You will be prompted with a Window and command prompt. The first window provides a brief description of the program.

After reading the description, press the "Start" button.

![start program](https://user-images.githubusercontent.com/3781470/30253264-9018d756-9636-11e7-8705-c076045679ef.PNG)


After pressing the "Start" button, you will be presented with a window that has two text fields on the left.

Following the label of each field, enter the configuration file name on the first field, and the trace file name on the second field.
> In this example, the configuration file is named "example.cfg" and the trace file is named "example.trc".

![2 - enter files](https://user-images.githubusercontent.com/3781470/30253269-b5d625a2-9636-11e7-850d-eb181627a6ab.PNG)


Once both fields are entered, press the "Run" button at the bottom.

The simulation will begin, and the results will be displayed on the right side of the window, a message box should appear to confirm the simulation has completed.

![3 - run sim](https://user-images.githubusercontent.com/3781470/30253281-d209f87a-9636-11e7-8ea6-466453588278.PNG)

![4 - sim complete](https://user-images.githubusercontent.com/3781470/30253283-da4f56ba-9636-11e7-98d4-3c9365e20647.PNG)


You can run multiple simulations with different configuration and trace files (be sure to enter the new file names for each simulation).

NOTE: You can set your own configuration parameters by editting the .cfg file.

![config file contents](https://user-images.githubusercontent.com/3781470/30253288-f0a60b34-9636-11e7-8dac-337c62ac03d9.PNG)

NOTE: If you wish to use your own trace file, please refer to the format below:

![trace file contents](https://user-images.githubusercontent.com/3781470/30253292-f72ca256-9636-11e7-9c46-76d6cd6dc839.PNG)



## LITE VERSION:

If you do not have a windows OS, or the above program is not working, I have provided a "lite" command prompt version of the program.

Follow the above steps (1) and (2).

After running the program, you will be prompted to enter the configuration file name on the command prompt.

![lite config](https://user-images.githubusercontent.com/3781470/30253318-63c1ea66-9637-11e7-94b1-9fadb06fdff1.PNG)

After hitting enter (return), you will be prompted to enter the trace file name on the command prompt.

![lite trace](https://user-images.githubusercontent.com/3781470/30253325-6ca4327e-9637-11e7-9776-d41e897e1681.PNG)


The results of the simulation will be displayed after the processing has been completed.



---------------------SECTION 2---------------------

Below are some sample results from different tests of my program.

**--- Sample 1 ---**

  >Cache Size: 32768

  >Block Size: 64

  >Associativity: 4

  >Accesses: 695521

  >Hits: 594142

  >Misses: 101379

  >Hit-Rate: 85.424%
  
**--- Sample 2 ---**
>Cache Size: 50000

>Block Size: 64

>Associativity: 4

> Accesses: 695521

> Hits: 595295

> Misses: 100226

>Hit-Rate: 85.5898%

**--- Sample 3 ---**
>Cache Size: 50000

>Block Size: 256

>Associativity: 6

>Accesses: 695521

>Hits: 623587

>Misses: 71934

>Hit-Rate: 89.6575%

**--- Sample 4 ---**
>Cache Size: 12000

>Block Size: 64

>Associativity: 4

>Accesses: 695521

>Hits: 556661

>Misses: 138860

>Hit-Rate: 80.0351%

**--- Sample 5 ---**
>Cache Size: 25000

>Block Size: 42

>Associativity: 3

>Accesses: 695521

> Hits: 517067

> Misses: 178454

> Hit-Rate: 74.342399%



## Design and Implementation Methods

I took a few approaches to designing the programming (for example, the Win32 or the Lite versions).

Below are brief descriptions of some functions and how they interact with the program:


**string hexToBinaryConv(string)**

- After reading in hex number from file, convert the hex value to binary for processing.


**long long int binaryToDecimalConv(string)**

- Converts the binary value to decimal. Needed to incorporate the "long long" for large values (large values were being converted to negatives due to being outside of the int scope).

**void readConfigFile(string)**

- Reads lines from config file and calculates number of sets, tag length, offset length, etc.

**void readTraceFile(string)**

- Reads each line of the trace file.

- For each hex line, compare if the hex address's tag is found in it's respective set in the cache.

- If the tag is found, then count it as a hit, and adjust LRU to MRU value.

- If the tag was not found, count it as a miss, adjust the LRU to the current position, and set the respective set in the cache as the tag.

**LRESULT CALLBACK SimulationWindowProcedure(HWND, UINT, WPARAM, LPARAM)**

- Used to create the simulation window in the Win32 API.

- Buttons, text fields, and interactions are created here (calls to the above functions)


Other supporting functions can be found in the source code, I wanted to try to keep this README short.

---------------------------------------------------------

Best of luck, and thank you!
