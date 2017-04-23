#ifndef _FEATGEN_H
#define _FEATGEN_H

#include "event.h"
#include <map>
#include <vector>
#include <string>
#include <list>

class featGenerator
{
public:
	list<event> events;
	map<string, int> dic;
	string numericFeatures[];
	list< pair<string, string> > conexions;
	bool lexicalfeats, syntactfeats;
	ofstream out[2];


	featGenerator();
	featGenerator(bool lexicalfeats, bool syntactfeats);
	~featGenerator();

	//Add elements to structures
	void addEvent(event e);
	void addFeatToDic(string feat);
	void addConexion(string ei, string ej);
	bool openFile(int i, string path);
	
	//Print structures to control
	void printEvents();
	void printConexions();

	//Principal functions, generate features and dicctionary
	void printDic(ofstream &out, int numOfClasses);
	void printFeat(ofstream &out, string feat);
	void printFeatsSet(set<string> feats);
	void generateFeatures(list<sentence> &ls);
	void generateLexicalFeats(event &ei, event &ej, list<sentence> &ls);
	void generateSyntacticalFeats(event &ei, event &ej, list<sentence> &ls);

	//gets
	list<event> getEvents();

	//sets
	void setBooleans(bool lexicalfeats, bool syntactfeats);

private:
	string trataNumeric(string f);
	void printDateInfo(string dateInfo, string word);

	bool sortFunc(pair<string,int> first, pair<string,int> second);
};

#endif