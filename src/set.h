#ifndef HASHSET_H
#define HASHSET_H

#include <stdlib.h>
#include <stdbool.h>
#include "khash/khash.h"

#endif

#ifndef SET_NAME
#error "SET_NAME must be defined"
#endif

#ifndef SET_KEY_TYPE
#error "SET_KEY_TYPE must be defined"
#endif

#if defined(SET_STR)
#ifndef SET_HASH_EQUAL
#define SET_HASH_EQUAL kh_str_hash_equal
#endif
#ifndef SET_HASH_FUNC
#define SET_HASH_FUNC kh_str_hash_func
#endif
#endif

#if defined(SET_INT32)
#ifndef SET_HASH_EQUAL
#define SET_HASH_EQUAL kh_int_hash_equal
#endif
#ifndef SET_HASH_FUNC
#define SET_HASH_FUNC kh_int_hash_func
#endif
#endif

#if defined(SET_INT64)
#ifndef SET_HASH_EQUAL
#define SET_HASH_EQUAL kh_int64_hash_equal
#endif
#ifndef SET_HASH_FUNC
#define SET_HASH_FUNC kh_int64_hash_func
#endif
#endif

#ifndef SET_HASH_FUNC
#error "SET_HASH_FUNC must be defined, or SET_STR or SET_INT must be defined"
#endif

#ifdef SET_MALLOC
#define KH_ORIG_MALLOC kmalloc
#define kmalloc(Z) SET_MALLOC(Z)
#endif

#ifdef SET_CALLOC
#define KH_ORIG_CALLOC kcalloc
#define kcalloc(N,Z) SET_CALLOC(N,Z)
#endif

#ifdef SET_REALLOC
#define KH_ORIG_REALLOC krealloc
#define krealloc(P,Z) SET_REALLOC(P,Z)
#endif

#ifdef SET_FREE
#define KH_ORIG_FREE kfree
#define kfree(P) SET_FREE(P)
#endif

#define SET_INIT(name, key_type, hash_func, equal_func) KHASH_INIT(name, key_type, char, 0, hash_func, equal_func)

SET_INIT(SET_NAME, SET_KEY_TYPE, SET_HASH_FUNC, SET_HASH_EQUAL)

#define CONCAT_(a, b) a ## b
#define CONCAT(a, b) CONCAT_(a, b)
#define CONCAT3_(a, b, c) a ## b ## c
#define CONCAT3(a, b, c) CONCAT3_(a, b, c)

#define KH_TYPE(name) CONCAT3(kh_, name, _t)
typedef KH_TYPE(SET_NAME) SET_NAME;
#define KH_FUNC(func, name) CONCAT3(func, _, name)

#define SET_TYPE KH_TYPE(SET_NAME)
#define SET_NAME_TYPE CONCAT(SET_NAME, _t)
#define SET_FUNC(name) CONCAT(SET_NAME, _##name)

static inline SET_TYPE *SET_FUNC(new)() {
    return KH_FUNC(kh_init, SET_NAME)();
}

static inline void SET_FUNC(destroy)(SET_TYPE *set) {
    KH_FUNC(kh_destroy, SET_NAME)(set);
}


static inline bool SET_FUNC(contains)(SET_TYPE *set, SET_KEY_TYPE key) {
    khiter_t k = KH_FUNC(kh_get, SET_NAME)(set, key);
    return k != kh_end(set);
}

static inline bool SET_FUNC(add)(SET_TYPE *set, SET_KEY_TYPE key) {
    int ret;
    khiter_t k = KH_FUNC(kh_put, SET_NAME)(set, key, &ret);
    return ret && k != kh_end(set);
}

static inline bool SET_FUNC(merge)(SET_TYPE *set, SET_TYPE *other) {
    khiter_t k;
    khiter_t end = kh_end(other);
    for (k = kh_begin(other); k != end; ++k) {
        if (kh_exist(other, k)) {
            SET_FUNC(add)(set, kh_key(other, k));
        }
    }
    return true;
}

static inline size_t SET_FUNC(size)(SET_TYPE *set) {
    return kh_size(set);
}

static SET_TYPE *SET_FUNC(copy)(SET_TYPE *set) {
    SET_TYPE *copy = SET_FUNC(new)();
    khiter_t k;
    khiter_t end = kh_end(set);
    for (k = kh_begin(set); k != end; ++k) {
        if (!kh_exist(set, k)) continue;
        if (!SET_FUNC(add)(copy, kh_key(set, k))) {
            SET_FUNC(destroy)(copy);
            return NULL;
        }
    }
    return copy;
}

static SET_TYPE *SET_FUNC(intersection)(SET_TYPE *set, SET_TYPE *other) {
    size_t size = SET_FUNC(size)(set);
    size_t other_size = SET_FUNC(size)(other);
    if (size < other_size) {
        SET_TYPE *temp = set;
        set = other;
        other = temp;
    }
    SET_TYPE *intersection = SET_FUNC(new)();
    if (!intersection) return NULL;
    khiter_t k;
    khiter_t end = kh_end(set);
    for (k = kh_begin(set); k != end; ++k) {
        if (kh_exist(set, k)) {
            SET_KEY_TYPE key = kh_key(set, k);
            if (SET_FUNC(contains)(other, key)) {
                SET_FUNC(add)(intersection, key);
            }
        }
    }
    return intersection;
}

static SET_TYPE *SET_FUNC(union)(SET_TYPE *set, SET_TYPE *other) {
    size_t size = SET_FUNC(size)(set);
    size_t other_size = SET_FUNC(size)(other);
    if (size < other_size) {
        SET_TYPE *temp = set;
        set = other;
        other = temp;
    }
    SET_TYPE *u = SET_FUNC(copy)(set);
    if (!u) return NULL;
    SET_FUNC(merge)(u, other);
    return u;
}

static SET_TYPE *SET_FUNC(difference)(SET_TYPE *set, SET_TYPE *other) {
    SET_TYPE *difference = SET_FUNC(new)();
    if (!difference) return NULL;
    khiter_t k;
    khiter_t end = kh_end(set);
    for (k = kh_begin(set); k != end; ++k) {
        if (kh_exist(set, k)) {
            SET_KEY_TYPE key = kh_key(set, k);
            if (!SET_FUNC(contains)(other, key)) {
                SET_FUNC(add)(difference, key);
            }
        }
    }
    return difference;
}

static SET_TYPE *SET_FUNC(symmetric_difference)(SET_TYPE *set, SET_TYPE *other) {
    SET_TYPE *symmetric_difference = SET_FUNC(new)();
    if (!symmetric_difference) return NULL;
    khiter_t k;
    khiter_t end = kh_end(set);
    for (k = kh_begin(set); k != end; ++k) {
        if (kh_exist(set, k) && !SET_FUNC(contains)(other, kh_key(set, k))) {
            SET_FUNC(add)(symmetric_difference, kh_key(set, k));
        }
    }
    for (k = kh_begin(other); k != end; ++k) {
        if (kh_exist(other, k) && !SET_FUNC(contains)(set, kh_key(other, k))) {
            SET_FUNC(add)(symmetric_difference, kh_key(other, k));
        }
    }
    return symmetric_difference;
}

static SET_TYPE *SET_FUNC(from_array)(SET_KEY_TYPE *array, size_t size) {
    SET_TYPE *set = SET_FUNC(new)();
    for (size_t i = 0; i < size; i++) {
        SET_KEY_TYPE k = array[i];
        SET_FUNC(add)(set, k);
    }
    return set;
}

static bool SET_FUNC(is_subset)(SET_TYPE *set, SET_TYPE *other) {
    khiter_t k;
    khiter_t end = kh_end(set);
    for (k = kh_begin(set); k != end; ++k) {
        if (kh_exist(set, k) && !SET_FUNC(contains)(other, kh_key(set, k))) {
            return false;
        }
    }
    return true;
}

static bool SET_FUNC(is_superset)(SET_TYPE *set, SET_TYPE *other) {
    return SET_FUNC(is_subset)(other, set);
}

static bool SET_FUNC(is_disjoint)(SET_TYPE *set, SET_TYPE *other) {
    khiter_t k;
    khiter_t end = kh_end(set);
    for (k = kh_begin(set); k != end; ++k) {
        if (kh_exist(set, k) && SET_FUNC(contains)(other, kh_key(set, k))) {
            return false;
        }
    }
    return true;
}

static inline bool SET_FUNC(remove)(SET_TYPE *set, SET_KEY_TYPE key) {
    khiter_t k = KH_FUNC(kh_get, SET_NAME)(set, key);
    if (k == kh_end(set)) return false;
    KH_FUNC(kh_del, SET_NAME)(set, k);
    return true;
}


#undef CONCAT
#undef CONCAT_
#undef CONCAT3
#undef CONCAT3_
#undef KH_TYPE
#undef KH_FUNC
#undef SET_FUNC
#undef SET_TYPE
#undef SET_NAME_TYPE

#ifdef SET_ORIG_MALLOC
#define kmalloc(Z) SET_ORIG_MALLOC(Z)
#undef SET_ORIG_MALLOC
#endif

#ifdef SET_ORIG_CALLOC
#define kcalloc(N,Z) SET_ORIG_CALLOC(N,Z)
#undef SET_ORIG_CALLOC
#endif

#ifdef SET_ORIG_REALLOC
#define krealloc(P,Z) SET_ORIG_REALLOC(P,Z)
#undef SET_ORIG_REALLOC
#endif

#ifdef SET_ORIG_FREE
#define kfree(P) SET_ORIG_FREE(P)
#undef SET_ORIG_FREE
#endif