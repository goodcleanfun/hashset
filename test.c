#include <stdint.h>
#include "greatest/greatest.h"

#define SET_NAME test_set
#define SET_KEY_TYPE int32_t
#define SET_INT32
#include "hashset.h"
#undef SET_NAME
#undef SET_KEY_TYPE
#undef SET_INT32

TEST test_hashset(void) {
    test_set *s = test_set_new();

    ASSERT(test_set_add(s, 1));
    ASSERT(test_set_add(s, 2));
    ASSERT(test_set_add(s, 3));

    ASSERT_EQ(test_set_contains(s, 1), true);
    ASSERT_EQ(test_set_contains(s, 2), true);
    ASSERT_EQ(test_set_contains(s, 3), true);

    test_set *s2 = test_set_new();
    ASSERT(test_set_add(s2, 2));
    ASSERT(test_set_add(s2, 3));
    ASSERT(test_set_add(s2, 4));

    test_set *intersection = test_set_intersection(s, s2);
    
    ASSERT(test_set_contains(intersection, 2));
    ASSERT(test_set_contains(intersection, 3));
    ASSERT_EQ(test_set_size(intersection), 2);
    ASSERT_FALSE(test_set_contains(intersection, 1));
    ASSERT_FALSE(test_set_contains(intersection, 4));

    test_set *u = test_set_union(s, s2);
    ASSERT_EQ(test_set_size(u), 4);
    ASSERT(test_set_contains(u, 1));
    ASSERT(test_set_contains(u, 2));
    ASSERT(test_set_contains(u, 3));
    ASSERT(test_set_contains(u, 4));

    ASSERT(test_set_is_subset(s, u));
    ASSERT(test_set_is_superset(u, s));
    ASSERT(test_set_is_subset(s2, u));
    ASSERT(test_set_is_superset(u, s2));

    test_set *s3 = test_set_new();
    ASSERT(test_set_add(s3, 5));
    ASSERT(test_set_add(s3, 6));
    ASSERT(test_set_add(s3, 7));
    ASSERT(test_set_is_disjoint(s, s3));
    ASSERT(test_set_is_disjoint(s2, s3));
    ASSERT(test_set_is_disjoint(u, s3));

    test_set *diff1 = test_set_difference(s, s2);
    ASSERT(test_set_contains(diff1, 1));
    ASSERT_EQ(test_set_size(diff1), 1);
    ASSERT_FALSE(test_set_contains(diff1, 2));
    ASSERT_FALSE(test_set_contains(diff1, 3));
    ASSERT_FALSE(test_set_contains(diff1, 4));

    test_set *diff2 = test_set_difference(s2, s);
    ASSERT(test_set_contains(diff2, 4));
    ASSERT_EQ(test_set_size(diff2), 1);
    ASSERT_FALSE(test_set_contains(diff2, 1));
    ASSERT_FALSE(test_set_contains(diff2, 2));
    ASSERT_FALSE(test_set_contains(diff2, 3));


    test_set *symmetric_difference = test_set_symmetric_difference(s, s2);
    ASSERT_EQ(test_set_size(symmetric_difference), 2);
    ASSERT(test_set_contains(symmetric_difference, 1));
    ASSERT(test_set_contains(symmetric_difference, 4));


    test_set_destroy(s);
    test_set_destroy(s2);
    test_set_destroy(intersection);
    test_set_destroy(u);
    test_set_destroy(diff1);
    test_set_destroy(diff2);
    PASS();
}

/* Add definitions that need to be in the test runner's main file. */
GREATEST_MAIN_DEFS();

int32_t main(int32_t argc, char **argv) {
    GREATEST_MAIN_BEGIN();      /* command-line options, initialization. */

    RUN_TEST(test_hashset);

    GREATEST_MAIN_END();        /* display results */
}
