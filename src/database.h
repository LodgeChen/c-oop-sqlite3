#ifndef _OOP_SQLITE
#define _OOP_SQLITE

#include <stddef.h>
#include <stdbool.h>

typedef struct DBConnection DBConnection;
typedef struct PreparedStatement PreparedStatement;

struct DBConnection {
    void* handler;
    bool (*free)(DBConnection* c);
    bool (*prepareStatement)(DBConnection* c, PreparedStatement* preparedStatement, const char* sqlRequest);

	// Transactions
    bool (*startTransaction)(DBConnection* c);
    bool (*commit)(DBConnection* c);
    bool (*rollback)(DBConnection* c);
};



struct PreparedStatement {
    void* handler;
    void* connectionHandler;
    bool (*free)(PreparedStatement* p);

    // Reading
    bool (*next)(PreparedStatement* p);
    const unsigned char* (*getText)(PreparedStatement* p, int columnIndex);
    int (*getInt)(PreparedStatement* p, int columnIndex);
    double (*getDouble)(PreparedStatement* p, int columnIndex);
    const void* (*getBlob)(PreparedStatement* p, int columnIndex);

    // Writing
    void (*setText)(PreparedStatement* p, int placeholderIndex, const char* value);
    void (*setInt)(PreparedStatement* p, int placeholderIndex, int value);
    void (*setDouble)(PreparedStatement* p, int placeholderIndex, double value);
    void (*setBlob)(PreparedStatement* p, int placeholderIndex, const void* value, size_t length);
    bool (*executeUpdate)(PreparedStatement* p);
};

bool DBConnection_init(DBConnection* c, const char* dbPath);

#endif
