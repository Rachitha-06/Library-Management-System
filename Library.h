#ifndef LIBRARY_H
#define LIBRARY_H

#include "Book.h"
#include "BorrowRecord.h"
#include "Member.h"

#include <string>
#include <vector>

enum class BookSort { Id, Title, Author, Category };

struct DashboardStats {
    std::size_t totalBooks = 0;
    std::size_t availableBooks = 0;
    std::size_t issuedBooks = 0;
    std::size_t activeMembers = 0;
    std::size_t overdueBooks = 0;
    std::string mostPopularCategory = "N/A";
};

class Library {
private:
    std::string name_;
    std::vector<Book> books_;
    std::vector<Member> members_;
    std::vector<BorrowRecord> records_;
    int nextRecordId_ = 1;

public:
    explicit Library(std::string name = "LibSphere");

    const std::string& getName() const;
    const std::vector<Book>& getBooks() const;
    const std::vector<Member>& getMembers() const;
    const std::vector<BorrowRecord>& getRecords() const;
    void replaceData(std::vector<Book> books, std::vector<Member> members,
                     std::vector<BorrowRecord> records);

    bool addBook(const Book& book, std::string& message);
    bool updateBook(int id, const std::string& title, const std::string& author,
                    const std::string& category, std::string& message);
    bool archiveBook(int id, std::string& message);
    void sortBooks(BookSort sortBy);

    bool registerMember(const Member& member, std::string& message);
    bool updateMember(int id, const std::string& name, const std::string& phone,
                      const std::string& email, std::string& message);
    bool deactivateMember(int id, std::string& message);

    bool issueBook(int bookId, int memberId, std::string& message);
    bool returnBook(int bookId, int memberId, std::string& message);

    const Book* findBookById(int id) const;
    Book* findBookById(int id);
    const Member* findMemberById(int id) const;
    Member* findMemberById(int id);
    std::vector<const Book*> searchBooksByTitle(const std::string& query) const;
    std::vector<const Book*> searchBooksByAuthor(const std::string& query) const;
    std::vector<const Book*> searchBooksByCategory(const std::string& query) const;
    std::vector<const Member*> searchMembersByName(const std::string& query) const;
    std::vector<const BorrowRecord*> activeRecords() const;
    DashboardStats dashboard() const;

    static bool containsIgnoreCase(const std::string& text, const std::string& query);
};

#endif
