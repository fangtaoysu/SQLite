#include "../include/orm/database.h"
#include <vector>
#include <iostream>
#include <stdio.h>


int main() {
    orm::Database db("test.db");
    std::vector<std::string> queries {
    "",
    "CREATE TABLE user",
    "INSERT INTO users VALUES(1, 'Alice');",
    "SELECT * FROM users"
    };

    for (const auto query : queries) {
        auto err = db.execute(query);
        db.PrintError(err, query);
    }
    return 0;
}