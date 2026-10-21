#include "FileManager.h"

#include "Library.h"

#include <cerrno>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

namespace {
bool ensureDirectory(const std::string& directory) {
#ifdef _WIN32
    const int result = _mkdir(directory.c_str());
#else
    const int result = mkdir(directory.c_str(), 0777);
#endif
    return result == 0 || errno == EEXIST;
}

std::vector<std::string> splitTabs(const std::string& line) {
    std::vector<std::string> parts;
    std::istringstream input(line);
    std::string part;
    while (std::getline(input, part, '\t')) parts.push_back(part);
    if (!line.empty() && line[line.size() - 1] == '\t') parts.push_back("");
    return parts;
}

std::string joinIds(const std::vector<int>& ids) {
    std::ostringstream result;
    for (std::size_t i = 0; i < ids.size(); ++i) result << (i ? "," : "") << ids[i];
    return result.str();
}

bool parseIds(const std::string& value, std::vector<int>& ids) {
    ids.clear();
    if (value.empty()) return true;
    std::istringstream input(value);
    std::string item;
    try {
        while (std::getline(input, item, ',')) {
            const int id = std::stoi(item);
            if (id <= 0) return false;
            ids.push_back(id);
        }
    } catch (...) { return false; }
    return true;
}

bool parseFlag(const std::string& value, bool& result) {
    if (value == "0") { result = false; return true; }
    if (value == "1") { result = true; return true; }
    return false;
}
}

bool FileManager::save(const Library& library, const std::string& dataDirectory,
                       std::string& message) {
    if (!ensureDirectory(dataDirectory)) { message = "Could not create the data folder."; return false; }
    std::ofstream booksFile(dataDirectory + "/books.txt");
    std::ofstream membersFile(dataDirectory + "/members.txt");
    std::ofstream recordsFile(dataDirectory + "/records.txt");
    if (!booksFile || !membersFile || !recordsFile) { message = "Could not open one or more data files for writing."; return false; }

    for (std::vector<Book>::const_iterator it = library.getBooks().begin(); it != library.getBooks().end(); ++it) {
        booksFile << it->getId() << '\t' << it->isAvailable() << '\t' << it->isArchived() << '\t'
                  << it->getTitle() << '\t' << it->getAuthor() << '\t' << it->getCategory() << '\n';
    }
    for (std::vector<Member>::const_iterator it = library.getMembers().begin(); it != library.getMembers().end(); ++it) {
        membersFile << it->getId() << '\t' << it->isActive() << '\t' << it->getName() << '\t'
                    << it->getPhone() << '\t' << it->getEmail() << '\t'
                    << joinIds(it->getBorrowedBookIds()) << '\n';
    }
    for (std::vector<BorrowRecord>::const_iterator it = library.getRecords().begin(); it != library.getRecords().end(); ++it) {
        recordsFile << it->getRecordId() << '\t' << it->getBookId() << '\t' << it->getMemberId() << '\t'
                    << it->getIssueDate() << '\t' << it->getReturnDate() << '\n';
    }
    if (!booksFile || !membersFile || !recordsFile) { message = "A file write failed."; return false; }
    message = "Data saved to " + dataDirectory;
    return true;
}

bool FileManager::load(Library& library, const std::string& dataDirectory,
                       std::string& message) {
    std::ifstream booksFile(dataDirectory + "/books.txt");
    std::ifstream membersFile(dataDirectory + "/members.txt");
    std::ifstream recordsFile(dataDirectory + "/records.txt");
    if (!booksFile && !membersFile && !recordsFile) {
        message = "No saved data found; starting with an empty library.";
        return true;
    }
    if (!booksFile || !membersFile || !recordsFile) {
        message = "One or more data files are missing or cannot be read.";
        return false;
    }

    std::vector<Book> books;
    std::vector<Member> members;
    std::vector<BorrowRecord> records;
    std::string line;
    try {
        while (std::getline(booksFile, line)) {
            const std::vector<std::string> part = splitTabs(line);
            bool available, archived;
            if (part.size() != 6 || !parseFlag(part[1], available) || !parseFlag(part[2], archived)) throw 1;
            Book book(std::stoi(part[0]), part[3], part[4], part[5], available, archived);
            if (!book.isValid()) throw 1;
            books.push_back(book);
        }
        while (std::getline(membersFile, line)) {
            const std::vector<std::string> part = splitTabs(line);
            bool active;
            std::vector<int> borrowedIds;
            if (part.size() != 6 || !parseFlag(part[1], active) || !parseIds(part[5], borrowedIds)) throw 1;
            Member member(std::stoi(part[0]), part[2], part[3], part[4], active, borrowedIds);
            if (!member.isValid()) throw 1;
            members.push_back(member);
        }
        while (std::getline(recordsFile, line)) {
            const std::vector<std::string> part = splitTabs(line);
            if (part.size() != 5) throw 1;
            records.push_back(BorrowRecord(std::stoi(part[0]), std::stoi(part[1]), std::stoi(part[2]), part[3], part[4]));
        }
    } catch (...) {
        message = "A data file has an invalid format.";
        return false;
    }
    library.replaceData(std::move(books), std::move(members), std::move(records));
    message = "Saved data loaded successfully.";
    return true;
}
