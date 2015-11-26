//
// Created by nsamson on 11/24/15.
//

#include "sqlite3pp.hpp"

openair::SQLiteConnection::SQLiteConnection(std::string filename) {
    sqlite3* conn = nullptr;
    int conn_result = sqlite3_open(filename.c_str(), &conn);

    if (conn_result != SQLITE_OK) {
        throw SQLiteException(conn_result);
    }

    this->conn = sqlite3_ptr(conn, sqlite3_close);
}

sqlite3 *openair::SQLiteConnection::get_raw_connection() {
    return this->conn.get();
}

openair::SQLiteStatement::SQLiteStatement(openair::SQLiteConnection &conn, std::string& sql_stmt)
    : conn(conn) {


    sqlite3_stmt* stmt = nullptr;
    int result = sqlite3_prepare_v2(this->conn.get_raw_connection(),
                                    sql_stmt.c_str(),
                                    -1,
                                    &stmt,
                                    nullptr
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }

    this->stmt = sqlite3_stmt_ptr(stmt, sqlite3_finalize);


}

void openair::SQLiteConnection::execute(std::string &sql_stmt, int (*callback)(void *, int, char **, char **),
                                        void *callback_first_arg) {

    auto lock = this->get_conn_lock();
    char* errmesg = nullptr;

    int result = sqlite3_exec(
            this->get_raw_connection(),
            sql_stmt.c_str(),
            callback,
            callback_first_arg,
            &errmesg
    );

    if (result == SQLITE_ABORT) {
        std::string err_mesg = std::string(errmesg);
        sqlite3_free(errmesg);
        throw SQLiteException(err_mesg);
    }
    lock.unlock();

}

void openair::SQLiteStatement::bind_blob(int param_index, const void *data, int data_size, void (*destructor)(void *)) {
    int result = sqlite3_bind_blob(
            this->stmt.get(),
            param_index,
            data,
            data_size,
            destructor
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_blob64(int param_index, const void *data, sqlite3_uint64 data_size,
                                           void (*destructor)(void *)) {
    int result = sqlite3_bind_blob64(
            this->stmt.get(),
            param_index,
            data,
            data_size,
            destructor
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_double(int param_index, double data) {
    int result = sqlite3_bind_double(
            this->stmt.get(),
            param_index,
            data
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_int(int param_index, int data) {
    int result = sqlite3_bind_int(
            this->stmt.get(),
            param_index,
            data
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_int64(int param_index, sqlite3_int64 data) {
    int result = sqlite3_bind_int64(
            this->stmt.get(),
            param_index,
            data
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_null(int param_index) {
    int result = sqlite3_bind_null(
            this->stmt.get(),
            param_index
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_text(int param_index, const char *data, int data_size, void (*destructor)(void *)) {
    int result = sqlite3_bind_text(
            this->stmt.get(),
            param_index,
            data,
            data_size,
            destructor
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_text16(int param_index, const void *data, int data_size,
                                           void (*destructor)(void *)) {
    int result = sqlite3_bind_text16(
            this->stmt.get(),
            param_index,
            data,
            data_size,
            destructor
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_text64(int param_index, const char *data, sqlite3_uint64 data_size,
                                           void (*destructor)(void *), unsigned char encoding) {
    int result = sqlite3_bind_text64(
            this->stmt.get(),
            param_index,
            data,
            data_size,
            destructor,
            encoding
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_value(int param_index, const sqlite3_value *value) {
    int result = sqlite3_bind_value(
            this->stmt.get(),
            param_index,
            value
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_zeroblob(int param_index, int size) {
    int result = sqlite3_bind_zeroblob(
            this->stmt.get(),
            param_index,
            size
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::bind_zeroblob64(int param_index, sqlite3_uint64 size) {
    int result = sqlite3_bind_zeroblob64(
            this->stmt.get(),
            param_index,
            size
    );

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

size_t openair::SQLiteStatement::bind_parameter_count() {
    return (size_t) sqlite3_bind_parameter_count(this->stmt.get());
}

std::string openair::SQLiteStatement::bind_parameter_name(int param_index) {
    return std::string(sqlite3_bind_parameter_name(this->stmt.get(), param_index));
}

int openair::SQLiteStatement::bind_parameter_index(std::string &param_name) {
    return sqlite3_bind_parameter_index(this->stmt.get(), param_name.c_str());
}

int openair::SQLiteStatement::column_count() {
    return sqlite3_column_count(this->stmt.get());
}

openair::SQLiteType openair::SQLiteStatement::column_type(int column_index) {
    switch (sqlite3_column_type(this->stmt.get(), column_index)) {
        case SQLITE_INTEGER:
            return openair::SQLiteType::INTEGER;

        case SQLITE_FLOAT:
            return openair::SQLiteType::FLOAT;

        case SQLITE_TEXT:
            return openair::SQLiteType::TEXT;

        case SQLITE_BLOB:
            return openair::SQLiteType::BLOB;

        case SQLITE_NULL:
            return openair::SQLiteType::SQL_NULL;

        default:
            throw SQLiteException("sqlite3_column_type returned a type that doesn't exist!");
    }
}

std::vector<uint8_t> openair::SQLiteStatement::column_blob(int column_index) {
    const void* result = sqlite3_column_blob(this->stmt.get(), column_index);

    if (result == nullptr) {
        throw SQLiteException(std::string("column_blob returned null for column index ")
                                      .append(std::to_string(column_index)));
    }

    size_t length = (size_t) sqlite3_column_bytes(this->stmt.get(), column_index);

    std::vector<uint8_t> out(length);

    std::copy((const char*)result, (const char*) result + length, out.begin());

    return out;
}

double openair::SQLiteStatement::column_double(int column_index) {
    return sqlite3_column_double(this->stmt.get(), column_index);
}

int openair::SQLiteStatement::column_int(int column_index) {
    return sqlite3_column_int(this->stmt.get(), column_index);
}

int64_t openair::SQLiteStatement::column_int64(int column_index) {
    return sqlite3_column_int64(this->stmt.get(), column_index);
}

std::string openair::SQLiteStatement::column_text(int column_index) {
    const unsigned char* result = sqlite3_column_text(this->stmt.get(), column_index);

    if (result == nullptr) {
        throw SQLiteException(std::string("column_text returned null for column index ")
                                      .append(std::to_string(column_index)));
    }

    return std::string((const char*)result);
}

std::u16string openair::SQLiteStatement::column_text16(int column_index) {
    const void* result = sqlite3_column_text(this->stmt.get(), column_index);

    if (result == nullptr) {
        throw SQLiteException(std::string("column_text16 returned null for column index ")
                                      .append(std::to_string(column_index)));
    }

    return std::u16string((const char16_t*)result);
}

sqlite3_value *openair::SQLiteStatement::column_value(int column_index) {
    auto result = sqlite3_column_value(this->stmt.get(), column_index);

    if (result == nullptr) {
        throw SQLiteException(std::string("column_value returned null for column index ")
                                      .append(std::to_string(column_index)));
    }

    return result;
}

bool openair::SQLiteStatement::step() {
    int result = sqlite3_step(this->stmt.get());

    if (result == SQLITE_ERROR || result == SQLITE_MISUSE) {
        throw SQLiteException(result);
    }

    return result == SQLITE_DONE;
}

void openair::SQLiteStatement::reset() {
    int result = sqlite3_reset(this->stmt.get());

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

void openair::SQLiteStatement::clear_bindings() {
    int result = sqlite3_clear_bindings(this->stmt.get());

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }
}

std::unique_lock<std::mutex> openair::SQLiteConnection::get_conn_lock() {
    return std::unique_lock<std::mutex>(this->connection_mutex);
}

void openair::SQLiteConnection::backup_database(std::string filename, int n_pages) {
    auto lock = this->get_conn_lock();

    openair::SQLiteConnection backup(filename);

    auto backup_lock = backup.get_conn_lock();

    auto backup_ptr = sqlite3_backup_init(
        backup.get_raw_connection(),
        "main",
        this->get_raw_connection(),
        "main"
    );

    while (true) {
        int result = sqlite3_backup_step(backup_ptr, n_pages);

        if (result != SQLITE_DONE){
            break;
        }
    }

    int result = sqlite3_backup_finish(backup_ptr);

    if (result != SQLITE_OK) {
        throw SQLiteException(result);
    }

    backup_lock.unlock();
    lock.unlock();
}
