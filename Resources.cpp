//////////////////////////////////////////////////////
//  EE 5123 - Computer Architecture (Spring 2017)
//
//  by: Patrick Stockton
//      
//
//  Project 2: A Simple Cache Performance Simulator
//
//////////////////////////////////////////////////////
//	Updates:
//		v1.0.00 - Added config and trace file selection functions
//      v1.0.01 - Read lines from config and trace files
//      v1.0.02 - Calculate number of sets, index, offset, and tag lengths
//      v1.0.03 - Implemented binary, hex, and decimal converters & initial test of hit and miss
//      v1.0.04 - Implemented set and block memories, and searches for LRU
//      v1.0.05 - Implemented error log of invalid conversion
//      v1.1.00 - Solved negative conversion, changed binToDec from long int to long long int (out of scope number)
//      v1.1.01 - Cleaned up code, fixed index issue
//      v1.1.02 - Created readTraceFile function to transfer functions from main function
//      -------------- end of LITE version --------------
//      -------------------------------------------------
//      --------------- WIN32 API Version ---------------
//      v2.0.00 - Created WIN32 API structure in "EE5123_win32.cpp" C++ file
//
//
//
//////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <cstring>

using namespace std;

void readConfigFile(string);
void readTraceFile(string);
long long int binaryToDecimalConv (string);
string hexToBinaryConv(string);
string hexTable(char);
void LRU_Update(int, int, int);


int lru_index[500][20];
int cache_size = 0;
int block_size = 0;
int associativity_value = 0;
int num_of_sets = 0;
int tag_length = 0;
int index_length = 0;
int offset_length = 0;
int num_of_blocks = 0;

int num_of_accesses = 0;
int hit_count = 0;
int miss_count = 0;
double hit_rate = 0.0;

ifstream config_infile;
ifstream trace_infile;

// Main function for file reading and result calculation
/*int main( ) {
    cout << "\n\nEE5123 - Computer Architecture (Spring 2017)" << endl;
    cout << "Project 2: A Simple Cache Performance Simulator" << endl;
    cout << "\nWritten by Patrick Stockton \n\n" << endl;

    readConfigFile( ); // read in cache parameters from file
    readTraceFile( ); // read in addresses from trace file

    return 0;

}   // end of main
*/

// Reads a configuration file
void readConfigFile(string filename) {
    //string filename = ""; // user selected input file
    cout << "=================================================================" << endl;
    cout << "====================== CONFIGURATION FILE ======================" << endl;
    cout << "=================================================================" << endl;

    ifstream infile(filename);

    // Test for invalid filename or failed to open
    if (!infile) {
        cout << "Error! The file was not found" << endl;
        exit(0);
    }
    string lineR = ""; // Line pulled from file
    string input_line = "";

    cout << endl;

    // Read file line by line and convert accordingly
    while (getline(infile, lineR)) {
        input_line = lineR;
        cout << input_line << endl;
        if (input_line.substr(0,5)== "Cache") {
            //cout << "CACHE FOUND!" << endl;
            //cout << input_line.substr(11) << endl;
            cache_size = atoi(input_line.substr(11).c_str());
            //cout << "Integer value of cache_size = " << cache_size << endl;
        }
        if (input_line.substr(0,5) == "Block") {
            //cout << "BLOCK FOUND!" << endl;
            //cout << input_line.substr(11) << endl;
            block_size = atoi(input_line.substr(11).c_str());
            //cout << "Integer value of block_size = " << block_size << endl;
        }
        if (input_line.substr(0,13) == "Associativity") {
            //cout << "ASSOCIATIVITY FOUND!" << endl;
            //cout << input_line.substr(14) << endl;
            associativity_value = atoi(input_line.substr(14).c_str());
            //cout << "Integer value of associativity_value = " << associativity_value << endl;
        }

    }

    infile.close( ); // Close the config file at EOF

    // Calculate number of sets, index length, offset length, and tag length
    num_of_sets = cache_size / (block_size * associativity_value);
    index_length = ceil(log(num_of_sets) / log(2));
    offset_length = ceil(log(block_size) / log(2));
    tag_length = 32 - index_length - offset_length;
    num_of_blocks = num_of_sets * associativity_value;
    block_size = cache_size / num_of_blocks;
    cout << "Number of sets = " << num_of_sets << endl;
    cout << "index_length = " << index_length << endl;
    cout << "offset_length = " << offset_length << endl;
    cout << "tag_length = " << tag_length << endl;
    cout << "num_of_blocks = " << num_of_blocks << endl;
    cout << "block_size = " << block_size << endl;
    cout << "\n" << endl;

} // end of readConfigFile


// Reads a trace file
void readTraceFile(string file_name) {

    long long int converted_address;
    //double hit_rate;
    //int num_of_accesses = 0;
    //int hit_count = 0;
    //int miss_count = 0;
    int set_val = 0;
    int address_tag = 0;
    int cache_pos = 0;  // Tracks position of "hit" in cache (for LRU mainly)
    bool address_found = false; // Tracks if a "hit" was found from the input address
    string input_line = "";
    string lineRead = "";
    int conversion_errors = 0; // Logs the amount of times a read-in conversion returned an error value
    //string file_name = "";

    cout << "=================================================================" << endl;
    cout << "========================== TRACE FILE ==========================" << endl;
    cout << "=================================================================" << endl;

    cout << "\n\n========================== PROCESSING... ==========================" << endl;
    ifstream infile(file_name);

    // Test for invalid filename or failed to open
    if (!infile) {
        cout << "Error! The file was not found" << endl;
        exit(0);
    }

    // Calculate number of blocks and sets
    //num_of_blocks = cache_size / block_size;
    //cout << "post-trace num_of_blocks = " << num_of_blocks << endl;
    //num_of_sets = cache_size / (associativity_value * block_size);
    //cout << "post-trace num_of_sets = " << num_of_sets << endl;

    // Create the cache
    int cache[num_of_sets][associativity_value];

    // Declaring loop variables outside to avoid multiple initial declarations
    int i = 0;
    int j = 0;

    // Initialize both cache and LRU indexes
    for (i = 0; i < num_of_sets; i++) {
        for (j = 0; j < associativity_value; j++) {
            cache[i][j] = 0;
            lru_index[i][j] = j;
        }
    }

    while(!infile.eof( )) { //Reading each address from trace file
        infile >> lineRead;
        input_line = lineRead.substr(2);
        converted_address = binaryToDecimalConv(hexToBinaryConv(input_line));
        //cout << "input_line = " << input_line << endl;
        //cout << "converted_address = " << converted_address << endl;
        if (converted_address < 0) { // Handles conversion errors
            //cout << "lineRead = " << lineRead << " || input_line = " << input_line << " || converted_address = " << converted_address << endl;
            converted_address = 0;
            conversion_errors++; // Increase conversion error count
        }

        num_of_accesses++;  // Increment access count
        address_found = false;  // Reset "hit" catch
        set_val = (converted_address / block_size) % num_of_sets; // Calculate set value given config parameters and sample hex address
        address_tag = converted_address / (block_size * num_of_sets); // Calculate tag value given config parameters and sample hex address

        // Check to see if the address tag from the input was found in the given respective set in the cache
        for(i = 0; i < associativity_value; i++) {
            if(cache[set_val][i] == address_tag) {
                cache_pos = i;
                address_found = true;
            }
        }

        if(address_found) {    // If a hit was found
           hit_count++;
           LRU_Update(set_val, associativity_value, cache_pos);
        }

        else { // If a hit was not found
           miss_count++;
           i = lru_index[set_val][0];   // update i to first LRU index value in the respective set
           LRU_Update(set_val, associativity_value, i); // Update LRU index with respective set, associativity value, and position
           cache[set_val][i] = address_tag; // Set respective set of cache and position with current tag
        }
    }

    infile.close( ); // Close the trace file at EOF
    num_of_accesses = num_of_accesses - 1;
    hit_count = hit_count - 1;
    cout << "\nNumber of accesses: " << num_of_accesses << endl;
    cout << "Number of hits: " << hit_count << endl;
    //cout << "Number of misses: " << num_of_accesses - hit_count << endl;
    cout << "Number of misses: " << miss_count << endl;
    hit_rate = (double(hit_count) / double(num_of_accesses)) * 100;
    cout << "Hit rate: " << hit_rate << "%" << endl;
    //cout << "Number of conversion errors: " << conversion_errors << endl;
    //cout << "Absolute Total (Hits, Misses, Conv. Errors): " << hit_count + num_of_accesses - hit_count + conversion_errors << endl;

}   // end of readTraceFile





/* ATTEMPT 1
// Reads a trace file
void readTraceFile( ) {
    string filename = ""; // user selected input file
    int counter = 0, cache_counter = 0;
    string hex_to_binary = "";
    int binary_to_decimal = 0;
    int hit_count = 0, miss_count = 0, previous_line = 0;
    double hit_rate = 0.0;

    int total_uses = 1;
    bool block_found = false;
    //int sets[num_of_sets][num_of_blocks];
    int sets[num_of_sets][associativity_value];
    int block[num_of_blocks][block_size];
    int block_uses[num_of_sets][associativity_value][total_uses];

    int index[num_of_sets][associativity_value];
    int index_uses[num_of_sets][associativity_value];


    cout << "=================================================================" << endl;
    cout << "========================== TRACE FILE ==========================" << endl;
    cout << "=================================================================" << endl;
    cout << "Please enter the name of your desired input file." << endl;
    cout << "If the input file is in the same folder, input the file name" << endl;
    cout << "If the input file is in a different directory, enter the directory/file address" << endl;
    cin >> filename;

    ifstream infile(filename);
    string lineR = ""; // Line pulled from file
    string input_line = "";
    int num_of_accesses = 0;

    cout << "How many lines do you wish to read? " << endl;
    cin >> num_of_accesses;

    // index
    for (int i = 0; i < num_of_sets; i++) {
        for (int j = 0; j < associativity_value; j++) {
            index[i][j] = 0;
        }
    }

    // index_uses
    for (int i = 0; i < num_of_sets; i++) {
        for (int j = 0; j < associativity_value; j++) {
            index_uses[i][j] = 0;
        }
    }


    while (getline(infile,lineR) && counter < num_of_accesses) {
        input_line = lineR;
        //cout << input_line << endl;
        counter++;

        hex_to_binary = hexToBinaryConv(input_line.substr(2));
        binary_to_decimal = binaryToDecimalConv(hex_to_binary);

        //cout << input_line << " -- " << "Set: " << binaryToDecimalConv(hex_to_binary.substr(19,7)) << " Block: " << binaryToDecimalConv(hex_to_binary.substr(0,19)) << endl;

        for(int i = 0; i < associativity_value; i++) {
            if(index[binaryToDecimalConv(hex_to_binary.substr(19,7))][i] == binaryToDecimalConv(hex_to_binary.substr(0,19))) {
                //cout << " HIT! " << endl;
                hit_count++;
                for (int k = 0; k < associativity_value; k++) {
                    index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][k]++; // Increase LRU age
                }
                index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][i] = 0; // Increase MRU value
                block_found = true;
                break;
            } else if (block_found == false) {
                //cout << "MISS!" << endl;
                miss_count++;
                int curr = 0;
                int prev = index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][0];
                for (int j = 0; j < associativity_value; j++) {
                    curr = index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][i];
                    if (curr < prev) {
                        prev = curr;
                    }
                }
                index[binaryToDecimalConv(hex_to_binary.substr(19,7))][prev] = binaryToDecimalConv(hex_to_binary.substr(0,19));
            }


        }
        if (block_found == false) {
                int prev = index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][0];
                int curr = 0;
                for (int i = 0; i < associativity_value; i++) {
                    //cout << "MISS!" << endl;
                    miss_count++;
                    //int curr = 0;
                    //int prev = index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][0];
                    //for (int j = 0; j < associativity_value; j++) {
                        curr = index_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][i];
                        if (curr > prev) {
                            prev = curr;
                        }
                    //}
                index[binaryToDecimalConv(hex_to_binary.substr(19,7))][prev] = binaryToDecimalConv(hex_to_binary.substr(0,19));
                }

            }
        block_found = false;

    }



    // Read file line by line and convert accordingly
    while (getline(infile,lineR) && cache_counter < cache_size) {
        input_line = lineR;
        //cout << input_line << endl;
        cache_counter++;

        hex_to_binary = hexToBinaryConv(input_line.substr(2));
        binary_to_decimal = binaryToDecimalConv(hex_to_binary);
        //cout << hex_to_binary << endl;
        //cout << hex_to_binary.substr(19,7) << endl;


        // Look for block in given set
        for (int i = 0; i < associativity_value; i++) {
            if (sets[binaryToDecimalConv(hex_to_binary.substr(19,7))][i] == binaryToDecimalConv(hex_to_binary.substr(0,19))) {
                cout << input_line << " -- " << binary_to_decimal << " -- HIT " << endl;
                hit_count++;
                block_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][binaryToDecimalConv(hex_to_binary.substr(0,19))][i]++;
                block_found = true;
            } else {
                block_found = false;
            }
        }

        if (block_found == false) {
            cout << input_line << " -- " << binary_to_decimal << " -- MISS " << endl;
            miss_count++;
            int uses_current = 0;
            int uses_prev = block_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][binaryToDecimalConv(hex_to_binary.substr(0,19))][0]; // fencepost
            for (int i = 0; i < associativity_value; i++) {   // Search through LRU block occurences
                uses_current = block_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][binaryToDecimalConv(hex_to_binary.substr(0,19))][i];
                if (uses_current < uses_prev) { // If LRU block in search
                    uses_prev = uses_current;
                }
            }
            sets[binaryToDecimalConv(hex_to_binary.substr(19,7))][uses_prev] = binaryToDecimalConv(hex_to_binary.substr(0,19)); // Replace the LRU block with the new
        } else {
            block_found = false;
        }
    }

    // Read file line by line and convert accordingly
    while (getline(infile,lineR) && counter < num_of_accesses - cache_size) {
        input_line = lineR;
        //cout << input_line << endl;
        counter++;
        hex_to_binary = hexToBinaryConv(input_line.substr(2));
        binary_to_decimal = binaryToDecimalConv(hex_to_binary);
        //cout << hex_to_binary << endl;
        //cout << hex_to_binary.substr(19,7) << endl;


        // Look for block in given set
        for (int i = 0; i < associativity_value; i++) {
            if (sets[binaryToDecimalConv(hex_to_binary.substr(19,7))][i] == binaryToDecimalConv(hex_to_binary.substr(0,19))) {
                cout << input_line << " -- " << binary_to_decimal << " -- HIT " << endl;
                hit_count++;
                block_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][binaryToDecimalConv(hex_to_binary.substr(0,19))][i]++;
                block_found = true;
            }
        }

        if (block_found == false) {
            cout << input_line << " -- " << binary_to_decimal << " -- MISS " << endl;
            miss_count++;
            int uses_current = 0;
            int uses_prev = block_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][binaryToDecimalConv(hex_to_binary.substr(0,19))][0]; // fencepost
            for (int i = 0; i < associativity_value; i++) {   // Search through LRU block occurences
                uses_current = block_uses[binaryToDecimalConv(hex_to_binary.substr(19,7))][binaryToDecimalConv(hex_to_binary.substr(0,19))][i];
                if (uses_current < uses_prev) { // If LRU block in search
                    uses_prev = uses_current;
                }
            }
            sets[binaryToDecimalConv(hex_to_binary.substr(19,7))][uses_prev] = binaryToDecimalConv(hex_to_binary.substr(0,19)); // Replace the LRU block with the new
        }



        if (binary_to_decimal - 8 == previous_line || binary_to_decimal + 8 == previous_line) {
            cout << input_line << " -- " << binary_to_decimal << " -- HIT " << endl;
            previous_line = binary_to_decimal;
            hit_count++;
        } else {
            cout << input_line << " -- " << binary_to_decimal << " -- MISS " << endl;
            previous_line = binary_to_decimal;
            miss_count++;
        }
        //cout << binary_to_decimal << endl;

    } //
    //hit_rate = (double)hit_count / (double)num_of_accesses + cache_size;
    hit_rate = (double)hit_count / (double)num_of_accesses;
    //cout << "Number of accesses: " << num_of_accesses + cache_size << endl;
    cout << "Number of accesses: " << num_of_accesses << endl;
    cout << "Number of hits: " << hit_count << endl;
    cout << "Number of misses: " << miss_count << endl;
    cout << "Hit rate: " << hit_rate * 100 << "%" << endl;

} // end of readTraceFile
*/



// Binary to Decimal Converter
// long int did not work, large hex values would return negative (int size was larger than long)
// Issue solved by changing function to long long int
long long int binaryToDecimalConv (string binary) {

    int count = 0;
    int size = binary.size() - 1;
    long long int decimal = 0;

    while (count < binary.size()) {
        if (binary[count] == '1') {
            decimal = (decimal + pow(2.0, size));
            //cout << "1 triggered = " << decimal << endl;
            //cout << "Decimal is :" << decimal << endl;
        }
        else {
            //decimal = (decimal + pow(0.0, count));
            decimal = decimal + 0;
            //cout << "0 triggered = " << decimal << endl;
        }
        size--;
        count++;
        //cout << "decimal iteration " << count << " = " << decimal << endl;
    }
    //cout << "Decimal representation of " << binary << " is: " << decimal << endl;

    return decimal;
} // end of binaryToDecimalConv


// Hexadecimal to Binary Converter
string hexToBinaryConv(string hex_value) {
    char hex_value_char = 'g';
    string converted_hex = "";  // Used to store resulted hex value
    for (int i = 0; i < hex_value.length(); i++) {
        hex_value_char = hex_value[i];
        //cout << hexTable(hex_value_char) << endl;
        converted_hex = converted_hex + hexTable(hex_value_char);
    }

    //cout << "Hex value " << hex_value << " converted to binary: " << converted_hex << endl;
    return converted_hex;
} // end of hexToBinaryConv



// Function for determining hex index value
string hexTable(char value) {
	switch (value) {
		case '0':
			return "0000";
			break;
		case '1':
			return "0001";
			break;
		case '2':
			return "0010";
			break;
		case '3':
			return "0011";
			break;
		case '4':
			return "0100";
			break;
		case '5':
			return "0101";
			break;
		case '6':
			return "0110";
			break;
		case '7':
			return "0111";
			break;
		case '8':
			return "1000";
			break;
		case '9':
			return "1001";
			break;
		case 'A':
			return "1010";
			break;
		case 'B':
			return "1011";
			break;
		case 'C':
			return "1100";
			break;
		case 'D':
			return "1101";
			break;
		case 'E':
			return "1110";
			break;
		case 'F':
			return "1111";
			break;
		default :
			cout << "ERROR";
			break;
	}
    return "";
} // end of hexTable


// Updates LRU index by updating order of the address tags
void LRU_Update(int set_val, int associativityVal, int index_pos)
{
    int curr = 0;

    for(int i = 0; i < associativityVal; i++) {
        if(lru_index[set_val][i] == index_pos) {
            curr = i;
        }
    }
    for(int i = curr; i < associativityVal - 1 ; i++) {
        lru_index[set_val][i] = lru_index[set_val][i+1]; // Move index up
    }
    // Move tag position up (MRU)
    lru_index[set_val][associativityVal - 1] = index_pos;
}   // end of LRU_Update


string cache_size_str = "Cache Size: ";
string cache_size_str_temp = "";
char cache_size_c[100] = "Cache Size: ";

string block_size_str = "Block Size: ";
string block_size_str_temp = "";
char block_size_c[100] = "Block Size: ";

string associativity_size_str = "Associativity: ";
string associativity_size_str_temp = "";
char associativity_size_c[100] = "Associativity: ";

string sets_size_str = "Number of Sets: ";
string sets_size_str_temp = "";
char sets_size_c[100] = "Number of Sets: ";

string block_num_str = "Number of Blocks: ";
string block_num_str_temp = "";
char block_num_c[100] = "Number of Blocks: ";

string tag_length_str = "Tag Length: ";
string tag_length_str_temp = "";
char tag_length_c[100] = "Tag Length: ";

string index_length_str = "Index Length: ";
string index_length_str_temp = "";
char index_length_c[100] = "Index Length: ";

string offset_length_str = "Offset Length: ";
string offset_length_str_temp = "";
char offset_length_c[100] = "Offset Length: ";

string accesses_str = "Number of accesses: ";
string accesses_str_temp = "";
char accesses_c[100] = "Number of accesses: ";

string hit_count_str = "Number of hits: ";
string hit_count_str_temp = "";
char hit_count_c[100] = "Number of hits: ";

string miss_count_str = "Number of misses: ";
string miss_count_str_temp = "";
char miss_count_c[100] = "Number of misses: ";

string hit_rate_str = "Hit rate: ";
string hit_rate_str_temp = "";
char hit_rate_c[100] = "Hit rate: ";
