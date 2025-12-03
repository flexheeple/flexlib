#ifndef FLEXHEEPLE_CSV_H_
#define FLEXHEEPLE_CSV_H_

#include "fsv.h"

typedef struct fcsv_row {
    union { size_t size;  size_t length; };
    size_t capacity;
    union {
        fsv_t *datas;
        fsv_t *columns;
    };
} fcsv_row_t;

typedef struct fcsv {
    fsv_t      parse_point;
    fsb_t      content;
    fcsv_row_t header;
    fcsv_row_t rows;
} fcsv_t;

bool fcsv_open(fcsv_t *csv, const char *file_path, bool have_header);
void fcsv_close(fcsv_t *csv);
bool fcsv_get_next_column(fsv_t *row, fsv_t *column);
bool fcsv_get_next_row(fcsv_t *csv, fcsv_row_t *out);

#if 0
///////////////////////// Example /////////////////////////

#include <stdio.h>
#define FSV_IMPLEMENTATION
#define FCSV_IMPLEMENTATION
#include "fcsv.h"

int main(void) {
    fcsv_t csv = {0};
    const char *file_path = "path/to/csv/file";
    if (!fcsv_open(&csv, file_path, true)) return 1;

    // `fcsv_open` will parse the first line and the rewind back to
    // the beginning of the file. So if you want to skip the header,
    // you need to call `fcsv_get_next_row` once.
    fcsv_row_t row = {};
    while (fcsv_get_next_row(&csv, &row)) {
        for (size_t i = 0; i < row.size; ++i) {
            if (row.columns[i].length != 0 && row.columns[i].datas != NULL) {
                fprintf(stdout, fsv_fmt ", ", fsv_arg(row.columns[i]));
            } else {
                fprintf(stdout, "(null), ");
            }
        }
        fprintf(stdout, "\n");
    }

    fcsv_close(&csv);
    return 0;
}
#endif 


#ifdef FCSV_IMPLEMENTATION

bool fcsv_open(fcsv_t *csv, const char *file_path, bool have_header) {
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
        while (fcsv_get_next_column(&row, &column)) {
            if (have_header) { fda_append(&csv->header, column); }
            column_count++;
        }
        fda_reserve(&csv->rows, column_count);
    }

    csv->parse_point = saved_point;
    return true;
}

bool fcsv_get_next_column(fsv_t *row, fsv_t *column) {
    if (fsv_starts_with_cstr(*row, "\"", true)) {
        fsv_t unused = {};
        if (!fsv_split_by_pair(row, "\"\"", column, &unused)) return false;
        if (row->length > 0) {
            // Remove ','
            row->datas++;
            row->length--;
        }
    } else {
        if (!fsv_split_by_delim(row, ',', column)) return false;
    }
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
    fda_free(&csv->header);
    fda_free(&csv->rows);
}

#endif // FCSV_IMPLEMENTATION

#endif // FLEXHEEPLE_CSV_H_
