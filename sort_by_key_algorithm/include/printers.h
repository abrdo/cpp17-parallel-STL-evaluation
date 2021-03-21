#include <iostream>
#include <fstream>
#include <vector>

void PRINT_vector(const std::vector<int>& inputkeys){
    for(int key : inputkeys){
        std::cout << key << ",\t";
    }
    std::cout<<"\n--------------------\n";
}

template<typename T>
void to_file(const std::vector<T>& v, std::ofstream &f){
    f<<"[ ";
    for(int i = 0; i<v.size(); i++){
        f<<v[i];
        if(i != v.size()-1) f<<", ";
    }
    f<<"]\n\n";
}
