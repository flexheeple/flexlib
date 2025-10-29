#include "gtest/gtest.h"

#define FSV_IMPLEMENTATION
#include "../fsv.h"

testing::AssertionResult fexpect_sv_eq_cstr(fsv_t sv, const char *cstr) {
    if (cstr == nullptr && sv.length != 0) {
        return testing::AssertionFailure() << "NULL string shouldn't have length of " << sv.length;
    } else if (cstr != nullptr && sv.length != strlen(cstr)) {
        return testing::AssertionFailure() << "String: `"  <<
            sv.datas << "` (len = " << sv.length    << ") is different from string `" <<
            cstr     << "` (len = " << strlen(cstr) << ")";
    } else if (strncmp(sv.datas, cstr, sv.length) != 0) {
        return testing::AssertionFailure() << "`" << sv.datas << "`" << " is different from `" << cstr << "`";
    } else {
        return testing::AssertionSuccess();
    }
}

TEST(fstring_view, fsv_from_cstr_NORMAL_CSTRING) {
    srand(time(NULL));
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    fsv_t sv = fsv_from_cstr(cstr);
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));
}

TEST(fstring_view, fsv_from_cstr_NULL) {
    const char *cstr = nullptr;
    fsv_t sv = fsv_from_cstr(cstr);
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));
}

TEST(fstring_view, fsv_from_cstr_EMPTY_STRING) {
    const char *cstr = "";
    fsv_t sv = fsv_from_cstr(cstr);
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));
}

TEST(fstring_view, fsv_from_partial_cstr_NORMAL_EQUAL_LENGTH) {
    const char *cstr = "The quick brown fox jumps over the lazy dog";
    size_t cstr_len = strlen(cstr);
    fsv_t sv = fsv_from_partial_cstr(cstr, cstr_len);
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));
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

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "the"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "quick\nbrown\tfox\vjumps\fover\rthe lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "quick"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "brown\tfox\vjumps\fover\rthe lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "brown"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "fox\vjumps\fover\rthe lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "fox"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "jumps\fover\rthe lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "jumps"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "over\rthe lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "over"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "the lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "the"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "lazy dog"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "lazy"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "dog"));

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "lazy"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "dog"));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_LEADING_SPACE) {
    fsv_t ot = {};
    fsv_t sv = fsv_from_cstr(" \r\n\t\v\fleading space");

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\r\n\t\v\fleading space"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\n\t\v\fleading space"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\t\v\fleading space"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\v\fleading space"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\fleading space"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "leading space"));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "leading"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "space"));

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "leading"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "space"));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_TRAILING_SPACE) {
    fsv_t ot = {};
    fsv_t sv = fsv_from_cstr("trailing space\r\n\t\f\v ");

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "trailing"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "space\r\n\t\f\v "));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "space"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\n\t\f\v "));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\t\f\v "));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\f\v "));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "\v "));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, " "));

    EXPECT_TRUE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_NOTHING_TO_SPLIT) {
    const char *cstr = "there_is_nothing_to_split";
    fsv_t sv = fsv_from_cstr(cstr);
    fsv_t ot = {};

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_FROM_EMPTY_STRING_AND_NULL) {
    const char *cstr = "";
    fsv_t sv = fsv_from_cstr(cstr);
    fsv_t ot = {};

    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    cstr = nullptr;
    EXPECT_FALSE(fsv_split(&sv, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_delim_STARTS_AND_ENDS_WITH_DELIM) {
    const char *cstr  = "/home/user/some_folder/";
    char delim        = '/';
    fsv_t sv          = fsv_from_cstr(cstr);
    fsv_t ot          = {};

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "home/user/some_folder/"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "home"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "user/some_folder/"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "user"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "some_folder/"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "some_folder"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "some_folder"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_delim_STARTS_WITH_DELIM) {
    const char *cstr  = "%home%user%some_folder";
    char delim        = '%';
    fsv_t sv          = fsv_from_cstr(cstr);
    fsv_t ot          = {};

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "home%user%some_folder"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "home"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "user%some_folder"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "user"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "some_folder"));

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "user"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "some_folder"));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_delim_ENDS_WITH_DELIM) {
    const char *cstr  = "home&user&some_folder&";
    char delim        = '&';
    fsv_t sv          = fsv_from_cstr(cstr);
    fsv_t ot          = {};

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "home"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "user&some_folder&"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "user"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, "some_folder&"));

    EXPECT_TRUE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "some_folder"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, "some_folder"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_delim_NOTHING_TO_SPLIT) {
    const char *cstr  = "there's nothing to split";
    fsv_t ot          = {};
    fsv_t sv          = fsv_from_cstr(cstr);
    char delim        = '/';

    // After a fail split, input should be the same
    // as before passing into the function
    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_delim_FROM_NULL_AND_EMPTY_STRING) {
    fsv_t ot   = {};
    fsv_t sv   = fsv_from_cstr(nullptr);
    char delim = '/';

    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    sv = fsv_from_cstr("");
    EXPECT_FALSE(fsv_split_by_delim(&sv, delim, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, ""));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_cstr_SPLIT_BY_NULL_OR_EMPTY_STRING) {
    const char *cstr  = "THe QUIck bROwn FoX juMPS oVeR THe lazy DOg";
    const char *delim = nullptr;
    fsv_t ot          = {};
    fsv_t sv          = fsv_from_cstr(cstr);

    EXPECT_FALSE(fsv_split_by_cstr(&sv, delim, true, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));

    EXPECT_FALSE(fsv_split_by_cstr(&sv, delim, false, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));

    delim = "";
    EXPECT_FALSE(fsv_split_by_cstr(&sv, delim, true, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));

    EXPECT_FALSE(fsv_split_by_cstr(&sv, delim, false, &ot));
    EXPECT_TRUE(fexpect_sv_eq_cstr(ot, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(sv, cstr));

    EXPECT_EQ(fsv_tmp_save_point(), 0);
}

TEST(fstring_view, fsv_split_by_pair_NORMAL) {
    const char *cstr  = "THe QUIck bROwn FoX [juMPS] oVeR THe lazy DOg";
    fsv_t right       = fsv_from_cstr(cstr);
    fsv_t left        = {};
    fsv_t middle      = {};

    EXPECT_TRUE(fsv_split_by_pair(&right, "[]", &middle, &left));
    EXPECT_TRUE(fexpect_sv_eq_cstr(left, "THe QUIck bROwn FoX "));
    EXPECT_TRUE(fexpect_sv_eq_cstr(middle, "juMPS"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(right, " oVeR THe lazy DOg"));
}

TEST(fstring_view, fsv_split_by_pair_MISSING_RIGHT_PAIR) {
    const char *cstr  = "THe QUIck bROwn FoX [juMPS oVeR THe lazy DOg";
    fsv_t right       = fsv_from_cstr(cstr);
    fsv_t left        = {};
    fsv_t middle      = {};

    EXPECT_FALSE(fsv_split_by_pair(&right, "[]", &middle, &left));
    EXPECT_TRUE(fexpect_sv_eq_cstr(left, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(middle, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(right, cstr));
}

TEST(fstring_view, fsv_split_by_pair_MISSING_LEFT_PAIR) {
    const char *cstr  = "THe QUIck bROwn FoX juMPS] oVeR THe lazy DOg";
    fsv_t right       = fsv_from_cstr(cstr);
    fsv_t left        = {};
    fsv_t middle      = {};

    EXPECT_FALSE(fsv_split_by_pair(&right, "[]", &middle, &left));
    EXPECT_TRUE(fexpect_sv_eq_cstr(left, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(middle, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(right, cstr));
}

TEST(fstring_view, fsv_split_by_pair_PAIR_AT_START_AND_END) {
    const char *cstr  = "(THe QUIck bROwn FoX juMPS oVeR THe lazy DOg)";
    fsv_t right       = fsv_from_cstr(cstr);
    fsv_t left        = {};
    fsv_t middle      = {};

    EXPECT_TRUE(fsv_split_by_pair(&right, "()", &middle, &left));
    EXPECT_TRUE(fexpect_sv_eq_cstr(left, ""));
    EXPECT_TRUE(fexpect_sv_eq_cstr(middle, "THe QUIck bROwn FoX juMPS oVeR THe lazy DOg"));
    EXPECT_TRUE(fexpect_sv_eq_cstr(right, ""));
}

TEST(fstring_builder, MACRO_fda_append) {
    fsb_t sb = {};

    fda_append(&sb, rand()%((int)'z') + 'a');
    EXPECT_TRUE(sb.capacity > 0);
    EXPECT_EQ(sb.length, 1);

    fda_free(&sb);
    EXPECT_EQ(sb.datas, nullptr);
    EXPECT_EQ(sb.capacity, 0);
    EXPECT_EQ(sb.length, 0);
}

TEST(fstring_builder, MACRO_fda_append_many) {
    fsb_t sb = {};
    const char *cstr = "The quick brown fox jumps over the lazy dog";

    fda_append_many(&sb, cstr, strlen(cstr));
    EXPECT_TRUE(sb.capacity > 0);
    EXPECT_EQ(sb.length, strlen(cstr));
    EXPECT_EQ(strncmp(sb.datas, cstr, sb.length), 0);

    fda_free(&sb);
    EXPECT_EQ(sb.datas, nullptr);
    EXPECT_EQ(sb.capacity, 0);
    EXPECT_EQ(sb.length, 0);
}

TEST(fstring_builder, MACRO_fda_reserve) {
    fsb_t sb = {};
    size_t len = rand()%(1024*1024);

    fda_reserve(&sb, len);
    EXPECT_EQ(sb.capacity, len);
    EXPECT_EQ(sb.length, 0);

    fda_free(&sb);
    EXPECT_EQ(sb.datas, nullptr);
    EXPECT_EQ(sb.capacity, 0);
    EXPECT_EQ(sb.length, 0);
}

TEST(fstring_builder, fsb_read_entire_file_TEST) {
    fsb_t sb = {};
    const char *file_path = "fsv.h";

    EXPECT_TRUE(fsb_read_entire_file(file_path, &sb));
    EXPECT_NE(sb.length, 0);
    EXPECT_NE(sb.capacity, 0);
    EXPECT_NE(sb.datas, nullptr);

    fda_free(&sb);
    EXPECT_EQ(sb.datas, nullptr);
    EXPECT_EQ(sb.capacity, 0);
    EXPECT_EQ(sb.length, 0);
}

TEST(fstring_builder, fsb_read_entire_dir_TEST) {
    ffp_t fp = {};
    const char *file_path = ".";

    EXPECT_TRUE(fsb_read_entire_dir(file_path, &fp, false));
    EXPECT_NE(fp.size, 0);
    EXPECT_NE(fp.capacity, 0);
    EXPECT_NE(fp.datas, nullptr);
    size_t non_recursive_len = fp.size;

    ffp_free(&fp);
    EXPECT_EQ(fp.datas, nullptr);
    EXPECT_EQ(fp.capacity, 0);
    EXPECT_EQ(fp.size, 0);

    EXPECT_TRUE(fsb_read_entire_dir(file_path, &fp, true));
    EXPECT_NE(fp.size, 0);
    EXPECT_NE(fp.capacity, 0);
    EXPECT_NE(fp.datas, nullptr);
    EXPECT_TRUE(fp.size > non_recursive_len);

    ffp_free(&fp);
    EXPECT_EQ(fp.datas, nullptr);
    EXPECT_EQ(fp.capacity, 0);
    EXPECT_EQ(fp.size, 0);
}

TEST(ftemp_buffer, fsv_tmp_alloc_test) {
    size_t save_point = fsv_tmp_save_point();

    size_t length = rand()%FSV_TMP_CAPACITY;
    fsv_tmp_alloc(length);
    EXPECT_EQ(fsv_tmp_save_point() - save_point, length);

    fsv_tmp_rewind(save_point);
    EXPECT_EQ(fsv_tmp_size, save_point);
}

TEST(ftemp_buffer, fsv_tmp_strdup) {
    size_t save_point = fsv_tmp_save_point();
    const char *cstr = "The quick brown fox jumps over the lazy dog";

    char *ret = fsv_tmp_strdup(cstr);
    EXPECT_EQ(fsv_tmp_save_point() - save_point - 1, strlen(cstr));
    EXPECT_STREQ(ret, cstr);

    fsv_tmp_rewind(save_point);
    EXPECT_EQ(fsv_tmp_size, save_point);
}
