#include "FileManager.h"
#include "Library.h"

#include <cctype>
#include <iostream>
#include <string>

namespace {
std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

std::string readRequiredText(const std::string& prompt) {
    while (true) {
        const std::string value = readLine(prompt);
        if (Book::hasText(value) && Book::isSafeText(value)) return value;
        std::cout << "Please enter non-empty, single-line text.\n";
    }
}

int readPositiveInt(const std::string& prompt) {
    while (true) {
        const std::string text = readLine(prompt);
        try {
            std::size_t used = 0;
            const int value = std::stoi(text, &used);
            while (used < text.size() && std::isspace(static_cast<unsigned char>(text[used]))) ++used;
            if (used == text.size() && value > 0) return value;
        } catch (...) {}
        std::cout << "Enter a positive whole number.\n";
    }
}

int readChoice(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        const int choice = readPositiveInt(prompt);
        if (choice >= minimum && choice <= maximum) return choice;
        std::cout << "Choose a number from " << minimum << " to " << maximum << ".\n";
    }
}

void showDashboard(const Library& library) {
    const DashboardStats stats = library.dashboard();
    std::cout << "\n========================================\n"
              << "              LIBSPHERE\n"
              << "========================================\n\n"
              << "Total Books       : " << stats.totalBooks << '\n'
              << "Available Books   : " << stats.availableBooks << '\n'
              << "Issued Books      : " << stats.issuedBooks << "\n\n"
              << "Active Members    : " << stats.activeMembers << '\n'
              << "Overdue Books     : " << stats.overdueBooks << "\n\n"
              << "Most Popular Category:\n" << stats.mostPopularCategory << '\n'
              << "========================================\n";
}

void showBooks(const std::vector<const Book*>& books) {
    if (books.empty()) { std::cout << "No matching books found.\n"; return; }
    std::cout << books.size() << " book(s) found.\n";
    for (const Book* book : books) book->display(std::cout);
}

void viewAllBooks(const Library& library) {
    std::vector<const Book*> books;
    for (const Book& book : library.getBooks()) books.push_back(&book);
    showBooks(books);
}

void searchBook(const Library& library) {
    std::cout << "\nSearch book by:\n1. ID\n2. Title\n3. Author\n4. Category\n";
    const int choice = readChoice("Choice: ", 1, 4);
    if (choice == 1) {
        const Book* book = library.findBookById(readPositiveInt("Book ID: "));
        if (book) book->display(std::cout); else std::cout << "Book not found.\n";
        return;
    }
    const std::string query = readRequiredText("Search text: ");
    if (choice == 2) showBooks(library.searchBooksByTitle(query));
    if (choice == 3) showBooks(library.searchBooksByAuthor(query));
    if (choice == 4) showBooks(library.searchBooksByCategory(query));
}

void bookMenu(Library& library) {
    while (true) {
        std::cout << "\n--- BOOK MANAGEMENT ---\n1. Add book\n2. View all books\n3. Search book\n4. Update book\n5. Archive book\n6. Sort books\n7. Back\n";
        const int choice = readChoice("Choice: ", 1, 7);
        std::string message;
        if (choice == 1) {
            const int id = readPositiveInt("Book ID: ");
            const Book book(id, readRequiredText("Title: "), readRequiredText("Author: "), readRequiredText("Category: "));
            library.addBook(book, message);
            std::cout << message << '\n';
        } else if (choice == 2) viewAllBooks(library);
        else if (choice == 3) searchBook(library);
        else if (choice == 4) {
            const int id = readPositiveInt("Book ID to update: ");
            library.updateBook(id, readRequiredText("New title: "), readRequiredText("New author: "), readRequiredText("New category: "), message);
            std::cout << message << '\n';
        } else if (choice == 5) {
            library.archiveBook(readPositiveInt("Book ID to archive: "), message);
            std::cout << message << '\n';
        } else if (choice == 6) {
            std::cout << "Sort by:\n1. Book ID\n2. Title\n3. Author\n4. Category\n";
            const int sortChoice = readChoice("Choice: ", 1, 4);
            library.sortBooks(static_cast<BookSort>(sortChoice - 1));
            std::cout << "Catalog sorted.\n";
        } else return;
    }
}

void showMembers(const std::vector<const Member*>& members) {
    if (members.empty()) { std::cout << "No matching members found.\n"; return; }
    std::cout << members.size() << " member(s) found.\n";
    for (const Member* member : members) member->display(std::cout);
}

void viewAllMembers(const Library& library) {
    std::vector<const Member*> members;
    for (const Member& member : library.getMembers()) members.push_back(&member);
    showMembers(members);
}

void memberMenu(Library& library) {
    while (true) {
        std::cout << "\n--- MEMBER MANAGEMENT ---\n1. Register member\n2. View all members\n3. Search member\n4. Update member\n5. Deactivate member\n6. Back\n";
        const int choice = readChoice("Choice: ", 1, 6);
        std::string message;
        if (choice == 1) {
            const int id = readPositiveInt("Member ID: ");
            const Member member(id, readRequiredText("Name: "), readRequiredText("Phone: "), readRequiredText("Email: "));
            library.registerMember(member, message);
            std::cout << message << '\n';
        } else if (choice == 2) viewAllMembers(library);
        else if (choice == 3) {
            std::cout << "Search member by:\n1. ID\n2. Name\n";
            if (readChoice("Choice: ", 1, 2) == 1) {
                const Member* member = library.findMemberById(readPositiveInt("Member ID: "));
                if (member) member->display(std::cout); else std::cout << "Member not found.\n";
            } else showMembers(library.searchMembersByName(readRequiredText("Name text: ")));
        } else if (choice == 4) {
            const int id = readPositiveInt("Member ID to update: ");
            library.updateMember(id, readRequiredText("New name: "), readRequiredText("New phone: "), readRequiredText("New email: "), message);
            std::cout << message << '\n';
        } else if (choice == 5) {
            library.deactivateMember(readPositiveInt("Member ID to deactivate: "), message);
            std::cout << message << '\n';
        } else return;
    }
}

void borrowingMenu(Library& library) {
    while (true) {
        std::cout << "\n--- BORROWING ---\n1. Issue book\n2. Return book\n3. View active records\n4. Back\n";
        const int choice = readChoice("Choice: ", 1, 4);
        std::string message;
        if (choice == 1) {
            library.issueBook(readPositiveInt("Book ID: "), readPositiveInt("Member ID: "), message);
            std::cout << message << '\n';
        } else if (choice == 2) {
            library.returnBook(readPositiveInt("Book ID: "), readPositiveInt("Member ID: "), message);
            std::cout << message << '\n';
        } else if (choice == 3) {
            const auto records = library.activeRecords();
            if (records.empty()) std::cout << "No active borrowing records.\n";
            for (const BorrowRecord* record : records) {
                std::cout << "Record " << record->getRecordId() << ": Book " << record->getBookId()
                          << ", Member " << record->getMemberId() << ", issued " << record->getIssueDate() << '\n';
            }
        } else return;
    }
}
}

int main() {
    Library library("LibSphere");
    const std::string dataDirectory = "data";
    std::string message;
    std::cout << "\nWelcome to LibSphere.\n";
    if (!FileManager::load(library, dataDirectory, message)) std::cout << "Load warning: " << message << '\n';
    else std::cout << message << '\n';

    while (true) {
        std::cout << "\n========================================\n"
                  << "              LIBSPHERE MENU\n"
                  << "========================================\n"
                  << "1. Dashboard\n2. Book management\n3. Member management\n4. Borrowing\n5. Save data\n6. Exit\n";
        const int choice = readChoice("Choice: ", 1, 6);
        if (choice == 1) showDashboard(library);
        else if (choice == 2) bookMenu(library);
        else if (choice == 3) memberMenu(library);
        else if (choice == 4) borrowingMenu(library);
        else if (choice == 5) {
            if (FileManager::save(library, dataDirectory, message)) std::cout << message << '\n';
            else std::cout << "Save error: " << message << '\n';
        } else {
            if (FileManager::save(library, dataDirectory, message)) std::cout << "Saved. Goodbye!\n";
            else std::cout << "Save error: " << message << "\nGoodbye!\n";
            return 0;
        }
    }
}
