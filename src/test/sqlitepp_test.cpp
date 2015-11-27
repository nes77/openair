//
// Created by nsamson on 11/26/15.
//

#include <iostream>
#include "libopenair/sqlite3pp.hpp"

std::string test_db_creation_string = R"(CREATE TABLE IF NOT EXISTS test_table(id INTEGER, name TEXT);)";
std::string db_insertion_test_string = R"(INSERT INTO test_table(name) VALUES(?);)";
std::string db_read_test_string = R"(SELECT ROWID, name FROM test_table;)";
std::string backup_name = "backup.db";

openair::SQLiteConnection sqlite_create_db_test() {
    auto database = openair::SQLiteConnection(":memory:"); // create an in-memory database

    auto db_lock = database.get_conn_lock();
    auto statement = openair::SQLiteStatement(database, test_db_creation_string);

    bool done = statement.step();

    db_lock.unlock();

    std::cout << done << std::endl;

    return database;
}

void sqlite_insertion_test(openair::SQLiteConnection conn) {

    auto db_lock = conn.get_conn_lock();
    openair::SQLiteStatement stmt(conn, db_insertion_test_string);

    for (int i = 0; i < 10; i++){
        auto int_str = std::to_string(i);
        stmt.bind_text(1, int_str.c_str(), (int) int_str.length() + 1, SQLITE_STATIC);
        stmt.step();
        stmt.reset();
    }

    db_lock.unlock();
}

void sqlite_read_test(openair::SQLiteConnection conn) {
    auto db_lock = conn.get_conn_lock();
    openair::SQLiteStatement stmt(conn, db_read_test_string);

    while (!stmt.step()) {
        int rowid = stmt.column_int(0);
        std::string row_text = stmt.column_text(1);
        std::cout << "Got ROWID " << rowid << ", name " << row_text << " from test_table\n";
    }

    int num_columns = stmt.column_count();

    std::cout << "Got " << num_columns << " columns" << std::endl;

    db_lock.unlock();
}

void sqlite_backup_test(openair::SQLiteConnection conn) {
    std::cout << "\nTesting out backup capabilities...\n";
    conn.backup_database(backup_name);
    openair::SQLiteConnection backup(backup_name);
    sqlite_read_test(backup);
    std::cout << "\nWe can do backups!\n" << std::endl;
}

int main(int argc, const char** argv) {
    auto db = sqlite_create_db_test();
    sqlite_insertion_test(db);
    sqlite_read_test(db);
    sqlite_backup_test(db);
    remove(backup_name.c_str());
}