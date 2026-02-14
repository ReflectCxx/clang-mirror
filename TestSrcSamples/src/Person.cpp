
#include "Person.h"

Person::Person(const std::string& pName, std::string pDob)
	: m_firstName(pName)
	, m_dob(pDob)
	, m_address("NA")
	, m_lastName("NA")
{ }


std::string Person::getFirstName() const
{
	return m_firstName;
}


void Person::setAddress(std::string pAddress)
{
	m_address = pAddress;
}


void Person::setLastName(std::string pLastName)
{
	m_lastName = pLastName;
}


std::string Person::getAccessCard(const std::string& pVisitPurpose, const Person& pObj)
{
	return ("\nName: " + pObj.m_firstName + " " + pObj.m_lastName +
		    "\nDOB: " + pObj.m_dob +
		    "\nAddress: " + pObj.m_address +
			"\nPurpoe of visit: " + pVisitPurpose);
}