//////////////////////////////////////////////////////////////////////////////////////
//      EE 5123 - Computer Architecture
//
//  by: Patrick Stockton
//      
//
//  Project 2: A Simple Cache Performance Simulator
//
//
//////////////////////////////////////////////////////////////////////////////////////
#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <string>
#include <array>
#include <wchar.h>
#include <stdio.h>
#include <fstream>
#include "Resources.cpp"

using namespace std;

// Pre-defining all of the Callback window procedures
LRESULT CALLBACK StartWindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SimulationWindowProcedure(HWND, UINT, WPARAM, LPARAM);

// HWND instances for primary windows
HWND startWindow;
HWND simulationWindow;

HINSTANCE hInst;

// HWND instances for item values
HWND buttonToOptionsMenu;
HWND cachesize_window;
HWND blocksize_window;
HWND associativity_window;
HWND numofsets_window;
HWND numofblocks_window;
HWND taglength_window;
HWND indexlength_window;
HWND offsetlength_window;
HWND accesses_window;
HWND hits_window;
HWND misses_window;
HWND hit_rate_window;

static HWND configurationFileTextbox;   // Values for configuration filename textbox field
static HWND traceFileTextbox;   // Values for trace filename textbox field

int g_scrollY = 0;

// File IO objects
ofstream login_log;

#define ID_START_BUTTON 0
#define ID_BACK_BUTTON 1
#define ID_RUN_BUTTON 2
#define ID_CONFIGFILE_TEXTBOX 4000
#define ID_TRACEFILE_TEXTBOX 4001

// Creates the Start Window class object
void CreateStartWindow( ){
    WNDCLASSEX wc; /* A properties struct of our window */
    /* zero out the struct and set the stuff we want to modify */
    memset(&wc,0,sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = StartWindowProcedure; /* This is where we will send messages to */
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    /* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE); // COLOR_3DFACE is the default gray color, lighter shade
    wc.lpszClassName = "Start Window";
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Start Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return;
    }

    startWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        "Main window",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, /* x */
        CW_USEDEFAULT, /* y */
        544, /* width */
        375, /* height */
        NULL, NULL, hInst, NULL);

    if(startWindow == NULL) {
        MessageBox(NULL, "Start Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return;
    }
}

// Creates the Simulation Window class object
void CreateSimulationWindow( ){
    WNDCLASSEX wc; /* A properties struct of our window */
    /* zero out the struct and set the stuff we want to modify */
    memset(&wc,0,sizeof(wc));
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.lpfnWndProc   = SimulationWindowProcedure; /* This is where we will send messages to */
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);

    /* White, COLOR_WINDOW is just a #define for a system color, try Ctrl+Clicking it */
    //wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE); // COLOR_3DFACE is the default gray color, lighter shade
    wc.lpszClassName = "SimulationWindowClass";
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION); /* Load a standard icon */
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION); /* use the name "A" to use the project icon */

    if(!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Simulation Window Registration Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return;
    }

    simulationWindow = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        wc.lpszClassName,
        "Simulation window",
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, /* x */
        CW_USEDEFAULT, /* y */
        580, /* width */
        580, /* height */
        NULL, NULL, hInst, NULL);

    if(simulationWindow == NULL) {
        MessageBox(NULL, "Simulation Window Creation Failed!","Error!",MB_ICONEXCLAMATION|MB_OK);
        return;
    }
}   // end of CreateSimulationWindow


// Main function for managing the WIN32 API
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    MSG msg; /* A temporary location for all messages */
    hInst = hInstance;
    CreateStartWindow( );
    CreateSimulationWindow( );
    ShowWindow(startWindow, nCmdShow);
    ShowWindow(simulationWindow, SW_HIDE);

    /*
        This is the heart of our program where all input is processed and
        sent to WndProc. Note that GetMessage blocks code flow until it receives something, so
        this loop will not produce unreasonably high CPU usage
    */
    while(GetMessage(&msg, NULL, 0, 0) > 0) { /* If no error is received... */
        TranslateMessage(&msg); /* Translate key codes to chars if present */
        DispatchMessage(&msg); /* Send it to WndProc */
    }
    return msg.wParam;
}




// Callback procedure for start window
LRESULT CALLBACK StartWindowProcedure(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
switch(Message) {
    case WM_CREATE:{

            CreateWindow(TEXT("STATIC"), TEXT("EE 5123 - Computer Architecture (Spring 2017)"),
                         WS_VISIBLE | WS_CHILD,
                         125,  10, 350, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("Project 2: A Simple Cache Simulator"),
                         WS_VISIBLE | WS_CHILD,
                         150,  30, 350, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("By: Patrick Stockton"),
                         WS_VISIBLE | WS_CHILD,
                         200,  50, 350, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("______________________________________"),
                         WS_VISIBLE | WS_CHILD,
                         125, 70, 300, 20,
                         hwnd, (HMENU) NULL, hInst, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("This program provides a basic N-Way Set Associative Cache Simulation. The user can enter a configuration filename/directory and trace filename/directory in the fields provided on the next window. The parameters and results will be displayed for each simulation."),
                         WS_VISIBLE | WS_CHILD | WS_BORDER,
                         100,  125, 350, 100,
                         hwnd, (HMENU) NULL, NULL, NULL);

            buttonToOptionsMenu = CreateWindow("BUTTON", "START",
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         200,  250, 100, 50,
                         hwnd, (HMENU) ID_START_BUTTON, hInst, NULL);


        break;
    }
    case WM_COMMAND:{
        if (LOWORD(wParam) == ID_START_BUTTON) {
                ShowWindow(startWindow, SW_HIDE);
                ShowWindow(simulationWindow, SW_SHOW);
        }
        break;
    }

    case WM_CLOSE:{
        if (MessageBox(hwnd, "Exiting Program...", "Cache Sim", MB_ICONQUESTION) == IDOK)
        {
            DestroyWindow(hwnd);
        }
        break;
    }
    /* Upon destruction, tell the main thread to stop */
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }

    /* All other messages (a lot of them) are processed using default procedures */
    default:
        return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
} // end of StartWindowProcedure


// Callback procedure for Simulation menu
LRESULT CALLBACK SimulationWindowProcedure(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_CREATE: {
            CreateWindow(TEXT("STATIC"), TEXT("EE 5123 - Computer Architecture (Spring 2017)"),
                         WS_VISIBLE | WS_CHILD,
                         125,  10, 350, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("Project 2: A Simple Cache Simulator"),
                         WS_VISIBLE | WS_CHILD,
                         150,  30, 350, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("By: Patrick Stockton"),
                         WS_VISIBLE | WS_CHILD,
                         200,  50, 350, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("______________________________________"),
                         WS_VISIBLE | WS_CHILD,
                         125, 70, 300, 20,
                         hwnd, (HMENU) NULL, hInst, NULL);

            CreateWindow("BUTTON", "BACK",
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         100, 425, 100, 25,
                         hwnd, (HMENU) ID_BACK_BUTTON, NULL, NULL);


            CreateWindow(TEXT("STATIC"), TEXT("Enter Filenames (case sensitive)"),
                         WS_VISIBLE | WS_CHILD,
                         75,  125, 125, 30,
                         hwnd, (HMENU) NULL, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("Configuration Filename:"),
                         WS_VISIBLE | WS_CHILD,
                         50,  175, 200, 30,
                         hwnd, (HMENU) NULL, NULL, NULL);

            configurationFileTextbox = CreateWindow(TEXT("EDIT"), TEXT("CONFIG.EXT"),
                         WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                         75,  200, 150, 25,
                         hwnd, (HMENU) ID_CONFIGFILE_TEXTBOX, NULL, NULL);

            CreateWindow(TEXT("STATIC"), TEXT("Trace Filename:"),
                         WS_VISIBLE | WS_CHILD,
                         50,  250, 200, 30,
                         hwnd, (HMENU) NULL, NULL, NULL);

            traceFileTextbox = CreateWindow(TEXT("EDIT"), TEXT("TRACE.EXT"),
                         WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                         75, 275, 150, 25,
                         hwnd, (HMENU) ID_TRACEFILE_TEXTBOX, NULL, NULL);

            CreateWindow("BUTTON", "RUN",
                         WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                         50, 325, 200, 50,
                         hwnd, (HMENU) ID_RUN_BUTTON, NULL, NULL);

            break;
        }
        case WM_COMMAND:{

            if (LOWORD(wParam) == ID_RUN_BUTTON) {
                int len_config = GetWindowTextLength(configurationFileTextbox) + 1; // Returns the username field text length
                int len_trace = GetWindowTextLength(traceFileTextbox) + 1; // Returns the password field text length
                static char configVal[500] = "";
                static char traceVal[500] = "";

                GetWindowText(configurationFileTextbox, configVal, len_config);
                GetWindowText(traceFileTextbox, traceVal, len_trace);
                //MessageBox(hwnd, title, "Message Box", MB_OK);;

                string config_filename;  //or use wstring for wchar_t
                string trace_filename;
                config_filename = configVal;
                trace_filename = traceVal;
                cout << "config_filename value: " << config_filename << endl;
                cout << "trace_filename value: " << trace_filename << endl;

                //config_infile.open(config_filename);
                //trace_infile.open(trace_filename);

                if (config_filename == "CONFIG.EXT" || trace_filename == "TRACE.EXT") {
                    MessageBox(hwnd, "Invalid filename(s)!\nChange filenames from default values!", "Filename Error", MB_ICONERROR);
                } else {
                    readConfigFile(config_filename);
                    readTraceFile(trace_filename);
                    MessageBox(hwnd, "Simulation Complete!", "Simulation Complete", MB_OK);
                }

                /////////////////// Configuration Parameters ///////////////////
                CreateWindow(TEXT("STATIC"), TEXT("Configuration File Parameters"),
                         WS_VISIBLE | WS_CHILD,
                         300,  125, 250, 30,
                         hwnd, (HMENU) NULL, NULL, NULL);

                cache_size_str_temp = cache_size_str + to_string(cache_size);
                strcpy(cache_size_c, cache_size_str_temp.c_str());
                cachesize_window = CreateWindow(TEXT("STATIC"), cache_size_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  150, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                block_size_str_temp = block_size_str + to_string(block_size);
                strcpy(block_size_c, block_size_str_temp.c_str());
                blocksize_window = CreateWindow(TEXT("STATIC"), block_size_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  175, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                associativity_size_str_temp = associativity_size_str + to_string(associativity_value);
                strcpy(associativity_size_c, associativity_size_str_temp.c_str());
                associativity_window = CreateWindow(TEXT("STATIC"), associativity_size_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  200, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                sets_size_str_temp = sets_size_str + to_string(num_of_sets);
                strcpy(sets_size_c, sets_size_str_temp.c_str());
                numofsets_window = CreateWindow(TEXT("STATIC"), sets_size_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  225, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                block_num_str_temp = block_num_str + to_string(num_of_blocks);
                strcpy(block_num_c, block_num_str_temp.c_str());
                numofblocks_window = CreateWindow(TEXT("STATIC"), block_num_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  250, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                tag_length_str_temp = tag_length_str + to_string(tag_length);
                strcpy(tag_length_c, tag_length_str_temp.c_str());
                taglength_window = CreateWindow(TEXT("STATIC"), tag_length_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  275, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                index_length_str_temp = index_length_str + to_string(index_length);
                strcpy(index_length_c, index_length_str_temp.c_str());
                indexlength_window = CreateWindow(TEXT("STATIC"), index_length_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  300, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                offset_length_str_temp = offset_length_str + to_string(offset_length);
                strcpy(offset_length_c, offset_length_str_temp.c_str());
                offsetlength_window = CreateWindow(TEXT("STATIC"), offset_length_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  325, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                ////////////////////////////////////////////////////////////////
                /////////////////// Trace Simulation Results ///////////////////
                CreateWindow(TEXT("STATIC"), TEXT("Trace Simulation Results"),
                         WS_VISIBLE | WS_CHILD,
                         300,  375, 250, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                CreateWindow(TEXT("STATIC"), TEXT("-------------------------"),
                         WS_VISIBLE | WS_CHILD,
                         300,  350, 250, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                accesses_str_temp = accesses_str + to_string(num_of_accesses);
                strcpy(accesses_c, accesses_str_temp.c_str());
                accesses_window = CreateWindow(TEXT("STATIC"), accesses_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  400, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                hit_count_str_temp = hit_count_str + to_string(hit_count);
                strcpy(hit_count_c, hit_count_str_temp.c_str());
                hits_window = CreateWindow(TEXT("STATIC"), hit_count_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  425, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                miss_count_str_temp = miss_count_str + to_string(miss_count);
                strcpy(miss_count_c, miss_count_str_temp.c_str());
                misses_window = CreateWindow(TEXT("STATIC"), miss_count_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  450, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);

                hit_rate_str_temp = hit_rate_str + to_string(hit_rate) + "%";
                strcpy(hit_rate_c, hit_rate_str_temp.c_str());
                hit_rate_window = CreateWindow(TEXT("STATIC"), hit_rate_c,
                         WS_VISIBLE | WS_CHILD,
                         300,  475, 300, 25,
                         hwnd, (HMENU) NULL, NULL, NULL);


                // Reset values for further simulations on the same program partition
                num_of_accesses = 0;
                hit_count = 0;
                miss_count = 0;
                hit_rate = 0.0;
                ////////////////////////////////////////////////////////////////

            }

            if (LOWORD(wParam) == ID_BACK_BUTTON) {
                cout << "Returning to Start screen...\n" << endl;
                ShowWindow(simulationWindow, SW_HIDE);
                ShowWindow(startWindow, SW_SHOW);
            }


            break;
        }

        case WM_CLOSE:{
            CloseWindow(hwnd);
            ShowWindow(startWindow, SW_SHOW);
            break;
        }
        /* All other messages (a lot of them) are processed using default procedures */
        default:
            return DefWindowProc(hwnd, Message, wParam, lParam);
    }
    return 0;
} // end of SimulationWindowProcedure










