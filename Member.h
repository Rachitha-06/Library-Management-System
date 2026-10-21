#ifndef MEMBER_H
#define MEMBER_H

#include <iosfwd>
#include <string>
#include <vector>

class Member {
private:
    int id_;
    std::string name_;
    std::string phone_;
    std::string email_;
    bool active_;
    std::vector<int> borrowedBookIds_;

public:
    static constexpr std::size_t BORROW_LIMIT = 3;

    Member(int id, std::string name, std::string phone, std::string email,
           bool active = true, std::vector<int> borrowedBookIds = {});

    int getId() const;
    const std::string& getName() const;
    const std::string& getPhone() const;
    const std::string& getEmail() const;
    bool isActive() const;
    const std::vector<int>& getBorrowedBookIds() const;
    bool canBorrow() const;
    bool isValid() const;

    bool updateDetails(const std::string& name, const std::string& phone,
                       const std::string& email);
    bool deactivate();
    bool borrowBook(int bookId);
    bool returnBook(int bookId);
    void display(std::ostream& output) const;

    static bool hasText(const std::string& value);
    static bool isSafeText(const std::string& value);
};

#endif
