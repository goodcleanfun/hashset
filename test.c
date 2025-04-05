#include <stdint.h>
#include "greatest/greatest.h"

#define SET_NAME int_set
#define SET_KEY_TYPE int32_t
#define SET_INT32
#include "set.h"
#undef SET_NAME
#undef SET_KEY_TYPE
#undef SET_INT32

TEST test_set(void) {
    int_set *s = int_set_new();

    ASSERT(int_set_add(s, 1));
    ASSERT(int_set_add(s, 2));
    ASSERT(int_set_add(s, 3));

    ASSERT_EQ(int_set_contains(s, 1), true);
    ASSERT_EQ(int_set_contains(s, 2), true);
    ASSERT_EQ(int_set_contains(s, 3), true);

    int_set *s2 = int_set_new();
    ASSERT(int_set_add(s2, 2));
    ASSERT(int_set_add(s2, 3));
    ASSERT(int_set_add(s2, 4));

    int_set *intersection = int_set_intersection(s, s2);
    
    ASSERT(int_set_contains(intersection, 2));
    ASSERT(int_set_contains(intersection, 3));
    ASSERT_EQ(int_set_size(intersection), 2);
    ASSERT_FALSE(int_set_contains(intersection, 1));
    ASSERT_FALSE(int_set_contains(intersection, 4));

    int_set *u = int_set_union(s, s2);
    ASSERT_EQ(int_set_size(u), 4);
    ASSERT(int_set_contains(u, 1));
    ASSERT(int_set_contains(u, 2));
    ASSERT(int_set_contains(u, 3));
    ASSERT(int_set_contains(u, 4));

    ASSERT(int_set_is_subset(s, u));
    ASSERT(int_set_is_superset(u, s));
    ASSERT(int_set_is_subset(s2, u));
    ASSERT(int_set_is_superset(u, s2));

    int_set *s3 = int_set_new();
    ASSERT(int_set_add(s3, 5));
    ASSERT(int_set_add(s3, 6));
    ASSERT(int_set_add(s3, 7));
    ASSERT(int_set_is_disjoint(s, s3));
    ASSERT(int_set_is_disjoint(s2, s3));
    ASSERT(int_set_is_disjoint(u, s3));

    int_set *diff1 = int_set_difference(s, s2);
    ASSERT(int_set_contains(diff1, 1));
    ASSERT_EQ(int_set_size(diff1), 1);
    ASSERT_FALSE(int_set_contains(diff1, 2));
    ASSERT_FALSE(int_set_contains(diff1, 3));
    ASSERT_FALSE(int_set_contains(diff1, 4));

    int_set *diff2 = int_set_difference(s2, s);
    ASSERT(int_set_contains(diff2, 4));
    ASSERT_EQ(int_set_size(diff2), 1);
    ASSERT_FALSE(int_set_contains(diff2, 1));
    ASSERT_FALSE(int_set_contains(diff2, 2));
    ASSERT_FALSE(int_set_contains(diff2, 3));


    int_set *symmetric_difference = int_set_symmetric_difference(s, s2);
    ASSERT_EQ(int_set_size(symmetric_difference), 2);
    ASSERT(int_set_contains(symmetric_difference, 1));
    ASSERT(int_set_contains(symmetric_difference, 4));


    int_set_destroy(s);
    int_set_destroy(s2);
    int_set_destroy(intersection);
    int_set_destroy(u);
    int_set_destroy(diff1);
    int_set_destroy(diff2);
    PASS();
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int32_t main(int32_t argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_set);

    GREATEST_MAIN_END();        /* display results */
}
