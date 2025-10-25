#include "gtest/gtest.h"

#define FSV_IMPLEMENTATION
#include "../fsv.h"

#define FTEST_STRING            "   \t\r\n\v\n\f\n\tTHe QUIck bROwn FoX juMPS oVeR THe lazy DOg\t\r\t\v\t\f\n\n\n    "
#define FTEST_STRING_LOWERCASE  "   \t\r\n\v\n\f\n\tthe quick brown fox jumps over the lazy dog\t\r\t\v\t\f\n\n\n    "
#define FTEST_STRING_UPPERCASE  "   \t\r\n\v\n\f\n\tTHE QUICK BROWN FOX JUMPS OVER THE LAZY DOG\t\r\t\v\t\f\n\n\n    "
#define FTEST_STRING_TRIM_RIGHT "   \t\r\n\v\n\f\n\tTHe QUIck bROwn FoX juMPS oVeR THe lazy DOg"
#define FTEST_STRING_TRIM_LEFT  "THe QUIck bROwn FoX juMPS oVeR THe lazy DOg\t\r\t\v\t\f\n\n\n    "
#define FTEST_STRING_TRIM       "THe QUIck bROwn FoX juMPS oVeR THe lazy DOg"

#define FTEST_STRING_LENGTH            (strlen(FTEST_STRING))
#define FTEST_STRING_TRIM_LENGTH       (strlen(FTEST_STRING_TRIM))
#define FTEST_STRING_TRIM_LEFT_LENGTH  (strlen(FTEST_STRING_TRIM_LEFT))
#define FTEST_STRING_TRIM_RIGHT_LENGTH (strlen(FTEST_STRING_TRIM_RIGHT))

TEST(fsv_test, fsv_from_cstr_NORMAL_CSTRING) {
    srand(time(NULL));
    fsv_t sv = fsv_from_cstr(FTEST_STRING);

    EXPECT_EQ(sv.length, FTEST_STRING_LENGTH);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING, sv.length), 0);
}

TEST(fsv_test, fsv_from_cstr_NULL) {
    fsv_t sv = fsv_from_cstr(nullptr);

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fsv_test, fsv_from_partial_cstr_NORMAL_EQUAL_LENGTH) {
    fsv_t sv = fsv_from_partial_cstr(FTEST_STRING, FTEST_STRING_LENGTH);

    EXPECT_EQ(sv.length, FTEST_STRING_LENGTH);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING, sv.length), 0);
}

TEST(fsv_test, fsv_from_partial_cstr_NORMAL_SMALLER_LENGTH) {
    size_t offset = rand()%FTEST_STRING_LENGTH + 1;
    fsv_t sv = fsv_from_partial_cstr(FTEST_STRING, offset);

    EXPECT_EQ(sv.length, offset);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING, offset), 0);
}

TEST(fsv_test, fsv_from_partial_cstr_NORMAL_BIGGER_LENGTH) {
    fsv_t sv = fsv_from_partial_cstr(FTEST_STRING, SIZE_MAX);

    EXPECT_EQ(sv.length, FTEST_STRING_LENGTH);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING, FTEST_STRING_LENGTH), 0);
}

TEST(fsv_test, fsv_from_partial_cstr_NULL) {
    fsv_t sv = fsv_from_partial_cstr(nullptr, SIZE_MAX);

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fsv_test, fsv_from_partial_cstr_ZERO_LENGTH) {
    fsv_t sv = fsv_from_partial_cstr(FTEST_STRING, 0);

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fsv_test, fsv_from_partial_cstr_FROM_MIDDLE_STRING_MAX_LENGTH) {
    size_t offset = rand()%FTEST_STRING_LENGTH;
    fsv_t sv = fsv_from_partial_cstr(FTEST_STRING + offset, SIZE_MAX);

    EXPECT_EQ(sv.length, FTEST_STRING_LENGTH - offset);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING + offset, FTEST_STRING_LENGTH - offset), 0);
}

TEST(fsv_test, fsv_from_partial_cstr_FROM_MIDDLE_STRING) {
    size_t left  = rand()%FTEST_STRING_LENGTH;
    size_t right = rand()%(FTEST_STRING_LENGTH - left);
    fsv_t sv = fsv_from_partial_cstr(FTEST_STRING + left, right);

    EXPECT_EQ(sv.length, right);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING + left, right), 0);
}

TEST(fsv_test, FSV_TRIM_LEFT) {
    fsv_t sv = fsv_trim_left(fsv_from_cstr(FTEST_STRING));

    EXPECT_EQ(sv.length, FTEST_STRING_TRIM_LEFT_LENGTH);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING_TRIM_LEFT, sv.length), 0);
}

TEST(fsv_test, FSV_TRIM_RIGHT) {
    fsv_t sv = fsv_trim_right(fsv_from_cstr(FTEST_STRING));

    EXPECT_EQ(sv.length, FTEST_STRING_TRIM_RIGHT_LENGTH);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING_TRIM_RIGHT, sv.length), 0);
}

TEST(fsv_test, FSV_TRIM) {
    fsv_t sv = fsv_trim(fsv_from_cstr(FTEST_STRING));

    EXPECT_EQ(sv.length, FTEST_STRING_TRIM_LENGTH);
    EXPECT_EQ(strncmp(sv.datas, FTEST_STRING_TRIM, sv.length), 0);
}

TEST(fsv_test, fsv_index_of_FOUND_CASE) {
    fsv_t sv = fsv_from_cstr("abcdefghijklmnopqrstuvwxyz");
    char c = 'a' + rand()%('z' - 'a');
    int index = fsv_index_of(sv, c);

    EXPECT_EQ(sv.datas[index], c);
    EXPECT_EQ(index, c - 'a');
}

TEST(fsv_test, fsv_index_of_NOT_FOUND_CASE) {
    fsv_t sv = fsv_from_cstr("abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(fsv_index_of(sv, 'Z'), -1);
}

TEST(fsv_test, FSV_STRLEN) {
    char str[] = "abcdefghijklmnopqrstuvwxyz";
    EXPECT_EQ(fsv_strlen(FTEST_STRING), strlen(FTEST_STRING));
    EXPECT_EQ(fsv_strlen(str), strlen(str));
    EXPECT_EQ(fsv_strlen(nullptr), 0);
}

TEST(fsv_test, FSV_LOWER) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        if ('a' <= c && c <= 'z') {
            EXPECT_EQ(fsv_lower(c), c);
        } else if ('A' <= c && c <= 'Z') {
            EXPECT_EQ(fsv_lower(c), tolower(c));
        } else {
            EXPECT_EQ(fsv_lower(c), static_cast<char>(c));
        }
    }
}

TEST(fsv_test, FSV_UPPER) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        if ('a' <= c && c <= 'z') {
            EXPECT_EQ(fsv_upper(c), toupper(c));
        } else if ('A' <= c && c <= 'Z') {
            EXPECT_EQ(fsv_upper(c), c);
        } else {
            EXPECT_EQ(fsv_upper(c), static_cast<char>(c));
        }
    }
}

TEST(fsv_test, FSV_IS_SPACE) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        EXPECT_EQ(fsv_is_space(c), static_cast<bool>(isspace(c)));
    }
}

TEST(fsv_test, FSV_IS_DIGIT) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        EXPECT_EQ(fsv_is_digit(c), static_cast<bool>(isdigit(c)));
    }
}

TEST(fsv_test, FSV_IS_ALPHANUMERIC) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        EXPECT_EQ(fsv_is_alphanumeric(c), static_cast<bool>(isalnum(c)));
    }
}
