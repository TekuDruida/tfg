#include "event.h"
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <map>
#include "freeling.h"
#include "freeling/morfo/traces.h"
using namespace std;
using namespace freeling;

featGenerator::featGenerator() {
	this->lexicalfeats = this->syntactfeats = false;
	numericFeatures[] = {"nWord","nSen","nVerb"};
}

featGenerator::featGenerator(bool lexicalfeats, bool syntactfeats) {
	this->lexicalfeats = lexicalfeats;
	this->syntactfeats = syntactfeats;
	numericFeatures[] = {"nWord","nSen","nVerb"};
}

//Add elements to structures
void featGenerator::addEvent(event e) {
	this->events.push_back(e);
}
void featGenerator::addFeatToDic(string feat) {
    feat = trataNumeric(feat);
    if(this->dic.find(feat) != this->dic.end()) this->dic[feat] += 1;
    else this->dic[feat] = 1;
}
void featGenerator::addConexion(string ei, string ej) {
	pair<string, string> p = make_pair(ei,ej);
    this->conexions.push_back(p);
}

bool featGenerator::openFile(int i, string path) {
	if (i >= 0 and i < 2) 
		this->out[i].open(path);
	else
		cout << "Solo se pueden abrir 2 ficheros, 0 -> features, 1 -> diccionario" << endl;

	return this->out[i].is_open();
}

//Print structures to control
void featGenerator::printEvents() {
	cout << "Print events" << endl;
	for(auto e: this->events) {
        wstring type = e.type;
        int start_offs = e.start_offs;
        int end_offs = e.end_offs;
        string id = e.id;
        string word = e.word;
        cout << word << " starts in: " << start_offs << " ends: " << end_offs << " id: " << id << " ";

        const freeling::word w = *e.w;
        int pos = e.pos;
        int sen = e.sen;
        cout << "pos: " << pos << " en la frase " << sen << " ";
        wcout << "form: " << w.get_form() << endl;
    }
}
void featGenerator::printConexions() {
	cout << "Conexions: " << endl;
    for (auto c : this->conexions) {
        cout << c.first << " - " << c.second << " | ";
    }
    cout << endl;
}

//Principal functions, generate features and dicctionary
void featGenerator::printDic(int numOfClasses) {
	if(out[1].is_open()) {
		int i = numOfClasses;

	    vector< pair<string,int> > vecAux(this->dic.begin(), this->dic.end());
	    sort(vecAux.begin(), vecAux.end(), &sortFunc);
	    for(auto p : vecAux) {out[1] << i << " " << p.first << " " << p.second << endl; ++i;}
	}
	else cout << "no se ha abierto el fichero del diccionario" << endl; 
}
void featGenerator::printFeat(string feat) {
	if(out[0].is_open()) {
		std::replace(f.begin(),f.end(),' ','_');
    	out[0] << f << " ";
	}
	else cout << "no se ha abierto el fichero de features" << endl;
}

void featGenerator::printFeatsSet(set<string> feats) {
	for(auto f : feats) {
        printFeat(f);
    }
}

void featGenerator::generateFeatures(list<sentence> &ls) {
	if(out[0].is_open()) {
		//Create features:
	    int i = 0;
	    for (auto ei : this->events) {
	    	int j = 0;
	        for (auto ej : this->events) {
	            string event = ei.id;
	            string related = ej.id;
	                        
	        	if (event != related and ei.id[0] != 't' and i < j and ((ei.sen - ej.sen) == 1 or (ei.sen - ej.sen) == 0)){

	                //Features from Freeling:
	                if(this->lexicalfeats) {
	                    generateLexicalFeats(ei,ej,ls);
	                }
	                if(this->syntactfeats) {
	                    generateSyntacticalFeats(ei, ej, ls);
	                }
	                out[0] << endl;
	        	}
	        ++j;
	        }
	    ++i;
	    }
	    out[0] << endl;
	}
	else cout << "no se ha abierto el fichero de features" << endl;
}

void featGenerator::generateLexicalFeats(event &ei, event &ej, list<sentence> &ls) {
	set<string> repetableFeats;
    int nWord = 0;
    int nSen = 0;
    int nVerb = 0;
    int ssen,spos,fsen,fpos;

    //out[0] << "word1:"<<ei.id<<endl;;
    //out[0] << "word2:"<<ej.id<<endl;


    const freeling::word *fword, *sword;

    if(ei.start_offs < ej.start_offs) {
        fword = ei.w; fsen = ei.sen; fpos = ei.pos;
        sword = ej.w; ssen = ej.sen; spos = ej.pos;
    }
    else {
        fword = ej.w; fsen = ej.sen; fpos = ej.pos;
        sword = ei.w; ssen = ei.sen; spos = ei.pos;
    }

    //out[0] << "pos1:"<<fpos<<endl;
    //out[0] << "pos2:"<<spos<<endl;
    //out[0] << "sen1:"<<fsen<<endl;
    //out[0] << "sen2:"<<ssen<<endl;

    //first word
    std::list<sentence>::iterator sen = ls.begin();
    for(int i = 0; i < fsen; ++i) ++sen;
    sentence::const_iterator w = sen->begin();
    for(int i = 0; i < fpos; ++i) ++w;
    if(w != sen->begin()) {
        --w;
        string f1before = "w1Before="+util::wstring2string(w->get_form());
        printFeat(f1before);
        ++w;
    }
    ++w;
    if(w != sen->end()) {
        string f1after = "w1After="+util::wstring2string(w->get_form());
        printFeat(f1after);
    }
    --w;
    bool fin = false;
    int senAct = fsen; int wordAct = fpos;

    //second word
    if (ej.id == "t0") printFeat("w1t0");
    else {
        while(not fin and sen != ls.end()) {
            while(not fin and w != sen->end()) {
            	//out[0] << "posACT:"<<wordAct<<endl;
        		//out[0] << "senACT:"<<senAct<<endl;
            	
            	//out[0] << util::wstring2string(w->get_form())<<endl;
                string form = "wordInMiddle="+util::wstring2string(w->get_form());
                string lemma = "WIMLemma="+util::wstring2string(w->get_lemma());
                repetableFeats.insert(form);
                repetableFeats.insert(lemma);
                
                string tag = util::wstring2string(w->get_tag());
                if(tag[0]=='V') ++nVerb;

                if(senAct == ssen and wordAct == spos) {
                    fin = true;
                    if (w != sen->begin()) {
                        --w; 
                        string f2before = "w2Before="+util::wstring2string(w->get_form());
                        printFeat(f2before);
                        ++w;
                    }
                    
                    ++w;
                    if (w != sen->end()) {
                        string f2after = "w2After="+util::wstring2string(w->get_form());
                        printFeat(f2after);
                    }
                    --w;
                }
                //out[0] << "nWord:"<<nWord<<endl;
        		//out[0] << "word:"<<util::wstring2string(w->get_form())<<endl;

                ++nWord;
                ++w;
                ++wordAct;
            }
            ++senAct;
            ++sen;
            ++nSen;
            w = sen->begin();
            wordAct=0;
        }
    }
    //sen->get_words().size();


    //print Forms and Lemmas
    string f1lemma = "w1Lemma="+util::wstring2string(fword->get_lemma());
    string f2lemma = "w2lemma="+util::wstring2string(sword->get_lemma());
    string f1form = "w1form="+util::wstring2string(fword->get_form());
    string f2form = "w2form="+util::wstring2string(sword->get_form());
    printFeat(f1form); printFeat(f2form); printFeat(f1lemma); printFeat(f2lemma);

    printFeatsSet(repetableFeats);

    //If some word is a DATE, print extra information
    if (util::wstring2string(fword->get_tag()) == "W") {
        string dateInfo = util::wstring2string(fword->get_lemma());
        printDateInfo(dateInfo, "w1");
    }
    if (util::wstring2string(sword->get_tag()) == "W") {
        string dateInfo = util::wstring2string(sword->get_lemma());
        printDateInfo(dateInfo, "w2");
    }

    //Print the distances
    if (ej.id != "t0") {
        string f8 = "nWord="+to_string(nWord);
        string f9 = "nVerb="+to_string(nVerb);
        string f10 = "nSen="+to_string(nSen);
        printFeat(f8); printFeat(f9); printFeat(f10);
    }
}

void featGenerator::generateSyntacticalFeats(event &ei, event &ej, list<sentence> &ls) {
	set<string> repetableFeats;
    list<freeling::sentence>::const_iterator s;
    freeling::dep_tree::const_iterator p;
    int spanS, spanF;
    bool found = false;

    word fword = *ei.w;
    word sword = *ej.w;

    //select the sentence of the word
    s=ls.begin();
    for (int i = 0; i < ei.sen; ++i) ++s;

    //select the word in the dep_tree
    p=s->get_dep_tree().begin();
    while  (p!=s->get_dep_tree().end()) {
        word w = p->get_word();
        spanS = w.get_span_start();
        spanF = w.get_span_finish();
        if((ei.start_offs == spanS) or (ei.start_offs >= spanS and ei.end_offs <= spanF)) {found = true; break;}
        ++p;
    }
    /*found = true;
    dep_tree::const_iterator pi = p->get_node_by_pos(ei.pos);
    */

    //If found the word print the features
    if(found and ei.id != "t0") {
        if (not fword.get_senses().empty()) {
            string sens = util::wstring2string(fword.get_senses().begin()->first);
            string feat = "w1Sense_"+sens; printFeat(feat);
        }

        string tag = util::wstring2string(fword.get_tag());
        if(tag[0]=='V') {
            for (freeling::dep_tree::const_sibling_iterator ch = p.sibling_begin(); ch!=p.sibling_end(); ch++) {
                string label = util::wstring2string(ch->get_label());
                if (label=="SBJ" or label=="OBJ" or (label[0]=='A' and label[1]=='M')) {
                    string lemma = util::wstring2string(ch->get_word().get_lemma());
                    string feat = "w1"+label+"_"+lemma; //printFeat(feat);
                    repetableFeats.insert(feat);
                }

                if (&ch->get_word() == ej.w) {
                    string form = util::wstring2string(ch->get_word().get_form());
                    string feat = "role12_"+label+"_"+form; printFeat(feat);
                }
            } 
        }
    }

    found = false;

    //select the sentence of the word
    s=ls.begin();
    for (int i = 0; i < ej.sen; ++i) ++s;

    //select the word in the dep_tree
    p=s->get_dep_tree().begin();
    while  (p!=s->get_dep_tree().end()) {
        word w = p->get_word();
        spanS = w.get_span_start();
        spanF = w.get_span_finish();
        if((ej.start_offs == spanS) or (ej.start_offs >= spanS and ej.end_offs <= spanF)) {found = true; break;}
        ++p;
    }
    /*found = true;
    dep_tree::const_iterator pi = p->get_node_by_pos(ej.pos);
    */

    //If found the word print the features
    if(found and ej.id != "t0") {
        if (not sword.get_senses().empty()) {
            string sens = util::wstring2string(sword.get_senses().begin()->first);
            string feat = "w2Sense_"+sens; printFeat(feat);
        }

        string tag = util::wstring2string(sword.get_tag());
        if(tag[0]=='V') {
            for (freeling::dep_tree::const_sibling_iterator ch = p.sibling_begin(); ch!=p.sibling_end(); ch++) {
                string label = util::wstring2string(ch->get_label());
                if (label=="SBJ" or label=="OBJ" or (label[0]=='A' and label[1]=='M')) {
                    string lemma = util::wstring2string(ch->get_word().get_lemma());
                    string feat = "w2"+label+"_"+lemma; //printFeat(feat);
                    repetableFeats.insert(feat);
                }

                if (&ch->get_word() == ei.w) {
                    string form = util::wstring2string(ch->get_word().get_form());
                    string feat = "role21_"+label+"_"+form; printFeat(feat);
                }
            }
        }
    }
    printFeatsSet(repetableFeats);
}

//gets
list<event> featGenerator::getEvents() {
	return this->events;
}

//sets
void featGenerator::setBooleans(bool lexicalfeats, bool syntactfeats) {
	this->lexicalfeats = lexicalfeats;
	this->syntactfeats = syntactfeats;
}


//private functions
string featGenerator::trataNumeric(string f) {
	string tag = f.substr(0,f.find('='));
    for (string n : this->numericFeatures) {
        if(tag == n) return tag;
    }
    return f;
}
void featGenerator::printDateInfo(string dateInfo, string word) {
	dateInfo = dateInfo.substr(1,(dateInfo.size()-2));
    vector<string> infos; string subdate = "";
    for (char c : dateInfo) {
        if(c == ':') {infos.push_back(subdate); subdate = "";}
        else subdate += c;
    }
    infos.push_back(subdate);

    if(infos[0] != "??") printFeat(word+"dayOfWeek="+infos[0]);
    if(infos[3] != "??") printFeat(word+"time="+infos[2]+infos[3]);

    vector<string> d; subdate = "";
    for (char c : infos[1]) {
        if(c == '/') {d.push_back(subdate); subdate = "";}
        else subdate += c;
    }
    d.push_back(subdate);

    if(d[0] != "??") printFeat(word+"dayOfMonth="+d[0]);
    if(d[1] != "??") printFeat(word+"Month="+d[1]);
    if(d[2] != "??") printFeat(word+"Year="+d[2]);
}

bool featGenerator::sortFunc(pair<string,int> first, pair<string,int> second) { return (first.second > second.second); }

