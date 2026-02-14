
#pragma once

#include <string>
#include <string_view>

class Person
{
    const std::string m_dob;
    const std::string m_firstName;
 
    std::string m_address;
    std::string m_lastName;

public:

	Person() = default;
	Person(Person&&) = default;
	Person(const Person&) = default;
	
    Person(const std::string_view pName, const std::string_view pDob);
	
    std::string getFirstName() const;

    void setAddress(const std::string_view pAddress);

    void setLastName(const std::string_view pLastName);

    static std::string getAccessCard(const std::string_view pVisitPurpose, const Person& pObj);
};