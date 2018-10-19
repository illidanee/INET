#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include "Person.pb.h"

int main(int argc, char** argv)
{
    //创建类Person并初始化。
    Person p1;
    p1.set_name("illidan");
    p1.set_email("smile@illidan.org");
    p1.set_age(28);
    printf("-- %s, %s, %d\n", p1.name().c_str(), p1.email().c_str(), p1.age());

    //序列化
    std::string buf;
    p1.SerializeToString(&buf);

    //反序列化
    Person p2;
    p2.ParseFromString(buf);
    printf("-- %s, %s, %d\n", p2.name().c_str(), p2.email().c_str(), p2.age());

    return 0;
}