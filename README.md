# c-oop-sqlite3 (C Object Oriented interface for SQLite3)

## Quick start

In the example below, you see how to *create*, how to *insert into* (or *update*/*delete*), and how to *read* a table :

	#include "database.h"
	#include <stdio.h>
	#include <stdlib.h>


	int main(void) {

		DBConnection c;
		if (DBConnection_init(&c, "./test.db")) {

			// 1. Create table
			PreparedStatement p1;
			if (c.prepareStatement(&c, &p1, "CREATE TABLE IF NOT EXISTS my_table(id INTEGER, name VARCHAR(20))")) {
				p1.executeUpdate(&p1);
			}
			p1.free(&p1);

			// 2. Writing (with a transaction)
			if (c.startTransaction(&c)) {
				PreparedStatement p2;
				if (c.prepareStatement(&c, &p2, "INSERT INTO my_table VALUES(?, ?)")) {
					// First insert
					p2.setInt(&p2, 1, 100);
					p2.setText(&p2, 2, "Super Mario");
					p2.executeUpdate(&p2);

					// Second insert
					p2.setInt(&p2, 1, 200);
					p2.setText(&p2, 2, "Super Luigi");
					p2.executeUpdate(&p2);
				}
				p2.free(&p2);
				c.commit(&c);
			}

			// 3. Reading
			PreparedStatement p3;
			if (c.prepareStatement(&c, &p3, "SELECT id, name FROM my_table")) {
				while (p3.next(&p3)) {
					fprintf(stdout, "The 'id' is '%d' and the 'name' is '%s' \n", p3.getInt(&p3, 1), p3.getText(&p3, 2));
				}
			}
			p3.free(&p3);
		}

		// Closing resource
		c.free(&c);

		return EXIT_SUCCESS;
	}

Output :

	The 'id' is '100' and the 'name' is 'Super Mario'
	The 'id' is '200' and the 'name' is 'Super Luigi'


## Compilation (with GCC)

### Option 1 : From source files

Compiled with GCC :

	gcc -Wall -Wextra -ansi -pedantic -std=c99 ../src/database.c ../ext/sqlite-amalgamation-3220000/sqlite3.c ./main.c -o program

### Option 2 : With Windows DLL

Compiled with GCC :

	gcc -Wall -Wextra -ansi -pedantic -std=c99 ../src/database.c ../lib/sqlite3/sqlite3.dll ./main.c -o program

Note that you can generate the DLL with :

	gcc -shared sqlite3.c -o sqlite3.dll

With option n°2, the DLL **must be present next to the executable**.