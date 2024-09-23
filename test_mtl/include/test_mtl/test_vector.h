#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H

#include <iostream>
#include <mtl/vector.h>

using std::ostream;

void test_constructor(ostream& os);
void test_push_pop_shrink(ostream& os);
void test_iterator(ostream& os);
void test_insert_remove(ostream& os);

#endif
