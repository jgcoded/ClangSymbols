#include <iostream>

using namespace std;

class MyClass
{
public:
    void foo() { cout << "hello" << endl; }
};

int main()
{
    MyClass m;
    m.foo();
    return 0;
}
