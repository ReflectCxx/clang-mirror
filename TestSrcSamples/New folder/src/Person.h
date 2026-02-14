
#pragma once

#include <string>

class Person
{
    const std::string m_dob;
    const std::string m_firstName;
 
    std::string m_address;
    std::string m_lastName;

public:

    Person(const std::string& pName, std::string pDob) noexcept;

    std::string getFirstName() const;

    void setAddress(std::string pAddress);

    void setLastName(std::string pLastName);

    static std::string getAccessCard(const std::string& pVisitPurpose, const Person& pObj);
};