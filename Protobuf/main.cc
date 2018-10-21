#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include "Person.pb.h"

using namespace MyMsg;

int main(int argc, char** argv)
{
    //创建类Person并初始化。
    Person p1;
    p1.set_name("illidan");
    p1.set_age(29);
    p1.add_email("admin@illidan.org");
    p1.add_email("smile@illidan.org");

    //打印
    printf("-- name:%s, age:%d\n", p1.name().c_str(), p1.age());
    int nSize1 = p1.email_size();
    for (int i = 0; i < nSize1; ++i)
    {
        printf("--    email:%s\n", p1.email(i).c_str());
    }
    printf("\n");

    //序列化
    std::string buf;
    p1.SerializeToString(&buf);

    //反序列化
    Person p2;
    p2.ParseFromString(buf);

    //打印
    printf("-- name:%s, age:%d\n", p2.name().c_str(), p2.age());
    int nSize2 = p2.email_size();
    for (int i = 0; i < nSize2; ++i)
    {
        printf("--    email:%s\n", p2.email(i).c_str());
    }
    printf("\n");

    //---------------------- 高级 ----------------------
    printf("------------------------------------------------------------------\n");
    //创建对象
    google::protobuf::Message* message = NULL;
    const google::protobuf::DescriptorPool* pool = google::protobuf::DescriptorPool::generated_pool();
    const google::protobuf::Descriptor* descriptor = google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName("MyMsg.Person");
    if (descriptor == NULL)
    {
        goto END;
    }

    const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (prototype == NULL)
    {
        goto END;
    }

    message = prototype->New();
    if (message == NULL)
    {
        goto END;
    }

    const google::protobuf::Descriptor* des = message->GetDescriptor();
    const google::protobuf::Reflection* ref = message->GetReflection();

    //赋值
    const google::protobuf::FieldDescriptor* fd = NULL;
    fd = des->FindFieldByName("name");
    if (fd)
    {
        ref->SetString(message, fd, "org.illidan");
    }

    fd = des->FindFieldByNumber(2);
    if (fd)
    {
        ref->SetInt32(message, fd, 18);
    }

    fd = des->FindFieldByName("email");
    if (fd)
    {
        ref->AddString(message, fd, "111@illidan.org");
        ref->AddString(message, fd, "555@illidan.org");
        ref->AddString(message, fd, "999@illidan.org");
    }

    //查询
    int nSize11 = des->field_count();
    for (int i = 0; i < nSize11; ++i)
    {
        const google::protobuf::FieldDescriptor* fd = des->field(i);
        printf("-- name:%s\n", fd->name().c_str());

        if (fd->is_repeated())
        {
            int size = ref->FieldSize(*message, fd);
            for (int j = 0; j < size; ++j)
            {
                switch (fd->cpp_type())
                {
                case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
                {
                    printf("--     %d\n", ref->GetRepeatedInt32(*message, fd, j));
                    break;
                }
                case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
                {
                    printf("--     %s\n", ref->GetRepeatedString(*message, fd, j).c_str());
                    break;
                }
                }
            }
            continue;
        }

        switch (fd->cpp_type())
        {
        case google::protobuf::FieldDescriptor::CppType::CPPTYPE_INT32:
        {
            printf("--     %d\n", ref->GetInt32(*message, fd));
            break;
        }
        case google::protobuf::FieldDescriptor::CppType::CPPTYPE_STRING:
        {
            printf("--     %s\n", ref->GetString(*message, fd).c_str());
            break;
        }
        }
    }

END:
    //删除对象
    if (message)
        delete message;

    return 0;
}