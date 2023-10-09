#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <functional>

using namespace std;

bool keepTranslation = false;

string addLibraries(){
    string code;
    code.append("\n//Standard Zinc functions from zincstd");
    code.append("\nvoid print(string input){cout << input;}\n");
    code.append("void println(string input){cout << input << endl;}\n");
    code.append("string input(string prompt){string Input;cout << prompt;cin >> Input;return Input;}\n");
    code.append("string getLine(string prompt){string Input;cout << prompt;getline(cin, Input);return(Input);}\n");

    return code;
}

int main(int argc, char **argv) {

    if (argc >= 3) {
        string arg2 = argv[2];
        if (arg2 == "-k" || arg2 == "--keep-translation") {
            keepTranslation = true;
        }
    }

    char cwd[1024];

    string argument = argv[1];

    if (argument.substr(0, 2) == "./") {
        argument = argument.substr(2);
    }

    string tmpcwd = getcwd(cwd, sizeof(cwd));
    string fullcwd = tmpcwd + "/" + argument;
    cout << "Full path: " << fullcwd << endl;

    ifstream zincFile(fullcwd.c_str());
    if (!zincFile.is_open()) {
        cerr << "Error: File '" << fullcwd << "' does not exist." << endl;
        return 1;
    }

    // Create a vector to store the translated C++ code
    std::vector<std::string> translatedCode;

    translatedCode.push_back("#include <iostream>\n#include <functional>\nusing namespace std;");

    // Read and process each line of the input script
    std::string line;
    while (std::getline(zincFile, line)) {

        // Translate 'fn ' to 'int ' for function declarations
        size_t fnPos = line.find("fn ");
        if (fnPos == 0 || (fnPos != std::string::npos && line[fnPos - 1] == ' ')) {
            line.replace(fnPos, 3, "void ");
        }

        // Translate main
        size_t mainPos = line.find("main()");
        if (mainPos == 0 || (mainPos != std::string::npos && line[mainPos - 1] == ' ')) {
            line.replace(mainPos, 0, "int ");
        }

        // Translate loops
        size_t loopPos = line.find("loop(");
        static int loopVarCounter = 0; // Counter for generating unique loop variable names

        if (loopPos != std::string::npos) {
            // Check if there is an opening parenthesis
            size_t openParenthesisPos = line.find("(", loopPos);

            if (openParenthesisPos != std::string::npos) {
                // Extract the argument inside the loop function call
                size_t closeParenthesisPos = line.find(")", openParenthesisPos);
                if (closeParenthesisPos != std::string::npos) {
                    std::string loopArgument = line.substr(openParenthesisPos + 1, closeParenthesisPos - openParenthesisPos - 1);

                    // Generate a unique loop variable name (i, j, k, etc.)
                    char loopVarName = 'i' + loopVarCounter;
                    loopVarCounter++;

                    // Replace the loop function call with a for loop
                    line.replace(loopPos, closeParenthesisPos - loopPos + 1,
                        "for(int " + std::string(1, loopVarName) + " = 0; " + std::string(1, loopVarName) + " < " + loopArgument + "; " + std::string(1, loopVarName) + "++)");
                }
            }
        }

        // Translate 'using zinc;' to a comment in C++
        if (line.find("using zincstd;") != std::string::npos) {
            line.erase();
            line.append(addLibraries());
        }

        // Add the translated line to the vector
        translatedCode.push_back(line);
    }

    // Close the input script file
    zincFile.close();

    // Output the translated C++ code to a file
    std::ofstream outputFile("zinc_to.cpp");
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to create output file." << std::endl;
        return 1;
    }

    for (const std::string& translatedLine : translatedCode) {
        outputFile << translatedLine << std::endl;
    }

    // Close the output file
    outputFile.close();

    // Compile the saved C++ code
    int compileResult = system("g++ -o zinc_output zinc_to.cpp");

    if (compileResult == 0) {
        // Compilation was successful
        std::cout << "Compilation successful. Running the program..." << std::endl;
        system("./zinc_output"); // Run the compiled program

        if(keepTranslation == true){
            cout << "\nKept c++ translation.\n";
        }else{
            system("rm ./zinc_to.cpp");
        }

    } else {
        // Compilation failed
        std::cerr << "Compilation failed. Please check the code for errors." << std::endl;
    }

    return 0;
}
