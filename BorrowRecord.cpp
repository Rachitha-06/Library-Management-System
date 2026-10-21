#include "BorrowRecord.h"

#include <chrono>
#include <cstdio>
#include <ctime>
#include <sstream>
#include <utility>

BorrowRecord::BorrowRecord(int recordId, int bookId, int memberId, std::string issueDate,
                           std::string returnDate)
    : recordId_(recordId), bookId_(bookId), memberId_(memberId),
      issueDate_(std::move(issueDate)), returnDate_(std::move(returnDate)) {}

int BorrowRecord::getRecordId() const { return recordId_; }
int BorrowRecord::getBookId() const { return bookId_; }
int BorrowRecord::getMemberId() const { return memberId_; }
const std::string& BorrowRecord::getIssueDate() const { return issueDate_; }
const std::string& BorrowRecord::getReturnDate() const { return returnDate_; }
bool BorrowRecord::isActive() const { return returnDate_.empty(); }

bool BorrowRecord::close(const std::string& returnDate) {
    if (!isActive() || returnDate.empty()) return false;
    returnDate_ = returnDate;
    return true;
}

std::string BorrowRecord::today() {
    const std::time_t now = std::time(nullptr);
    const std::tm* localTime = std::localtime(&now);
    if (localTime == nullptr) return "";
    char date[11] = {};
    if (std::strftime(date, sizeof(date), "%Y-%m-%d", localTime) == 0) return "";
    return date;
}

bool BorrowRecord::isOverdue(const std::string& todayDate, int loanDays) const {
    if (!isActive()) return false;
    int issuedYear, issuedMonth, issuedDay;
    int currentYear, currentMonth, currentDay;
    if (std::sscanf(issueDate_.c_str(), "%d-%d-%d", &issuedYear, &issuedMonth, &issuedDay) != 3 ||
        std::sscanf(todayDate.c_str(), "%d-%d-%d", &currentYear, &currentMonth, &currentDay) != 3) return false;
    std::tm issued = {};
    std::tm current = {};
    issued.tm_year = issuedYear - 1900;
    issued.tm_mon = issuedMonth - 1;
    issued.tm_mday = issuedDay;
    current.tm_year = currentYear - 1900;
    current.tm_mon = currentMonth - 1;
    current.tm_mday = currentDay;
    issued.tm_isdst = -1;
    current.tm_isdst = -1;
    const double elapsedSeconds = std::difftime(std::mktime(&current), std::mktime(&issued));
    return elapsedSeconds > loanDays * 24.0 * 60.0 * 60.0;
}
