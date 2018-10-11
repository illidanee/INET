#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "json.h"

/*
{
"vip": null,
"is_new": true,
"uid": 123,
"uname" : "hello!",
"man_prop": [1, "hello", "2"],
"weap_prop": {"default": "putongzidan"},
}
*/

int main()
{
	//±àÂëjson
	json_t* root = json_new_object();

	json_t* j_null = json_new_null();
	json_insert_pair_into_object(root, "vip", j_null);

	json_t* j_bool = json_new_true();
	json_insert_pair_into_object(root, "is_new", j_bool);

	json_t* j_number = json_new_number("123");
	json_insert_pair_into_object(root, "uid", j_number);

	json_t* j_str = json_new_string("hello!");
	json_insert_pair_into_object(root, "uname", j_str);

	json_t* j_array = json_new_array();
	json_insert_pair_into_object(root, "man_prop", j_array);
	json_t* c_number = json_new_number("1");
	json_insert_child(j_array, c_number);
	json_t* c_string1 = json_new_string("hello");
	json_insert_child(j_array, c_string1);
	json_t* c_string2 = json_new_string("2");
	json_insert_child(j_array, c_string2);

	json_t* j_object = json_new_object();
	json_insert_pair_into_object(root, "weap_prop", j_object);
	json_t* c_string3 = json_new_string("putongzidan");
	json_insert_pair_into_object(j_object, "default", c_string3);

	char* pHsonText;
	json_tree_to_string(root, &pHsonText);
	printf("--%s\n", pHsonText);

	//ÊÍ·ÅÄÚ´æ
	json_free_value(&root);
	root = NULL;

	//½âÂëjson
	json_t* root2 = NULL;
    enum json_error err = json_parse_document(&root2, pHsonText);

	json_t* key = json_find_first_label(root2, "uname");
	if (key)
	{
		json_t* value = key->child;
		switch (value->type)
		{
		case JSON_STRING:
			printf("key:%s; value:%s\n", key->text, value->text);
			break;
		}
	}

	free(pHsonText);

    system("pause");
	return 0;
}