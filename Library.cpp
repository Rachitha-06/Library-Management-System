#include "Library.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <utility>

Library::Library(std::string name) : name_(std::move(name)) {}
const std::string& Library::getName() const { return name_; }
const std::vector<Book>& Library::getBooks() const { return books_; }
const std::vector<Member>& Library::getMembers() const { return members_; }
const std::vector<BorrowRecord>& Library::getRecords() const { return records_; }

void Library::replaceData(std::vector<Book> books, std::vector<Member> members,
                          std::vector<BorrowRecord> records) {
    books_ = std::move(books);
    members_ = std::move(members);
    records_ = std::move(records);
    nextRecordId_ = 1;
    for (const BorrowRecord& record : records_) nextRecordId_ = std::max(nextRecordId_, record.getRecordId() + 1);
}

bool Library::containsIgnoreCase(const std::string& text, const std::string& query) {
    std::string lowerText = text;
    std::string lowerQuery = query;
    const auto toLower = [](unsigned char value) { return static_cast<char>(std::tolower(value)); };
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), toLower);
    std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), toLower);
    return lowerText.find(lowerQuery) != std::string::npos;
}

const Book* Library::findBookById(int id) const {
    const auto it = std::find_if(books_.begin(), books_.end(), [id](const Book& book) { return book.getId() == id; });
    return it == books_.end() ? nullptr : &*it;
}

Book* Library::findBookById(int id) {
    auto it = std::find_if(books_.begin(), books_.end(), [id](const Book& book) { return book.getId() == id; });
    return it == books_.end() ? nullptr : &*it;
}

const Member* Library::findMemberById(int id) const {
    const auto it = std::find_if(members_.begin(), members_.end(), [id](const Member& member) { return member.getId() == id; });
    return it == members_.end() ? nullptr : &*it;
}

Member* Library::findMemberById(int id) {
    auto it = std::find_if(members_.begin(), members_.end(), [id](const Member& member) { return member.getId() == id; });
    return it == members_.end() ? nullptr : &*it;
}

bool Library::addBook(const Book& book, std::string& message) {
    if (!book.isValid()) { message = "Book data is invalid."; return false; }
    if (findBookById(book.getId())) { message = "A book with that ID already exists."; return false; }
    books_.push_back(book);
    message = "Book added successfully.";
    return true;
}

bool Library::updateBook(int id, const std::string& title, const std::string& author,
                         const std::string& category, std::string& message) {
    Book* book = findBookById(id);
    if (!book) { message = "Book not found."; return false; }
    if (!book->updateDetails(title, author, category)) { message = "Title, author, and category cannot be blank."; return false; }
    message = "Book updated successfully.";
    return true;
}

bool Library::archiveBook(int id, std::string& message) {
    Book* book = findBookById(id);
    if (!book) { message = "Book not found."; return false; }
    if (!book->archive()) { message = "Only an available, non-archived book can be archived."; return false; }
    message = "Book archived. It remains in the catalog history.";
    return true;
}

void Library::sortBooks(BookSort sortBy) {
    std::sort(books_.begin(), books_.end(), [sortBy](const Book& left, const Book& right) {
        switch (sortBy) {
            case BookSort::Title: return left.getTitle() == right.getTitle() ? left.getId() < right.getId() : left.getTitle() < right.getTitle();
            case BookSort::Author: return left.getAuthor() == right.getAuthor() ? left.getId() < right.getId() : left.getAuthor() < right.getAuthor();
            case BookSort::Category: return left.getCategory() == right.getCategory() ? left.getId() < right.getId() : left.getCategory() < right.getCategory();
            case BookSort::Id: return left.getId() < right.getId();
        }
        return false;
    });
}

bool Library::registerMember(const Member& member, std::string& message) {
    if (!member.isValid()) { message = "Member data is invalid."; return false; }
    if (findMemberById(member.getId())) { message = "A member with that ID already exists."; return false; }
    members_.push_back(member);
    message = "Member registered successfully.";
    return true;
}

bool Library::updateMember(int id, const std::string& name, const std::string& phone,
                           const std::string& email, std::string& message) {
    Member* member = findMemberById(id);
    if (!member) { message = "Member not found."; return false; }
    if (!member->updateDetails(name, phone, email)) { message = "Name, phone, and email cannot be blank."; return false; }
    message = "Member updated successfully.";
    return true;
}

bool Library::deactivateMember(int id, std::string& message) {
    Member* member = findMemberById(id);
    if (!member) { message = "Member not found."; return false; }
    if (!member->deactivate()) { message = "A member can be deactivated only when active and holding no books."; return false; }
    message = "Member deactivated successfully.";
    return true;
}

bool Library::issueBook(int bookId, int memberId, std::string& message) {
    Book* book = findBookById(bookId);
    Member* member = findMemberById(memberId);
    if (!book) { message = "Book not found."; return false; }
    if (!member) { message = "Member not found."; return false; }
    if (!book->canBeIssued()) { message = "This book is unavailable or archived."; return false; }
    if (!member->isActive()) { message = "This member is inactive."; return false; }
    if (!member->canBorrow()) { message = "The member has reached the borrowing limit (3 books)."; return false; }
    if (!member->borrowBook(bookId) || !book->markIssued()) { message = "Issue operation could not be completed."; return false; }
    records_.emplace_back(nextRecordId_++, bookId, memberId, BorrowRecord::today());
    message = "Book issued successfully.";
    return true;
}

bool Library::returnBook(int bookId, int memberId, std::string& message) {
    Book* book = findBookById(bookId);
    Member* member = findMemberById(memberId);
    if (!book) { message = "Book not found."; return false; }
    if (!member) { message = "Member not found."; return false; }
    auto record = std::find_if(records_.begin(), records_.end(), [bookId, memberId](const BorrowRecord& item) {
        return item.isActive() && item.getBookId() == bookId && item.getMemberId() == memberId;
    });
    if (record == records_.end()) { message = "No active issue record exists for this book and member."; return false; }
    if (!member->returnBook(bookId) || !book->markReturned() || !record->close(BorrowRecord::today())) {
        message = "Return operation could not be completed.";
        return false;
    }
    message = "Book returned successfully.";
    return true;
}

std::vector<const Book*> Library::searchBooksByTitle(const std::string& query) const {
    std::vector<const Book*> matches;
    for (const Book& book : books_) if (containsIgnoreCase(book.getTitle(), query)) matches.push_back(&book);
    return matches;
}
std::vector<const Book*> Library::searchBooksByAuthor(const std::string& query) const {
    std::vector<const Book*> matches;
    for (const Book& book : books_) if (containsIgnoreCase(book.getAuthor(), query)) matches.push_back(&book);
    return matches;
}
std::vector<const Book*> Library::searchBooksByCategory(const std::string& query) const {
    std::vector<const Book*> matches;
    for (const Book& book : books_) if (containsIgnoreCase(book.getCategory(), query)) matches.push_back(&book);
    return matches;
}
std::vector<const Member*> Library::searchMembersByName(const std::string& query) const {
    std::vector<const Member*> matches;
    for (const Member& member : members_) if (containsIgnoreCase(member.getName(), query)) matches.push_back(&member);
    return matches;
}
std::vector<const BorrowRecord*> Library::activeRecords() const {
    std::vector<const BorrowRecord*> matches;
    for (const BorrowRecord& record : records_) if (record.isActive()) matches.push_back(&record);
    return matches;
}

DashboardStats Library::dashboard() const {
    DashboardStats stats;
    stats.totalBooks = books_.size();
    for (const Book& book : books_) {
        if (book.isAvailable() && !book.isArchived()) ++stats.availableBooks;
        if (!book.isAvailable()) ++stats.issuedBooks;
    }
    for (const Member& member : members_) if (member.isActive()) ++stats.activeMembers;
    const std::string currentDate = BorrowRecord::today();
    for (const BorrowRecord& record : records_) if (record.isOverdue(currentDate)) ++stats.overdueBooks;
    std::map<std::string, int> categoryUses;
    for (const BorrowRecord& record : records_) {
        const Book* book = findBookById(record.getBookId());
        if (book) ++categoryUses[book->getCategory()];
    }
    int highest = 0;
    for (std::map<std::string, int>::const_iterator it = categoryUses.begin();
         it != categoryUses.end(); ++it) {
        if (it->second > highest) {
            highest = it->second;
            stats.mostPopularCategory = it->first;
        }
    }
    return stats;
}
