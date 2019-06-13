#include <iostream>

using namespace std;

class Point {
public:
    double x = 0;
    double y = 0;
    Point() 
    {}
    Point(double a, double b):x(a), y(b)
    {}

    Point operator+(const Point &rhs) const;
    Point operator+=(const Point &rhs);
    void print() const;
};

Point Point::operator+(const Point &rhs) const
{
    return Point(x + rhs.x, y + rhs.y);
}

Point Point::operator+=(const Point &rhs)
{
    x += rhs.x;
    y += rhs.y;
    return *this;
}

void Point::print() const
{
    cout << "x: " << x << "\ty:" << y << endl;
}

int main(int argc, char** argv)
{
    Point up(0, 1);
    Point right(1, 0);
    Point sum = up + right;
    sum.print();
    sum += up;
    sum.print();

    cout << "hello world!" << endl; 
    return 0;
}
