#include "database.h"
#include <stdio.h>
#include "sqlite3.h"


bool DBConnection_free(DBConnection* c) {
    if (sqlite3_close((sqlite3*) c->handler) != SQLITE_OK) {
        fprintf(stderr, "[ERROR][Database_init][sqlite3_close] : %s \n", sqlite3_errmsg((sqlite3*) c->handler));
        return false;
    }

    return true;
}



// Transactions
bool DBConnection_startTransaction(DBConnection* c){
	char** errorMessage = NULL;
	if (sqlite3_exec((sqlite3*) c->handler, "BEGIN TRANSACTION", NULL, NULL, errorMessage) != 0) {
		if (errorMessage != NULL) {
			fprintf(stderr, "[ERROR][DBConnection_startTransaction][sqlite3_exec] : %s \n", *errorMessage);
			sqlite3_free(errorMessage);
		}

		return false;
	}

	return true;
}

bool DBConnection_commit(DBConnection* c) {
	char** errorMessage = NULL;
	if (sqlite3_exec((sqlite3*) c->handler, "COMMIT", NULL, NULL, errorMessage) != 0) {
		if (errorMessage != NULL) {
			fprintf(stderr, "[ERROR][DBConnection_commit][sqlite3_exec] : %s \n", *errorMessage);
			sqlite3_free(errorMessage);
		}

		return false;
	}

	return true;
}

bool DBConnection_rollback(DBConnection* c) {
	char** errorMessage = NULL;
	if (sqlite3_exec((sqlite3*) c->handler, "ROLLBACK", NULL, NULL, errorMessage) != 0) {
		if (errorMessage != NULL) {
			fprintf(stderr, "[ERROR][DBConnection_rollback][sqlite3_exec] : %s \n", *errorMessage);
			sqlite3_free(errorMessage);
		}

		return false;
	}

	return true;
}



bool PreparedStatement_free(PreparedStatement* p) {
    if (sqlite3_finalize((sqlite3_stmt*) p->handler) != SQLITE_OK) {
        fprintf(stderr, "[ERROR][Database_init][sqlite3_close] : %s \n", sqlite3_errmsg((sqlite3*) p->connectionHandler));
        return false;
    }

    return true;
}



/**
 * Reading (https://www.sqlite.org/c3ref/column_blob.html)
 */
bool PreparedStatement_next(PreparedStatement* p) {
    return sqlite3_step((sqlite3_stmt*) p->handler) == SQLITE_ROW;
}

const unsigned char* PreparedStatement_getText(PreparedStatement* p, int columnIndex) {
	return sqlite3_column_text((sqlite3_stmt*) p->handler, columnIndex - 1);
}

int PreparedStatement_getInt(PreparedStatement* p, int columnIndex) {
	return sqlite3_column_int((sqlite3_stmt*) p->handler, columnIndex - 1);
}

double PreparedStatement_getDouble(PreparedStatement* p, int columnIndex) {
	return sqlite3_column_double((sqlite3_stmt*) p->handler, columnIndex - 1);
}

const void* PreparedStatement_getBlob(PreparedStatement* p, int columnIndex) {
	return sqlite3_column_blob((sqlite3_stmt*) p->handler, columnIndex - 1);
}


/**
 * Writing (https://www.sqlite.org/c3ref/bind_blob.html)
 */
void PreparedStatement_setText(PreparedStatement* p, int placeholderIndex, const char* value) {
	sqlite3_bind_text((sqlite3_stmt*) p->handler, placeholderIndex, value, -1, SQLITE_STATIC);
}

void PreparedStatement_setInt(PreparedStatement* p, int placeholderIndex, int value) {
	sqlite3_bind_int((sqlite3_stmt*) p->handler, placeholderIndex, value);
}

void PreparedStatement_setDouble(PreparedStatement* p, int placeholderIndex, double value) {
	sqlite3_bind_double((sqlite3_stmt*) p->handler, placeholderIndex, value);
}

void PreparedStatement_setBlob(PreparedStatement* p, int placeholderIndex, const void* value, size_t length) {
	sqlite3_bind_blob((sqlite3_stmt*) p->handler, placeholderIndex, value, length, SQLITE_STATIC);
}

bool PreparedStatement_executeUpdate(PreparedStatement* p) {
	const bool executed = sqlite3_step((sqlite3_stmt*) p->handler) == SQLITE_DONE;

	// Reset preparedStatement for more executeUpdate() calls
	if (executed && sqlite3_reset((sqlite3_stmt*) p->handler) != SQLITE_OK) {
		fprintf(stderr, "[ERROR][PreparedStatement_executeUpdate][sqlite3_reset] : %s \n", sqlite3_errmsg((sqlite3*) p->connectionHandler));
		return false;
	}

	return executed;
}



bool DBConnection_prepareStatement(DBConnection* c, PreparedStatement* preparedStatement, const char* sqlRequest) {
    if (sqlite3_prepare_v2((sqlite3*) c->handler, sqlRequest, -1, (sqlite3_stmt**) &(preparedStatement->handler), NULL) != SQLITE_OK) {
        fprintf(stderr, "[ERROR][DBConnection_prepareStatement][sqlite3_prepare_v2] : %s \n", sqlite3_errmsg((sqlite3*) c->handler));
        return false;
    }

    // Init other attributes
    preparedStatement->connectionHandler = c->handler;

    // Init methods
    preparedStatement->free = PreparedStatement_free;

	// Reading
    preparedStatement->next = PreparedStatement_next;
	preparedStatement->getText = PreparedStatement_getText;
	preparedStatement->getInt = PreparedStatement_getInt;
	preparedStatement->getDouble = PreparedStatement_getDouble;
	preparedStatement->getBlob = PreparedStatement_getBlob;

	// Writing
	preparedStatement->setText = PreparedStatement_setText;
	preparedStatement->setInt = PreparedStatement_setInt;
	preparedStatement->setDouble = PreparedStatement_setDouble;
	preparedStatement->setBlob = PreparedStatement_setBlob;
	preparedStatement->executeUpdate = PreparedStatement_executeUpdate;

    return true;
}


/**
 * Initialize a DBConnection object with this function
 *
 * For errors, read : https://sqlite.org/c3ref/errcode.html
 */
bool DBConnection_init(DBConnection* c, const char* dbPath) {
    if (sqlite3_open(dbPath, (sqlite3**) &(c->handler)) != SQLITE_OK) {
        fprintf(stderr, "[ERROR][Database_init][sqlite3_open] : %s \n", sqlite3_errmsg((sqlite3*) c->handler));
        return false;
    }

    // Init methods
    c->free = DBConnection_free;
    c->prepareStatement = DBConnection_prepareStatement;

	// Transactions
	c->startTransaction = DBConnection_startTransaction;
	c->commit = DBConnection_commit;
	c->rollback = DBConnection_rollback;

    return true;
}
