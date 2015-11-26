//
// Created by nsamson on 11/24/15.
//

#ifndef OPENAIR_SQLITE3PP_HPP
#define OPENAIR_SQLITE3PP_HPP

#include "../libopenair/sqlite3.h"
#include "../libopenair/common.hpp"
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>
#include <thread>

namespace openair {

    typedef std::shared_ptr<sqlite3> sqlite3_ptr;
    typedef std::shared_ptr<sqlite3_stmt> sqlite3_stmt_ptr;

    class SQLiteValue;

    class SQLiteConnection {

        sqlite3_ptr conn;
        std::shared_ptr<std::mutex> connection_mutex;

    public:

        SQLiteConnection(std::string filename);

        sqlite3* get_raw_connection();

        void execute(std::string& sql_stmt,
                     int (*callback)(void*,int,char**,char**),
                     void* callback_first_arg
        );

        void backup_database(std::string filename, int n_pages=-1);

        std::unique_lock<std::mutex> get_conn_lock();

    };

    enum class SQLiteType {
        INTEGER, FLOAT, TEXT, BLOB, SQL_NULL
    };

    class SQLiteStatement {
        SQLiteConnection& conn;
        sqlite3_stmt_ptr stmt;

    public:

        SQLiteStatement(SQLiteConnection& conn, std::string& sql_stmt);

        void bind_blob(int param_index, const void* data, int data_size, void(*destructor)(void*));
        void bind_blob64(int param_index, const void* data, sqlite3_uint64 data_size, void(*destructor)(void*));
        void bind_double(int param_index, double data);
        void bind_int(int param_index, int data);
        void bind_int64(int param_index, sqlite3_int64 data);
        void bind_null(int param_index);
        void bind_text(int param_index, const char* data, int data_size, void(*destructor)(void*));
        void bind_text16(int param_index, const void* data, int data_size, void(*destructor)(void*));
        void bind_text64(int param_index, const char* data, sqlite3_uint64 data_size, void(*destructor)(void*), unsigned char encoding);
        void bind_value(int param_index, const sqlite3_value*);
        void bind_zeroblob(int param_index, int size);
        void bind_zeroblob64(int param_index, sqlite3_uint64 size);

        size_t bind_parameter_count();
        std::string bind_parameter_name(int param_index);
        int bind_parameter_index(std::string& param_name);


        int column_count();
        SQLiteType column_type(int column_index);

        std::vector<uint8_t> column_blob(int column_index);
        double column_double(int column_index);
        int column_int(int column_index);
        int64_t column_int64(int column_index);
        std::string column_text(int column_index);
        std::u16string column_text16(int column_index);
        sqlite3_value* column_value(int column_index);


        bool step();
        void reset();
        void clear_bindings();
    };

    class SQLiteException : public std::runtime_error {

    public:

        SQLiteException(int error_code)
                : runtime_error(std::string("SQLite connection failed with error code").append(std::to_string(error_code))) {

        }

        SQLiteException(const std::string &__arg) : runtime_error(__arg) {

        }

    };

}

#endif //OPENAIR_SQLITE3PP_HPP
