#ifndef __COUNTABLEMAP_H__
#define __COUNTABLEMAP_H__

#include <map>

using std::map;

template <class t>
class CountableMap
{
private:
	typedef map<t,int> table_t;
	typedef map<int,t> table_f;
	table_f fable;
	table_t table;
	int counter;

public:
	CountableMap() : counter(0) { }
	
	int Get(t key){
		typename table_t::iterator iter = table.find(key);
		if(iter == table.end()){
			table[key] = counter;
			fable[counter] = key;
			return counter++;
		}
		else return iter->second;
	}
	t getKey(int value){
		return fable[value];
	}

	int getSize() {return counter;}
};

#endif
