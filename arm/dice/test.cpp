#include <iostream>
#include <fstream>

using namespace std;

int main() {
  ofstream out;
  out.open("coordinates.txt");
  out << "things" << endl << "stuff" << endl;
  out.close();
}
