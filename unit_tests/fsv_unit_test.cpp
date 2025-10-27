#include "gtest/gtest.h"

#define FSV_IMPLEMENTATION
#include "../fsv.h"

TEST(fstring_view, fsv_from_cstr_NORMAL_CSTRING) {
    srand(time(NULL));
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    fsv_t sv = fsv_from_cstr(cstr);

    EXPECT_EQ(sv.length, strlen(cstr));
    EXPECT_EQ(strncmp(sv.datas, cstr, sv.length), 0);
}

TEST(fstring_view, fsv_from_cstr_NULL) {
    fsv_t sv = fsv_from_cstr(nullptr);

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fstring_view, fsv_from_cstr_EMPTY_STRING) {
    fsv_t sv = fsv_from_cstr("");

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fstring_view, fsv_from_partial_cstr_NORMAL_EQUAL_LENGTH) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    size_t cstr_len = strlen(cstr);
    fsv_t sv = fsv_from_partial_cstr(cstr, cstr_len);

    EXPECT_EQ(sv.length, cstr_len);
    EXPECT_STREQ(sv.datas, cstr);
}

TEST(fstring_view, fsv_from_partial_cstr_NORMAL_SMALLER_LENGTH) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    size_t cstr_len = strlen(cstr);
    size_t offset = rand()%cstr_len + 1;
    fsv_t sv = fsv_from_partial_cstr(cstr, offset);

    EXPECT_EQ(sv.length, offset);
    EXPECT_EQ(strncmp(sv.datas, cstr, offset), 0);
}

TEST(fstring_view, fsv_from_partial_cstr_NORMAL_BIGGER_LENGTH) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    size_t cstr_len = strlen(cstr);
    fsv_t sv = fsv_from_partial_cstr(cstr, SIZE_MAX);

    EXPECT_EQ(sv.length, cstr_len);
    EXPECT_STREQ(sv.datas, cstr);
}

TEST(fstring_view, fsv_from_partial_cstr_NULL) {
    fsv_t sv = fsv_from_partial_cstr(nullptr, SIZE_MAX);

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fstring_view, fsv_from_partial_cstr_ZERO_LENGTH) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    fsv_t sv = fsv_from_partial_cstr(cstr, 0);

    EXPECT_EQ(sv.length, 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fstring_view, fsv_from_partial_cstr_FROM_MIDDLE_STRING_OVERFLOW_LENGTH) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    size_t cstr_len  = strlen(cstr);
    size_t offset    = rand()%cstr_len;
    fsv_t sv         = fsv_from_partial_cstr(cstr + offset, SIZE_MAX);

    EXPECT_EQ(sv.length, cstr_len - offset);
    EXPECT_EQ(strncmp(sv.datas, cstr + offset, cstr_len - offset), 0);
}

TEST(fstring_view, fsv_from_partial_cstr_FROM_MIDDLE_STRING) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    size_t cstr_len  = strlen(cstr);
    size_t left      = rand()%cstr_len;
    size_t right     = rand()%(cstr_len - left);
    fsv_t sv         = fsv_from_partial_cstr(cstr + left, right);

    EXPECT_EQ(sv.length, right);
    EXPECT_EQ(strncmp(sv.datas, cstr + left, right), 0);
}

TEST(fstring_view, FSV_TRIM_LEFT) {
    const char *cstr      = "   \t\r\n\v\n\f\n\tTHe QUIck bROwn FoX juMPS oVeR THe lazy DOg\t\r\t\v\t\f\n\n\n    ";
    const char *cstr_trim = "THe QUIck bROwn FoX juMPS oVeR THe lazy DOg\t\r\t\v\t\f\n\n\n    ";
    size_t cstr_trim_len  = strlen(cstr_trim);
    fsv_t sv              = fsv_trim_left(fsv_from_cstr(cstr));

    EXPECT_EQ(sv.length, cstr_trim_len);
    EXPECT_STREQ(sv.datas, cstr_trim);
}

TEST(fstring_view, FSV_TRIM_RIGHT) {
    const char *cstr      = "   \t\r\n\v\n\f\n\tTHe QUIck bROwn FoX juMPS oVeR THe lazy DOg\t\r\t\v\t\f\n\n\n    ";
    const char *cstr_trim = "   \t\r\n\v\n\f\n\tTHe QUIck bROwn FoX juMPS oVeR THe lazy DOg";
    size_t cstr_trim_len  = strlen(cstr_trim);
    fsv_t sv              = fsv_trim_right(fsv_from_cstr(cstr));

    EXPECT_EQ(sv.length, cstr_trim_len);
    EXPECT_EQ(strncmp(sv.datas, cstr_trim, sv.length), 0);
}

TEST(fstring_view, FSV_TRIM) {
    const char *cstr      = "   \t\r\n\v\n\f\n\tTHe QUIck bROwn FoX juMPS oVeR THe lazy DOg\t\r\t\v\t\f\n\n\n    ";
    const char *cstr_trim = "THe QUIck bROwn FoX juMPS oVeR THe lazy DOg";
    size_t cstr_trim_len  = strlen(cstr_trim);
    fsv_t sv              = fsv_trim(fsv_from_cstr(cstr));

    EXPECT_EQ(sv.length, cstr_trim_len);
    EXPECT_EQ(strncmp(sv.datas, cstr_trim, sv.length), 0);
}

TEST(fstring_view, fsv_index_of_FOUND_CASE) {
    fsv_t sv = fsv_from_cstr("abcdefghijklmnopqrstuvwxyz");
    char c = 'a' + rand()%('z' - 'a');
    int index = fsv_index_of(sv, c);

    EXPECT_EQ(sv.datas[index], c);
    EXPECT_EQ(index, c - 'a');
}

TEST(fstring_view, fsv_index_of_NOT_FOUND_CASE) {
    fsv_t sv = fsv_from_cstr("abcdefghijklmnopqrstuvwxyz");
    EXPECT_EQ(fsv_index_of(sv, 'Z'), -1);
}

TEST(fstring_view, FSV_STRLEN) {
    char str_arr[]   = "abcdefghijklmnopqrstuvwxyz";
    const char *cstr = "abcdefghijklmnopqrstuvwxyz";

    EXPECT_EQ(fsv_strlen(cstr), strlen(cstr));
    // To keep behaviour same as `strlen`
    EXPECT_EQ(fsv_strlen(str_arr), strlen(str_arr));
    EXPECT_EQ(fsv_strlen(nullptr), 0);
}

TEST(fstring_view, FSV_LOWER) {
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

TEST(fstring_view, FSV_UPPER) {
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

TEST(fstring_view, FSV_IS_SPACE) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        EXPECT_EQ(fsv_is_space(c), static_cast<bool>(isspace(c)));
    }
}

TEST(fstring_view, FSV_IS_DIGIT) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        EXPECT_EQ(fsv_is_digit(c), static_cast<bool>(isdigit(c)));
    }
}

TEST(fstring_view, FSV_IS_ALPHANUMERIC) {
    for (int c = 0; c <= INT8_MAX; ++c) {
        EXPECT_EQ(fsv_is_alphanumeric(c), static_cast<bool>(isalnum(c)));
    }
}

TEST(fstring_view, fsv_eq_DIFFERENT_LENGTH) {
    fsv_t sv1 = fsv_from_cstr("The quick brown fox jumps over the lazy dog");
    fsv_t sv2 = fsv_from_cstr("The quick brown fox");
    EXPECT_FALSE(fsv_eq(sv1, sv2, false));
    EXPECT_FALSE(fsv_eq(sv1, sv2, true));
}

TEST(fstring_view, fsv_eq_NULL) {
    fsv_t sv1 = fsv_from_cstr(nullptr);
    fsv_t sv2 = fsv_from_cstr(nullptr);
    EXPECT_TRUE(fsv_eq(sv1, sv2, false));
    EXPECT_TRUE(fsv_eq(sv1, sv2, true));
}

TEST(fstring_view, fsv_eq_CASE_SENSITIVE_EQUAL) {
    fsv_t sv1 = fsv_from_cstr("The quick brown fox jumps over the lazy dog");
    fsv_t sv2 = fsv_from_cstr("The quick brown fox jumps over the lazy dog");
    EXPECT_TRUE(fsv_eq(sv1, sv2, false));
}

TEST(fstring_view, fsv_eq_CASE_SENSITIVE_NOT_EQUAL) {
    fsv_t sv1 = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    fsv_t sv2 = fsv_from_cstr("The quick brown fox jumps over the lazy dog");
    EXPECT_FALSE(fsv_eq(sv1, sv2, false));
}

TEST(fstring_view, fsv_eq_CASE_INSENSITIVE_EQUAL) {
    fsv_t sv1 = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    fsv_t sv2 = fsv_from_cstr("The quick brown fox jumps over the lazy dog");
    EXPECT_TRUE(fsv_eq(sv1, sv2, true));
}

TEST(fstring_view, fsv_eq_CASE_INSENSITIVE_NOT_EQUAL) {
    fsv_t sv1 = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    fsv_t sv2 = fsv_from_cstr("The quick brown dog jumps over the lazy fox");
    EXPECT_FALSE(fsv_eq(sv1, sv2, true));
}

TEST(fstring_view, fsv_starts_with_CASE_SENSITIVE_FALSE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "The Quick";
    EXPECT_FALSE(fsv_starts_with_cstr(sv, prefix, false));
}

TEST(fstring_view, fsv_starts_with_CASE_SENSITIVE_TRUE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "THe QUIck";
    EXPECT_TRUE(fsv_starts_with_cstr(sv, prefix, false));
}

TEST(fstring_view, fsv_starts_with_CASE_INSENSITIVE_FALSE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "tHe flash";
    EXPECT_FALSE(fsv_starts_with_cstr(sv, prefix, true));
}

TEST(fstring_view, fsv_starts_with_CASE_INSENSITIVE_TRUE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "thE quiCK";
    EXPECT_TRUE(fsv_starts_with_cstr(sv, prefix, true));
}

TEST(fstring_view, fsv_ends_with_CASE_SENSITIVE_FALSE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "lazy dog";
    EXPECT_FALSE(fsv_ends_with_cstr(sv, prefix, false));
}

TEST(fstring_view, fsv_ends_with_CASE_SENSITIVE_TRUE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "lazy DOg";
    EXPECT_TRUE(fsv_ends_with_cstr(sv, prefix, false));
}

TEST(fstring_view, fsv_ends_with_CASE_INSENSITIVE_FALSE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "lazy fox";
    EXPECT_FALSE(fsv_ends_with_cstr(sv, prefix, true));
}

TEST(fstring_view, fsv_ends_with_CASE_INSENSITIVE_TRUE) {
    fsv_t sv = fsv_from_cstr("THe QUIck bROwn FoX juMPS oVeR THe lazy DOg");
    const char *prefix = "lAzY doG";
    EXPECT_TRUE(fsv_ends_with_cstr(sv, prefix, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_FROM_NULL) {
    fsv_t sv = fsv_from_cstr(nullptr);
    const char *needle = "This should not segfault";

    EXPECT_FALSE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_FALSE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_FALSE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_FALSE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_FROM_EMPTY_STRING) {
    fsv_t sv = fsv_from_cstr("");
    const char *needle = "This should not segfault";

    EXPECT_FALSE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_FALSE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_FALSE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_FALSE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_NULL) {
    fsv_t sv = fsv_from_cstr("This should not segfault");
    const char *needle = nullptr;

    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_EMPTY_STRING) {
    fsv_t sv = fsv_from_cstr("This should not segfault");
    const char *needle = "";

    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_FROM_EMPTY_STRING_WITH_EMPTY_STRING) {
    fsv_t sv = fsv_from_cstr("");
    const char *needle = "";

    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_FROM_NULLWITH_NULL) {
    fsv_t sv = fsv_from_cstr(nullptr);
    const char *needle = nullptr;

    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_FROM_EMPTY_STRING_WITH_NULL) {
    fsv_t sv = fsv_from_cstr("");
    const char *needle = nullptr;

    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_starts_or_ends_with_FROM_NULL_WITH_EMPTY_STRING) {
    fsv_t sv = fsv_from_cstr(nullptr);
    const char *needle = "";

    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_starts_with_cstr(sv, needle, true));

    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, false));
    EXPECT_TRUE(fsv_ends_with_cstr(sv, needle, true));
}

TEST(fstring_view, fsv_split_NORMAL) {
    fsv_t ot = {};
    fsv_t sv = fsv_from_cstr("the quick\nbrown\tfox\vjumps\fover\rthe lazy dog");
    const char *ex_ot = nullptr;
    const char *ex_sv = nullptr;

    ex_ot = "the";
    ex_sv = "quick\nbrown\tfox\vjumps\fover\rthe lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "quick";
    ex_sv = "brown\tfox\vjumps\fover\rthe lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "brown";
    ex_sv = "fox\vjumps\fover\rthe lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "fox";
    ex_sv = "jumps\fover\rthe lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "jumps";
    ex_sv = "over\rthe lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "over";
    ex_sv = "the lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "the";
    ex_sv = "lazy dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "lazy";
    ex_sv = "dog";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "dog";
    ex_sv = "";
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);
}

TEST(fstring_view, fsv_split_LEADING_SPACE) {
    fsv_t ot = {};
    fsv_t sv = fsv_from_cstr(" \r\n\t\v\fleading space");
    const char *ex_ot = nullptr;
    const char *ex_sv = nullptr;

    ex_ot = "";
    ex_sv = "\r\n\t\v\fleading space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\n\t\v\fleading space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\t\v\fleading space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\v\fleading space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\fleading space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "leading space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "leading";
    ex_sv = "space";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "space";
    ex_sv = "";
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);
}

TEST(fstring_view, fsv_split_TRAILING_SPACE) {
    fsv_t ot = {};
    fsv_t sv = fsv_from_cstr("trailing space\r\n\t\f\v ");
    const char *ex_ot = nullptr;
    const char *ex_sv = nullptr;

    ex_ot = "trailing";
    ex_sv = "space\r\n\t\f\v ";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "space";
    ex_sv = "\n\t\f\v ";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\t\f\v ";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\f\v ";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "\v ";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = " ";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "";
    ex_sv = "";
    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    EXPECT_FALSE(fsv_split(&sv, &ot));
}

TEST(fstring_view, fsv_split_NOTHING_TO_SPLIT) {
    const char *cstr = "there_is_nothing_to_split";
    fsv_t sv = fsv_from_cstr(cstr);
    fsv_t ot = {};

    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, strlen(cstr));
    EXPECT_EQ(sv.length, 0);
    EXPECT_EQ(strncmp(ot.datas, cstr, ot.length), 0);
    EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fstring_view, fsv_split_FROM_EMPTY_STRING_AND_NULL) {
    const char *cstr = "";
    fsv_t sv = fsv_from_cstr(cstr);
    fsv_t ot = {};

    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, 0); EXPECT_EQ(sv.length, 0);

    cstr = nullptr;
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_EQ(ot.length, 0); EXPECT_EQ(sv.length, 0);
}

TEST(fstring_view, fsv_split_by_delim_STARTS_AND_ENDS_WITH_DELIM) {
    const char *cstr  = "/home/user/some_folder/";
    char delim        = '/';
    fsv_t sv          = fsv_from_cstr(cstr);
    fsv_t ot          = {};
    const char *ex_sv = nullptr;
    const char *ex_ot = nullptr;

    ex_ot = "";
    ex_sv = "home/user/some_folder/";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "home";
    ex_sv = "user/some_folder/";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "user";
    ex_sv = "some_folder/";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "some_folder";
    ex_sv = "";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
}

TEST(fstring_view, fsv_split_by_delim_STARTS_WITH_DELIM) {
    const char *cstr  = "%home%user%some_folder";
    char delim        = '%';
    fsv_t sv          = fsv_from_cstr(cstr);
    fsv_t ot          = {};
    const char *ex_sv = nullptr;
    const char *ex_ot = nullptr;

    ex_ot = "";
    ex_sv = "home%user%some_folder";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "home";
    ex_sv = "user%some_folder";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "user";
    ex_sv = "some_folder";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "some_folder";
    ex_sv = "";
    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);
}

TEST(fstring_view, fsv_split_by_delim_ENDS_WITH_DELIM) {
    const char *cstr  = "home&user&some_folder&";
    char delim        = '&';
    fsv_t sv          = fsv_from_cstr(cstr);
    fsv_t ot          = {};
    const char *ex_sv = nullptr;
    const char *ex_ot = nullptr;

    ex_ot = "home";
    ex_sv = "user&some_folder&";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "user";
    ex_sv = "some_folder&";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    ex_ot = "some_folder";
    ex_sv = "";
    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(ex_ot));
    EXPECT_EQ(sv.length, strlen(ex_sv));
    EXPECT_EQ(strncmp(ot.datas, ex_ot, ot.length), 0);
    EXPECT_EQ(strncmp(sv.datas, ex_sv, sv.length), 0);

    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
}

TEST(fstring_view, fsv_split_by_delim_NOTHING_TO_SPLIT) {
    const char *cstr  = "there's nothing to split";
    fsv_t ot          = {};
    fsv_t sv          = fsv_from_cstr(cstr);
    char delim        = '/';

    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, strlen(cstr)); EXPECT_STREQ(ot.datas, cstr);
    EXPECT_EQ(sv.length, 0);            EXPECT_STREQ(sv.datas, nullptr);
}

TEST(fstring_view, fsv_split_by_delim_FROM_NULL) {
    const char *cstr  = nullptr;
    fsv_t ot          = {};
    fsv_t sv          = fsv_from_cstr(cstr);
    char delim        = '/';

    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_EQ(ot.length, 0); EXPECT_STREQ(ot.datas, nullptr);
    EXPECT_EQ(sv.length, 0); EXPECT_STREQ(sv.datas, nullptr);
}
