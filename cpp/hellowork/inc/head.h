#ifndef __HEAD_H__
#define __HEAD_H__ 1

#include <iostream>

using namespace std;

class Dog {
    string name;
    int weight;
public:
    Dog();
    void set_name(const string &name);
    void set_weight(int weight);

    virtual void print() const;

    void bark()
    {
        cout << name << " barks" << endl;
    }

    virtual ~Dog();
};

class OwnedDog : public Dog
{
public:
    OwnedDog(string n);
    // virtual ~OwnedDog(void);
    void set_owner(const string & owner_name);
    void print() const override;
private:
    string owner;
};

#endif // 