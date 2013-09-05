#pragma once

class NoCopy{
protected:
	NoCopy(){}
	~NoCopy(){}
private:
	NoCopy(const NoCopy&);
	const NoCopy & operator = (const NoCopy &);
};