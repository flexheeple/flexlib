#ifndef FLEXHEEPLE_CSV_H_
#define FLEXHEEPLE_CSV_H_

#include "fsv.h"

typedef struct fcsv_row {
    union { size_t size;  size_t length; };
    size_t capacity;
    fsv_t *datas;
} fcsv_row_t;

typedef struct fcsv {
    fsv_t      parse_point;
    fsb_t      content;
    fcsv_row_t rows;
} fcsv_t;

bool fcsv_open(fcsv_t *csv, const char *file_path);
void fcsv_close(fcsv_t *csv);
bool fcsv_get_next_column(fsv_t *row, fsv_t *column);
bool fcsv_get_next_row(fcsv_t *csv, fcsv_row_t *out);

#if 0
// Example:
int main(void) {
    fcsv_t csv = {0};
    const char *file_path = "../fcsv-parser/cashew.csv";
    if (!fcsv_open(&csv, file_path)) return 1;

    fcsv_row_t *row = NULL;
    while (fcsv_get_next_row(&csv, NULL)) {
        row = &csv.rows;
        for (size_t i = 0; i < row->size; ++i) {
            if (row->datas[i].length != 0 && row->datas[i].datas != NULL) {
                fprintf(stdout, ", " fsv_fmt, fsv_arg(row->datas[i]));
            } else {
                fprintf(stdout, ", (null)");
            }
        }
        fprintf(stdout, "\n");
    }

    fcsv_close(&csv);
    return 0;
}
#endif 


#ifdef FCSV_IMPLEMENTATION

bool fcsv_open(fcsv_t *csv, const char *file_path) {
    if (!fsb_read_entire_file(file_path, &csv->content)) {
        FSV_LOGE("[FCSV] Couldn't open file `%s`. %s\n", file_path, strerror(errno));
        return false;
    }
    fsv_t saved_point = fsv_from_sb(csv->content);
    csv->parse_point = fsv_from_sb(csv->content);

    { // Getting column_count
        size_t column_count = 0;
        fsv_t row           = {};
        fsv_t column        = {};
        if (!fsv_split_by_cstr(&csv->parse_point, "\r\n", false, &row)) return false;
        while (fcsv_get_next_column(&row, &column)) { column_count++; }
        fda_reserve(&csv->rows, column_count);
    }

    csv->parse_point = saved_point;
    return true;
}

bool fcsv_get_next_column(fsv_t *row, fsv_t *column) {
    fsv_t unused = {};
    if (fsv_starts_with_cstr(*row, "\"", true)) {
        if (!fsv_split_by_pair(row, "\"\"", &unused, column)) return false;
    } else {
        if (!fsv_split_by_delim(row, ',', column)) return false;
    }
    (void) unused;
    return true;
}

bool fcsv_get_next_row(fcsv_t *csv, fcsv_row_t *out) {
    fsv_t row    = {};
    fsv_t column = {};
    if (!fsv_split_by_cstr(&csv->parse_point, "\r\n", false, &row)) return false;

    csv->rows.size = 0;
    while (fcsv_get_next_column(&row, &column)) { fda_append(&csv->rows, column); }
    if (out != NULL) *out = csv->rows;
    return true;
}

void fcsv_close(fcsv_t *csv) {
    fsb_free(&csv->content);
}

#endif // FCSV_IMPLEMENTATION

#endif // FLEXHEEPLE_CSV_H_
