#include "main.h"
#include <QCoreApplication>

var<int> sum(var<int> a, var<int> b){
    return a + b;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    var<int> a;
    a = -11;
    var<int> b;
    b = 7;
    var<int*> c;
    c = (int*)&b;
    var<int> summ = sum(a, b);
    std::cout<<*c<<"+"<<a<<"="<<summ;

    std::cin.get();
    return app.exec();
}
