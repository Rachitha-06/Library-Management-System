# LibSphere 

A console-based C++ library management system built to practice object-oriented design, STL algorithms, input validation, and text-file persistence. It is compatible with C++11 and newer compilers, including older MinGW installations.

## Features

- Add, view, search, update, archive, and sort books.
- Register, view, search, update, and deactivate members.
- Issue and return books with duplicate, availability, active-member, and borrowing-limit checks.
- Store every issue/return in a borrowing record.
- Show dashboard totals, overdue active loans (more than 14 days), and the most-borrowed category.
- Automatically load at startup and save on demand or exit.

## Build and run

From this folder, use a C++17 compiler:

```powershell
g++ -std=c++11 -Wall -Wextra -pedantic main.cpp Book.cpp Member.cpp BorrowRecord.cpp Library.cpp FileManager.cpp -o LibSphere.exe
.\LibSphere.exe
```

Run the command above from the project folder; it creates `LibSphere.exe` beside the source files.

## Class design

`Book` owns book identity, details, status, and update/archive rules. `Member` owns member details, active status, and its borrowed-book IDs. `BorrowRecord` represents one transaction. `Library` composes collections of all three and enforces cross-entity rules such as issuing a book. `FileManager` is responsible only for saving and loading files.

This project deliberately does **not** add `Person` / `Librarian` inheritance yet. A librarian currently has no distinct state or runtime behavior, so inheritance would be decorative rather than useful. A future version could add `StaffUser` roles with different permissions; that would justify a polymorphic base class.

## STL choices

- `vector` stores growing catalogs, members, records, and search matches.
- `find_if` plus lambdas locate books and members by ID.
- `sort` orders books by the selected field.
- `map` counts borrowing history by category for the dashboard.

## Persistence format

The `data/` folder contains `books.txt`, `members.txt`, and `records.txt`. Each object occupies one tab-separated line; tabs and line breaks are rejected from user text, while spaces are safe. `FileManager` uses `ifstream` to load and `ofstream` to overwrite the current snapshot when saving. Missing files on the first run are normal; malformed files generate a readable load error.

## Suggested tests

1. Add book `101`, then try adding another `101`; the second request must fail.
2. Register member `1`, issue book `101`, then attempt a second issue; it must fail.
3. Register four books and issue them all to one member; the fourth issue must fail because the limit is three.
4. Attempt to archive an issued book; it must fail. Return it, then archive it; that must succeed.
5. Attempt to deactivate a member holding a book; it must fail.
6. Save, exit, restart, and confirm books, members, and active records are restored.
7. Enter letters for an ID or an invalid menu number; the program must ask again without crashing.

## Limitations and next ideas

This is intentionally a beginner/intermediate project. It uses plain text files rather than a database, tracks dates at day precision, and does not authenticate librarians. Good next improvements are deleting archived books after audit approval, due-date configuration, librarian logins, fine calculations, and unit tests.
