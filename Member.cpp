#include "Member.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <utility>

Member::Member(int id, std::string name, std::string phone, std::string email,
               bool active, std::vector<int> borrowedBookIds)
    : id_(id), name_(std::move(name)), phone_(std::move(phone)), email_(std::move(email)),
      active_(active), borrowedBookIds_(std::move(borrowedBookIds)) {}

int Member::getId() const { return id_; }
const std::string& Member::getName() const { return name_; }
const std::string& Member::getPhone() const { return phone_; }
const std::string& Member::getEmail() const { return email_; }
bool Member::isActive() const { return active_; }
const std::vector<int>& Member::getBorrowedBookIds() const { return borrowedBookIds_; }
bool Member::canBorrow() const { return active_ && borrowedBookIds_.size() < BORROW_LIMIT; }

bool Member::hasText(const std::string& value) {
    return std::any_of(value.begin(), value.end(), [](unsigned char character) {
        return !std::isspace(character);
    });
}

bool Member::isSafeText(const std::string& value) {
    return value.find_first_of("\r\n\t") == std::string::npos;
}

bool Member::isValid() const {
    return id_ > 0 && hasText(name_) && hasText(phone_) && hasText(email_) &&
           isSafeText(name_) && isSafeText(phone_) && isSafeText(email_);
}

bool Member::updateDetails(const std::string& name, const std::string& phone,
                           const std::string& email) {
    Member proposed(id_, name, phone, email, active_, borrowedBookIds_);
    if (!proposed.isValid()) return false;
    name_ = name;
    phone_ = phone;
    email_ = email;
    return true;
}

bool Member::deactivate() {
    if (!active_ || !borrowedBookIds_.empty()) return false;
    active_ = false;
    return true;
}

bool Member::borrowBook(int bookId) {
    if (!canBorrow() || std::find(borrowedBookIds_.begin(), borrowedBookIds_.end(), bookId) != borrowedBookIds_.end()) return false;
    borrowedBookIds_.push_back(bookId);
    return true;
}

bool Member::returnBook(int bookId) {
    const auto it = std::find(borrowedBookIds_.begin(), borrowedBookIds_.end(), bookId);
    if (it == borrowedBookIds_.end()) return false;
    borrowedBookIds_.erase(it);
    return true;
}

void Member::display(std::ostream& output) const {
    output << "========================================\n"
           << "             MEMBER DETAILS\n"
           << "========================================\n"
           << "ID          : " << id_ << '\n'
           << "Name        : " << name_ << '\n'
           << "Phone       : " << phone_ << '\n'
           << "Email       : " << email_ << '\n'
           << "Status      : " << (active_ ? "Active" : "Inactive") << '\n'
           << "Borrowed IDs: ";
    if (borrowedBookIds_.empty()) output << "None";
    else for (std::size_t i = 0; i < borrowedBookIds_.size(); ++i) output << (i ? ", " : "") << borrowedBookIds_[i];
    output << "\n========================================\n";
}
