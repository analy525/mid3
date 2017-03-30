//modified by: Analy Velazquez
//date: March 9, 2017
//why: to optimize this lab for speed
//
//lab-7 framework
//This program does spell checking of a text string.
//
//Written by Gordon Griesel
//Mar  8, 2017
//
//Students will optimize this program's running time.
//Also, profile the functions outside of main.
//Optimization should be a refactoring operation.
//
//Enable profiling calls with a preprocessor directives such as this:
//
#define PROFILE_ON
//
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cctype>
#include <fstream>
#include <time.h>
#include <unistd.h>
using namespace std;

void spell_check(char *text);
const char fname[] = "/usr/share/dict/cracklib-small";
char article[] =
"In software engineering, a software development methodology (also known as \
a system development methodology, software development life cycle, software \
development process, software process) is a splitting of software development \
work into distinct phases (or stages) containing activities with the intent \
of better planning and management. It is often considered a subset of the \
systems development life cycle. The methodology may include the \
definition of specific deliverables and artifacts that are created and \
completed by a project team to develop or maintain an application. Common \
methodologies include waterfall, prototyping, iterative and incremental \
development, spiral development, rapid application development, extreme \
programming and various types of agile methodology. Some people consider a \
life-cycle model a more general term for a category of methodologies and a \
software development process a more specific term to refer to a specific \
process chosen by a specific organization. For example, there are many \
specific software development processes that fit the spiral life-cycle \
model. A variety of such frameworks have evolved over the years, each with \
its own recognized strengths csubrules and weaknesses. \
One software development methodology framework is not necessarily suitable \
for use by all projects. Each of the available methodology frameworks are \
best suited to specific kinds of projects, based on various technical, \
organizational, project and team considerations. \
Software development organizations implement process methodologies to ease \
the process of development. Sometimes, contractors may require methodologies \
employed, an example is the U.S. defense industry, which requires a rating \
based on process models to obtain contracts. The international standard for \
describing the method of selecting, implementing and monitoring the life \
cycle for software is ISO/IEC 12207. A decades long goal has been to find \
repeatable, predictable processes that improve productivity and quality. \
Some try to systematize or formalize the seemingly unruly task of designing \
software. Others apply project management techniques to designing software. \
Without effective project management, software projects can easily be \
delivered late or over budget. With large numbers of software projects not \
meeting their expectations in terms of functionality, cost, or delivery \
schedule, it is effective project management that appears to be lacking. \
Organizations may create a Software Engineering Process Group (SEPG), which \
is the focal point for process improvement. Composed of line practitioners \
who have varied skills, the group is at the center of the collaborative \
effort of everyone in the organization who is involved with software \
engineering process improvement. A particular development team may also \
agree to programming environment details, such as which integrated \
development environment is used, and one or more dominant programming \
paradigms, programming style rules, or choice of specific software libraries \
or software frameworks. These details are generally not dictated by the \
choice of model or general methodology. History. The software development \
methodology (also known as waterfall) framework didn't emerge until the 1960s. \
According to Elliott (2004) the systems development life cycle (SDLC) can be \
considered to be the oldest formalized methodology framework for building \
information systems. The main idea of the SDLC has been to pursue the \
development of information systems in a very deliberate, structured and \
methodical way, requiring each stage of the life cycle from inception of \
the idea to delivery of the final system to be carried out rigidly and \
sequentially within the context of the framework being applied. The \
main target of this methodology framework in the 1960s was to develop \
large scale functional business systems in an age of large scale business \
conglomerates. Information systems activities revolved around heavy data \
processing and number crunching routines. \
Methodologies, processes, and frameworks range from specific proscriptive \
steps that can be used directly by an organization in day-to-day work, to \
flexible frameworks that an organization uses to generate a custom set of \
steps tailored to the needs of a specific project or group. In some cases a \
sponsor or maintenance organization distributes an official set of \
documents that describe the process. \
Several software development approaches have been used since the origin of \
information technology, in two main categories. Typically an approach or a \
combination of approaches is chosen by management or a development team. \
Traditional methodologies such as waterfall that have distinct phases are \
sometimes known as software development life cycle SDLC methodologies, \
though this term could also be used more generally to refer to any \
methodology. A project could last for years and years, or you could zip \
right through. You plan for the worst. A life cycle approach with distinct \
phases is in contrast to Agile approaches which define a process of \
iteration, but where design, construction, and deployment of different \
pieces can occur simultaneously.";

//=============FROM ASTEROIDS FRAMEWORK=========================================
//Set up timers
const double physicsRate=1.0 / 60.0;
const double oobillion = 1.0 / 1e9;
struct timespec timeStart, timeCurrent;
struct timespec timePause;
double timeSpan=0.0;
double timeDiff( struct timespec *start, struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec) +
        (double)(end->tv_nsec - start->tv_nsec) * oobillion;
}
void timeCopy(struct timespec *dest, struct timespec *source) {
    memcpy(dest, source, sizeof(struct timespec));
}
//==============================================================================

int main(int argc, char *argv[])
{
	//This program spell-checks some text.
	char *txt = new char[1000000];
	strcpy(txt, article);
	if (argc > 1) {
		//There is a command-line argument.
		if (argv[1][0] == '.' || argv[1][0] == '/') {
			//argument is a path and file name...
			void loadfile(char *fname, char *txt);
			txt[0] = '\0';
            loadfile(argv[1], txt);
			cout << "text loaded from an external file.\n";
    //cout << "time of load check function: " <<timeSpan <<endl;  
		} 
        
    //cout << "time loadfile "<< timeSpan;
        else {
			//argument is a string of text..
			strcpy(txt, argv[1]);
			cout << "text loaded from command-line.\n";
		}
	} else {
		cout << "text loaded from internal string.\n";
	}


#ifdef PROFILE_ON
    //get start tim
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeStart);
#endif 
	spell_check(txt);
#ifdef PROFILE_ON
    //get end time
    //calculate real time
    clock_gettime(CLOCK_REALTIME, &timePause);
    clock_gettime(CLOCK_REALTIME, &timeCurrent);
    timeSpan = timeDiff(&timeStart, &timeCurrent);
    timeCopy(&timeStart, &timeCurrent);
    cout << "time of spell check function: " <<timeSpan <<endl;  
#endif 
//    spell_check(txt);
    delete [] txt;
	return 0;
}

void loadfile(char *fname, char *txt)
{
	ifstream fin(fname);
	if (fin.fail()) {
		cout << "Error opening **" << fname << "**" << endl;	
		txt[0] = '\0';
		return;
	}
	char line[200];
	fin >> line;
	while (!fin.eof()) {
		strcat(txt, " ");
        strcat(txt, line);
		fin >> line;
        
    //clock_gettime(CLOCK_REALTIME, &timeCurrent);
	}
    fin.close();
}

void spell_check(char *text)
{
	ifstream fin(fname);
	if (fin.fail()) {
		cout << "Dictionary file failed to open!\n\n" << endl;
		return ;
	}
	cout << "Spell checking the text..." << endl;
	//p1 and p2 are pointers into the article text.
	//char *p1 = article;
	char *p1 = text;
	char *p2 = p1;
	int n=0; 
    int done=0;
	//do until *p1 has reached its null termination.
	while (*p1 != '\0') {
		//Make sure p1 is at a character to start with.
        while (!isalpha(*p1)) 
            ++p1;
		//Get a word by moving p2 to the first whitespace,
		//or you might find a punctuation character, or somesuch.
        p2 = p1+1; 
       while (*p2 != '\0') {
			//A whitespace ends a word for sure.
			if (isspace(*p2))
				break;
			//Most non-alphabetic characters end a word, but not all.
			//Don't skip contractions or possessives.
			if (!isalpha(*p2) && *p2 != '\'')
				break;
			++p2;
        }
    
		if (*p2 == '\0')
			done=1;
		*p2 = '\0';
        //return;
		//Now p1 is the start of some word in the text.
		//Is the word in the dictionary?

		int search_for_word(ifstream &fin, char *word);
		int found = search_for_word(fin, p1);
        if (found != 1) {
			cout << "Word not found in dictionary: " << p1 << endl;
			++n;
            //return;
		}
        //return
        if (done) 
        break;
		//Move the word pointer just past the end of the last word.
        
        p1 = p2+1;
    }
	fin.close();
	cout << "There were " << n << " misspelled words found in the article.\n";
	cout << "File closed.\n";
	cout << "Spell check complete.\n";
	cout << "Total seconds spent in search_for_words(): ???\n";
	cout << endl;
    
	return ;
}

int search_for_word(ifstream &fin, char *word)
{
	//returns: 0 = word not found.
	//         1 = word found.
	//
	//Go to the top of the dictionary file.
	fin.clear();
	fin.seekg(0, fin.beg);
	//Now search through the whole file looking for the word.
	char line[200];
	int found=0;
	fin >> line;
	while (!fin.eof()) {
		//Make sure both words are the same case.
		int slen = strlen(line);
		slen = strlen(word);
		for (int i=0; i<slen; i++){
			line[i] = (char)tolower(line[i]);
		//slen = strlen(word);
		//for (int i=0; i<slen; i++)
			word[i] = (char)tolower(word[i]);
            //return 1;
        }
		//Compare words, for spelling match.
		if (strcmp(line, word) == 0)
			found=1;
	//	return 1;
		fin >> line;
	}
	if (found == 1)
		return 1;
	else
		return 0;

        //    return 1;
}






