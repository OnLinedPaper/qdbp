//conditional inclusion of a library (!!!!) which will leave out the 
//entirety of it if the compiler defines the TEST_1 flag.
#if defined TEST_1
#include <iostream>
#endif

//=============================================================================

//a simple class "c" (class) with one function "p" (print)
class c {
public:
  void p();
};

//conditionally define a piece of the function - can only be done with the 
//macro described above, because otherwise, the compiler will say "you want
//me to use cout but you DON'T want me to include the library? NAH"
void c::p() {
#if defined TEST_1
  std::cout << "p 1" << std::endl;
#endif
}

int main(void) {
  
  //if TEST_1 is defined, prints out a message. otherwise, does nothing
  c cl;
  cl.p();

  return 0;
}
