#include <iostream>
#include <string>
#include <libcuckoo/cuckoohash_map.hh>

int main() {
    libcuckoo::cuckoohash_map<int, std::string> table;

    for (int i = 0; i < 100; i++) {
        table.insert(i, "hello");
    }


    for (int i = 0; i < 101; i++) {
        std::string out;

        if (table.find(i, out)) {
            std::cout << i << "  " << out << std::endl;
        }
        else {
            std::cout << i << "  not found" << std::endl;
        }
    }
}
