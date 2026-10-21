#ifndef BOOK_H
#define BOOK_H

#include <iosfwd>
#include <string>

class Book {
private:
    int id_;
    std::string title_;
    std::string author_;
    std::string category_;
    bool available_;
    bool archived_;

public:
    Book(int id, std::string title, std::string author, std::string category,
         bool available = true, bool archived = false);

    int getId() const;
    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getCategory() const;
    bool isAvailable() const;
    bool isArchived() const;
    bool canBeIssued() const;
    bool isValid() const;

    bool updateDetails(const std::string& title, const std::string& author,
                       const std::string& category);
    bool archive();
    bool markIssued();
    bool markReturned();
    void display(std::ostream& output) const;

    static bool hasText(const std::string& value);
    static bool isSafeText(const std::string& value);
};

#endif
