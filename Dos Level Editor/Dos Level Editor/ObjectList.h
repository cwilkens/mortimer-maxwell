// header for the objectlist class
// controls a list of objects

#include <functional>
#include <algorithm>
#include <vector>

#ifndef OBJECTLIST_H
#define OBJECTLIST_H

template <class Type>
class ObjectList {

public:
	void AddObject(Type &obj);
	void RemoveObject(Type &obj);
	void StepAll();
	void DrawAll();

private:
	std::vector<Type*> list;
};


//---------definitions--------------
// add an object to the list
template <class Type> void ObjectList<Type>::AddObject(Type &obj)
{
	list.push_back(&obj);
}

// add an object to the list
template <class Type> void ObjectList<Type>::RemoveObject(Type &obj)
{
	//create an iterator to look through the list for the specified object
	std::vector<Type>::iterator list_iter = list.begin();
	while(list_iter != list.end())
	{
		if (&list_iter == &obj)
			list.erase(list_iter); // erase the object if it is the one specified
		list_iter++;
	}
}

// step all the objects in the list
template <class Type> void ObjectList<Type>::StepAll()
{
	for_each(list.begin(), list.end(), std::mem_fun(&Type::Step) );
}

// draw all the objects in the list
template <class Type> void ObjectList<Type>::DrawAll()
{
	for_each(list.begin(), list.end(), std::mem_fun(&Type::Draw) );
}

#endif /* OBJECTLIST_H */