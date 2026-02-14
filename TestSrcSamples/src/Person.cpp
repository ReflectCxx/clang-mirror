
#include "Person.h"

Person::Person(const std::string_view pName, const std::string_view pDob)
	: m_firstName(pName)
	, m_dob(pDob)
	, m_address("NA")
	, m_lastName("NA")
{ }


std::string Person::getFirstName() const
{
	return m_firstName;
}


void Person::setAddress(const std::string_view pAddress)
{
	m_address = std::string(pAddress);
}


void Person::setLastName(const std::string_view pLastName)
{
	m_lastName = std::string(pLastName);
}


std::string Person::getAccessCard(const std::string_view pVisitPurpose, const Person& pObj)
{
	return (std::string("\nName: ") + pObj.m_firstName + " " + pObj.m_lastName +
		    "\nDOB: " + pObj.m_dob +
		    "\nAddress: " + pObj.m_address +
			"\nPurpoe of visit: " + std::string(pVisitPurpose));
}