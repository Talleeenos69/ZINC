#include <iostream>
#include <string>
#include <type_traits>

//Standard Zinc functions from zincstd
template <typename T>
void print(const T& input){std::cout << input;}
template <typename T>
void println(const T& input = T()) { if constexpr (!std::is_void_v<T>) std::cout << input << std::endl; }
void println(){std::cout << std::endl;}
void newline(){std::cout << std::endl;}
std::string input(std::string prompt){std::string Input;std::cout << prompt;std::cin >> Input;return Input;}
std::string getline(std::string prompt){std::string Input;std::cout << prompt;getline(std::cin, Input);return(Input);}


std::string word = "abcdefghijklmnopqrstuvwxyz";

char* word_list = new char[word.length()];

int main(){
    for(int  a = 0;  a < word.length();  a++){
        word_list[a] = word[a];
    }

    for(int  i = 0;  i < word.size();  i++){
        for(int  j = 0;  j < i;  j++){
            print(word_list[j]);
        }
        print(" ");
        for(int  k = 0;  k < abs(i-word.size());  k++){
            print(word_list[k]);
        }
        newline();
    }

    // Don't forget to release the allocated memory when done
    delete[] word_list;
}