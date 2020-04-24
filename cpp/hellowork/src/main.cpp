#include <iostream>
#include <string>
#include "head.h"

using namespace std;



Dog::Dog(void) 
{
    cout << "a dog has been created!" << endl;
}

void Dog::set_name(const string &dog_name)
{
    Dog::name = dog_name;
}

void Dog::set_weight(int set_weight)
{
    Dog::weight = set_weight;
}

void Dog::print() const
{
    cout << "Dog is " << name << endl << "weight is " << weight << endl;
}

Dog::~Dog()
{
    cout << "goodbye " << name << endl;
}

OwnedDog::OwnedDog(string n)
{   
    cout << __FILE__ << ":" << __LINE__ << "\tconstruct!" << endl;
    set_name(n);
    bark();
}

// OwnedDog::~OwnedDog()
// {
// }

void OwnedDog::set_owner(const string & owner_name)
{
    owner = owner_name;
}

void OwnedDog::print() const
{
    cout << "Owner is " << owner << endl;
    Dog::print();
}

int main(int argc, char **argv)
{
    string s = "aaa";
    string s1 = "bbb";
    std::cout << "hello wrok" << std::endl;
    cout << s + s1 << endl;
    s.append(" ??");
    cout << s << endl;

    string & s_ref = s;
    s_ref = s1;
    cout << s << endl;

    const string & s1_ref = s1 + s1;
    // s1_ref += "hello";
    cout << s1 << endl;

    s1 = "????";
    cout << s1_ref << endl;

    Dog dg;
    dg.set_name("GG");
    dg.set_weight(8);
    dg.bark();
    dg.print();

    OwnedDog own("gg");
    own.set_owner("zizy");
    // own.set_name("gg");
    own.set_weight(8);
    own.bark();
    own.print();

    return 0;
}