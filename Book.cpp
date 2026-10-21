#include "Book.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

Book::Book(int id, std::string title, std::string author, std::string category,
           bool available, bool archived)
    : id_(id), title_(std::move(title)), author_(std::move(author)),
      category_(std::move(category)), available_(available), archived_(archived) {}

int Book::getId() const { return id_; }
const std::string& Book::getTitle() const { return title_; }
const std::string& Book::getAuthor() const { return author_; }
const std::string& Book::getCategory() const { return category_; }
bool Book::isAvailable() const { return available_; }
bool Book::isArchived() const { return archived_; }
bool Book::canBeIssued() const { return available_ && !archived_; }

bool Book::hasText(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char character) {
        return !std::isspace(character);
    });
}

bool Book::isSafeText(const std::string& value) {
    return value.find_first_of("\r\n\t") == std::string::npos;
}

bool Book::isValid() const {
    return id_ > 0 && hasText(title_) && hasText(author_) && hasText(category_) &&
           isSafeText(title_) && isSafeText(author_) && isSafeText(category_);
}

bool Book::updateDetails(const std::string& title, const std::string& author,
                         const std::string& category) {
    Book proposed(id_, title, author, category, available_, archived_);
    if (!proposed.isValid()) return false;
    title_ = title;
    author_ = author;
    category_ = category;
    return true;
}

bool Book::archive() {
    if (archived_ || !available_) return false;
    archived_ = true;
    return true;
}

bool Book::markIssued() {
    if (!canBeIssued()) return false;
    available_ = false;
    return true;
}

bool Book::markReturned() {
    if (available_) return false;
    available_ = true;
    return true;
}

void Book::display(std::ostream& output) const {
    output << "========================================\n"
           << "              BOOK DETAILS\n"
           << "========================================\n"
           << "ID          : " << id_ << '\n'
           << "Title       : " << title_ << '\n'
           << "Author      : " << author_ << '\n'
           << "Category    : " << category_ << '\n'
           << "Status      : " << (archived_ ? "Archived" : (available_ ? "Available" : "Issued")) << '\n'
           << "========================================\n";
}
