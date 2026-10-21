#ifndef BORROW_RECORD_H
#define BORROW_RECORD_H

#include <string>

class BorrowRecord {
private:
    int recordId_;
    int bookId_;
    int memberId_;
    std::string issueDate_;
    std::string returnDate_;

public:
    BorrowRecord(int recordId, int bookId, int memberId, std::string issueDate,
                 std::string returnDate = "");

    int getRecordId() const;
    int getBookId() const;
    int getMemberId() const;
    const std::string& getIssueDate() const;
    const std::string& getReturnDate() const;
    bool isActive() const;
    bool close(const std::string& returnDate);
    bool isOverdue(const std::string& today, int loanDays = 14) const;

    static std::string today();
};

#endif
