#ifndef MULTIPLE_EXCEPTION
#define MULTIPLE_EXCEPTION

#include <memory>

namespace safini
{

class MultipleException : public std::exception
{
public:
    MultipleException(const MultipleException&) noexcept = default;

    MultipleException(const std::string_view whatString)
    {
        m_ExceptionStringPtr->append("\n\t")
                             .append(whatString);
    }

    void addException(const std::string_view whatString)
    {
        SharedStorage newExceptionStringStorage{std::make_shared<std::string>(*m_ExceptionStringPtr)};
        newExceptionStringStorage->append("\n\t")
                                  .append(whatString);
        m_ExceptionStringPtr = newExceptionStringStorage;
    }

    virtual const char* what() const noexcept override
    {
        return m_ExceptionStringPtr->c_str();
    }

    virtual ~MultipleException() = default;

private:
    //we're doing COW because we need noexcept copy constructor
    using SharedStorage = std::shared_ptr<std::string>;
    SharedStorage m_ExceptionStringPtr{std::make_shared<std::string>("MultipleException:")};
};

}

#endif // MULTIPLE_EXCEPTION
