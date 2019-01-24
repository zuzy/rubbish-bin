#include <stdarg.h>  
#include <stddef.h>  
#include <setjmp.h>  
#include <stdlib.h>
#define UNIT_TESTING 1
#include <cmockery.h>  
/* Ensure add() adds two integers correctly. */  

// #if UNIT_TESTING
// extern void* _test_malloc(const size_t size, const char* file, const int line);
// extern void* _test_calloc(const size_t number_of_elements, const size_t size,
//                           const char* file, const int line);
// extern void _test_free(void* const ptr, const char* file, const int line);

// #define malloc(size) _test_malloc(size, __FILE__, __LINE__)
// #define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
// #define free(ptr) _test_free(ptr, __FILE__, __LINE__)
// #define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)
// #endif // UNIT_TESTING

extern int example_main();

void test_add(void **state) {  
    assert_int_equal(add(3, 3), 6);  
    assert_int_equal(add(3, -3), 0);  
}  
/* Ensure sub() subtracts two integers correctly.*/  
void test_sub(void **state) {  
    assert_int_equal(sub(3, 3), 0);  
    assert_int_equal(sub(3, -3), 0);  
}  

void common_test(void **state) {
    example_main();
}

void _mem_() {
    char *s = malloc(100);
    s = realloc(s, 120);
    free(s);
}

void memory_test(void **state) {
    _mem_();
    // free(s);
    // free(s);
}

int main(int argc, char *argv[])   
{  
    const UnitTest tests[] = {  
        unit_test(test_add),
        unit_test(test_sub),
        unit_test(common_test),  
        unit_test(memory_test),
    };  
    return run_tests(tests);  
}