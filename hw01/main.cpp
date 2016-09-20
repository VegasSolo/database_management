/*
	Author(s):		Dillon VanBuskirk & Jaewung Ryu
	Date:			September 11, 2016
	Title:			CSCE 4523 Database Management Homework #1
*/

//#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <fstream>
using namespace std;

const int RECORD_SIZE = 71;
const string HEADER = "id    experience married wage         industry                        ";
const int RECORD_OVERFLOW = 4;

int NUM_RECORDS = 0;
int NUM_RECORDS_OVERFLOW = 0;
string currentDatabase = "";
string overflowDatabase = "overflow.txt";
bool databaseOpen = false;



bool GetRecord(ifstream &Din, const int RecordNum, string &Id, int &Experience, string &Married, float &Wage, string &Industry);
bool binarySearch(ifstream &Din, const string Id, int &Experience, string &Married, float &Wage, string &Industry);
bool overflowSearch(ifstream &Din, const string Id, int &Experience, string &Married, float &Wage, string &Industry);
void showMenu();
void createNewDatabase(string filename);
bool openDatabase(fstream &file, string filename);
bool closeDatabase(fstream &file);
void displayRecord(const string Id);
bool updateRecord(); 
void createReport(); 
void addRecord();
bool deleteRecord(); 
void getRecordCount(fstream &file);
void mergeFiles();

int main()
{
	string Id = "ID";
	int Experience = -1;
	string Married = "UNKNOWN";
	float Wage = -1.0;
	string Industry = "INDUSTRY";
	int choice;
	string filename;
	fstream file;
	
	do {
		showMenu();
		cin >> choice;
		switch (choice) {
		case 1:
			//cout << "Create new database" << endl;
			cout << "Please type the name of the file you wish to create. Do not include extensions: " << endl;
			cin >> filename;
			filename += ".txt";
			cout << filename << endl;
			createNewDatabase(filename);
			break;


		case 2:
			//cout << "Open database" << endl;
			if (!databaseOpen) {
				cout << "Please type the name of the file you wish to open. Do not include extensions: " << endl;
				cin >> filename;
				filename += ".txt";
				if (!(openDatabase(file, filename)))
					cout << "Error opening file. Try again." << endl;
				else {
					currentDatabase = filename;
					databaseOpen = true;
					cout << currentDatabase << " was opened." << endl;
					getRecordCount(file);
				}
			}
			else
				cout << "A database is already open. Please close it before continuing." << endl;
			break;


		case 3:
			//cout << "Close database" << endl;
			if (databaseOpen) {
				if (!(closeDatabase(file)))
					cout << "Error closing the file. Try again." << endl;
				else {
					databaseOpen = false;
					currentDatabase = "";
					cout << "Database successfully closed." << endl;
				}
			}
			else
				cout << "No database is open." << endl;
			break;


		case 4:
			//cout << "Display record" << endl;
			if (databaseOpen) {
				cout << "Please insert the id that you wish to display: " << endl;
				cin >> Id;
				displayRecord(Id);
			}
			else
				cout << "You need to open a database first." << endl;
			break;


		case 5:
			//cout << "Update record" << endl;
			if (databaseOpen) {
				if (updateRecord())
					cout << "Record has been updated." << endl;
				else
					cout << "There was a problem updating the record." << endl;
			}
			else
				cout << "You need to open a database first." << endl;
			break;


		case 6:
			//cout << "Create report" << endl;
			if (databaseOpen) {
				createReport();
			}
			else
				cout << "You need to open a database first." << endl;
			break;


		case 7:
			//cout << "Add record" << endl;
			if (databaseOpen) {
				addRecord();
			}
			else
				cout << "You need to open a database first." << endl;
			break;


		case 8:
			//cout << "Delete record" << endl;
			if (databaseOpen) {
				deleteRecord();
			}
			else
				cout << "You need to open a database first." << endl;
			break;


		case 9:
			cout << "Quitting" << endl;
			if (databaseOpen)
				closeDatabase(file);
			break;


		default:
			cout << "Invalid input. Try again" << endl;
			break;
		}
	} while (choice != 9);

	cout << "Good bye." << endl;

	return 0;
}

/*Get record number n-th (from 1 to NUM_RECORDS) */
bool GetRecord(ifstream &Din, const int RecordNum, string &Id, int &Experience, string &Married, float &Wage, string &Industry)
{
	bool Success = true;
	string line;

	if ((RecordNum >= 1) && (RecordNum <= NUM_RECORDS))
	{
		Din.seekg(RecordNum * RECORD_SIZE, ios::beg);
		Din >> Id >> Experience >> Married >> Wage >> Industry;
	}
	else
		Success = false;

	return Success;
}

/*Binary Search record id */
bool binarySearch(ifstream &Din, const string Id, int &Experience, string &Married, float &Wage, string &Industry)
{
	int Low = 0;
	int High = NUM_RECORDS;
	int Middle;
	string MiddleId;

	bool Found = false;
	while (!Found && (High >= Low))
	{
		Middle = (Low + High) / 2;
		Din.seekg(Middle * RECORD_SIZE, ios::beg);
		Din >> MiddleId >> Experience >> Married >> Wage >> Industry;

		if (MiddleId == Id)
			Found = true;
		else if (MiddleId < Id)
			Low = Middle + 1;
		else
			High = Middle - 1;
	}
	return Found;
}

/* Searches the overflow for the record */
bool overflowSearch(ifstream &Din, const string Id, int &Experience, string &Married, float &Wage, string &Industry) 
{
	int i = 0;
	string searchedId = "";
	while (i < 4 && i < NUM_RECORDS_OVERFLOW) {
		Din >> searchedId >> Experience >> Married >> Wage >> Industry;
		if (searchedId == Id && Married != "null") {
			return true;
		}
		i++;
	}
	return false;
}

/* Displays the Menu of Operations */
void showMenu()
{
	cout << "Press [ 1 ] to Create a New Database" << endl;
	cout << "Press [ 2 ] to Open a Database" << endl;
	cout << "Press [ 3 ] to Close the Current Database" << endl;
	cout << "Press [ 4 ] to Display a Record" << endl;
	cout << "Press [ 5 ] to Update a Record" << endl;
	cout << "Press [ 6 ] to Create a Report" << endl;
	cout << "Press [ 7 ] to Add a New Record" << endl;
	cout << "Press [ 8 ] to Delete an Existing Record" << endl;
	cout << "Press [ 9 ] to Quit" << endl;
}

/* Creates a new database and writes the header */
void createNewDatabase(string filename)
{
	ofstream newFile;
	newFile.open(filename.c_str(), ios::trunc);
	if (!newFile.is_open())
		cout << "Error creating." << endl;
	newFile << HEADER << endl;
	newFile.close();
}

/* Opens a database */
bool openDatabase(fstream &file, string filename)
{
	ofstream ovflow;
	ovflow.open(overflowDatabase.c_str(), ios::trunc);
	ovflow.close();
	file.open(filename.c_str());
	if (file.is_open())
		return true;
	else
		return false;
}

/* Closes the current database */
bool closeDatabase(fstream &file)
{
	mergeFiles();
	file.close();
	if (file.is_open())
		return false;
	else
		return true;
}

/* Finds the requested record and displays it */
void displayRecord(string key)
{
	while (key.length() < 5) {
		key = "0" + key;
	}
	
	ifstream Din;
	Din.open(currentDatabase.c_str());
	string Id = key;
	int Experience = -1;
	string Married = "UNKNOWN";
	float Wage = -1.0;
	string Industry = "INDUSTRY";

	if (binarySearch(Din, Id, Experience, Married, Wage, Industry)) {
		if (Married == "null") {
			cout << "There is no record with the id " << Id << endl;
		}
		else {
			cout << "\nYou searched for id " << Id << " and we've found: " << endl;
			cout << "Experience: " << Experience << endl;
			cout << "Married: " << Married << endl;
			cout << "Wage: " << Wage << endl;
			cout << "Industry: " << Industry << endl;
			cout << endl;
		}
	}
	else {
		Din.close();
		Din.open(overflowDatabase.c_str());
		if (overflowSearch(Din, Id, Experience, Married, Wage, Industry)) {
			cout << "\nYou searched for id " << Id << " and we've found: " << endl;
			cout << "Experience: " << Experience << endl;
			cout << "Married: " << Married << endl;
			cout << "Wage: " << Wage << endl;
			cout << "Industry: " << Industry << endl;
			cout << endl;
		}
		else
			cout << "There is no record with the id " << Id << endl;
	}
	Din.close();
}

/* Prompts the user for a record that they wish to update fields */
bool updateRecord()
{
	int Experience = -1;
	string Married = "UNKNOWN";
	float Wage = -1.0;
	string Industry = "INDUSTRY";
	string Exp, Wag;

	string id;
	int choice;
	long pos;
	ifstream Din;
	ofstream Upd;
	Din.open(currentDatabase.c_str());
	cout << "Please enter the id of the record you wish to update: " << endl;
	cin >> id;
	while (id.length() < 5) {
		id = "0" + id;
	}
	if (binarySearch(Din, id, Experience, Married, Wage, Industry)) {
		pos = Din.tellg();
		pos = pos - 69 + (32 - Industry.length());
		Upd.open(currentDatabase.c_str(), ios::out | ios::in);
	}
	else {
		Din.close();
		Din.open(overflowDatabase.c_str());
		if (overflowSearch(Din, id, Experience, Married, Wage, Industry)) {
			pos = Din.tellg();
			pos = pos - 69 + (32 - Industry.length());
			Upd.open(overflowDatabase.c_str(), ios::out | ios::in);
		}
		else {
			cout << "There is no record with that id." << endl;
			return false;
		}
	}
	Din.close();

	displayRecord(id);
	cout << "Please enter [1] to update Experience, " << endl;
	cout << "             [2] to update Married, " << endl;
	cout << "             [3] to update Wage, " << endl;
	cout << "             [4] to update Industry, " << endl;
	cin >> choice;

	switch (choice) {
	case 1:
		cout << "Please enter the new value for experience: " << endl;
		cin >> Experience;
		if (Experience < 0 && Experience > 9999999999) {
			Experience = 0;
		}
		pos += 6;
		Exp = to_string(Experience);
		while (Exp.length() < 11)
			Exp += " ";
		Upd.seekp(pos);
		Upd << Exp;
		break;

	case 2:
		cout << "Please enter the new value for married: " << endl;
		cin >> Married;
		while (true) {
			if (Married != "yes" && Married != "no") {
				cout << "Sorry, but your entry for married is not quite right. Please enter yes or no: " << endl;
				cin >> Married;
			}
			else
				break;
		}
		pos += 17;
		while (Married.length() < 8)
			Married += " ";
		Upd.seekp(pos);
		Upd << Married;
		break;

	case 3:
		cout << "Please enter the new value for wage: " << endl;
		cin >> Wage;
		while (true) {
			if (to_string(Wage).length() > 12) {
				cout << "Sorry, but your entry for wage was too precise. Try again: " << endl;
				cin >> Wage;
			}
			else
				break;
		}
		pos += 25;
		Wag = to_string(Wage);
		while (Wag.length() < 13)
			Wag += " ";
		Upd.seekp(pos);
		Upd << Wag;
		break;

	case 4:
		cout << "Please enter the new value for industry: " << endl;
		cin >> Industry;
		while (true) {
			if (Industry.length() > 32) {
				cout << "Sorry, but your entry for industry was too long. Try again: " << endl;
				cin >> Industry;
			}
			else
				break;
		}
		pos += 38;
		while (Industry.length() < 32)
			Industry += " ";
		Upd.seekp(pos);
		Upd << Industry;
		break;

	default:
		cout << "Invalid input. Please try again." << endl;
		return false;
		break;
	}

	Upd.close();

	return true;
}

/* Displays the database in human-readable format for up to 10 records */
void createReport()
{
	ifstream Din;
	Din.open(currentDatabase.c_str());
	ofstream Dout;
	string report = currentDatabase;
	report.erase(report.size()-4,report.size());
	report += "_Report.txt";
	Dout.open(report.c_str(), ios::trunc);

	int count = 0;
	string Id = "ID";
	string Experience = "";
	string Married = "UNKNOWN";
	string Wage = "";
	string Industry = "INDUSTRY";
	string header = "";

	Din.seekg(0, ios::beg);
	Din >> header >> header >> header >> header >> header;
	while (count < 10 && count < NUM_RECORDS) {
		Din >> Id >> Experience >> Married >> Wage >> Industry;
		if (Married != "null") {
			Dout << "Id:             " << Id << endl;
			Dout << "Experience:     " << Experience << endl;
			Dout << "Married:        " << Married << endl;
			Dout << "Wage:           " << Wage << endl;
			Dout << "Industry:       " << Industry << endl;
			Dout << endl;
			count++;
		}
	}
	if (count < 10) {
		Din.close();
		Din.open(overflowDatabase.c_str());
		while (count < 10 && count < NUM_RECORDS_OVERFLOW) {
			Din >> Id >> Experience >> Married >> Wage >> Industry;
			if (Married != "null") {
				Dout << "Id:             " << Id << endl;
				Dout << "Experience:     " << Experience << endl;
				Dout << "Married:        " << Married << endl;
				Dout << "Wage:           " << Wage << endl;
				Dout << "Industry:       " << Industry << endl;
				Dout << endl;
				count++;
			}
		}
	}
	Din.close();
	Dout.close();
	cout << "A report has been generated and stored in " << report << endl << endl;
}

/* Takes input from user and adds a record to the database overflow */
void addRecord()
{
	bool overwrite = false;
	string Id = "ID";
	int Experience = -1, exp;
	string Married = "UNKNOWN", mar;
	float Wage = -1.0, wag;
	string Industry = "INDUSTRY", ind;
	ifstream Din;
	Din.open(currentDatabase.c_str());
	ifstream Din_over;
	Din_over.open(overflowDatabase.c_str());

	cout << "Please enter the new record's information: ";
	cout << "\n  Id:         ";
	cin >> Id;
	cout << "\n  Experience: ";
	cin >> Experience;
	cout << "\n  Married:    ";
	cin >> Married;
	cout << "\n  Wage:       ";
	cin >> Wage;
	cout << "\n  Industry:   ";
	cin >> Industry;

	while (Id.length() < 5) {
		Id = "0" + Id;
	}
	bool bad = true;
	while (bad) {
		bad = false;
		if (binarySearch(Din, Id, exp, mar, wag, ind)) {
			cout << "Sorry, but that Id is already in use. Please try again: " << endl;
			cin >> Id;
			while (Id.length() < 5) {
				Id = "0" + Id;
			}
			bad = true;
		}
		if (overflowSearch(Din_over, Id, exp, mar, wag, ind)) {
			cout << "Sorry, but that Id is already in use. Please try again: " << endl;
			cin >> Id;
			while (Id.length() < 5) {
				Id = "0" + Id;
			}
			bad = true;
			Din_over.seekg(0, ios::beg);
		}
		if (Experience < 0 && Experience > 9999999999) {
			Experience = 0;
			bad = true;
		}
		if (Married != "yes" && Married != "no") {
			cout << "Sorry, but your entry for married is not quite right. Please enter yes or no: " << endl;
			cin >> Married;
			bad = true;
		}
		if (to_string(Wage).length() > 12) {
			cout << "Sorry, but your entry for wage was too precise. Try again: " << endl;
			cin >> Wage;
			bad = true;
		}
		if (Industry.length() > 32) {
			cout << "Sorry, but your entry for industry was too long. Try again: " << endl;
			cin >> Industry;
			bad = true;
		}
	}
	
	if ((binarySearch(Din, Id, exp, mar, wag, ind)) && mar == "null")
		overwrite = true;
	else if ((overflowSearch(Din_over, Id, exp, mar, wag, ind)) && mar == "null")
		overwrite = true;

	if (!overwrite) {
		ofstream oout;
		oout.open(overflowDatabase.c_str(), ios::app);
		string Exp = to_string(Experience), Wag = to_string(Wage);
		Id += " ";
		while (Exp.length() < 11) {
			Exp += " ";
		}
		while (Married.length() < 8) {
			Married += " ";
		}
		while (Wag.length() < 13) {
			Wag += " ";
		}
		while (Industry.length() < 32) {
			Industry += " ";
		}
		oout << Id << Exp << Married << Wag << Industry << endl;
		cout << "\nRecord has been added successfully. " << endl;

		NUM_RECORDS_OVERFLOW++;
		if (NUM_RECORDS_OVERFLOW > RECORD_OVERFLOW) {
			mergeFiles();
			oout.close();
			oout.open(overflowDatabase.c_str(), ios::trunc);
		}
		oout.close();
	}
	else {
		long pos;
		ofstream Ov;
		if (overflowSearch(Din_over, Id, exp, mar, wag, ind)) {
			pos = Din_over.tellg();
			Ov.open(overflowDatabase.c_str(), ios::app);
		}
		else {
			pos = Din.tellg();
			Ov.open(currentDatabase.c_str(), ios::app);
		}
		pos -= 71;
		

		string Exp = to_string(Experience), Wag = to_string(Wage);
		Id += " ";
		while (Exp.length() < 11) {
			Exp += " ";
		}
		while (Married.length() < 8) {
			Married += " ";
		}
		while (Wag.length() < 13) {
			Wag += " ";
		}
		while (Industry.length() < 32) {
			Industry += " ";
		}

		Ov << Id << Exp << Married << Wag << Industry;
		cout << "\nRecord has been added successfully. " << endl;

		Ov.close();
	}
	
	Din.close();
	Din_over.close();
}

/* Sets the fields of the requested to-be-deleted record to null */
bool deleteRecord()
{
	int Experience = -1;
	string Married = "UNKNOWN";
	float Wage = -1.0;
	string Industry = "INDUSTRY";
	string Exp, Wag;

	string id;
	long pos;
	ifstream Din;
	ofstream Upd;
	Din.open(currentDatabase.c_str());
	cout << "Please enter the id of the record you wish to delete: " << endl;
	cin >> id;
	while (id.length() < 5) {
		id = "0" + id;
	}
	if (binarySearch(Din, id, Experience, Married, Wage, Industry)) {
		pos = Din.tellg();
		pos = pos - 64 + (32 - Industry.length());
		Upd.open(currentDatabase.c_str(), std::ios::in | std::ios::out);
	}
	else {
		Din.close();
		Din.open(overflowDatabase.c_str());
		if (overflowSearch(Din, id, Experience, Married, Wage, Industry)) {
			pos = Din.tellg();
			pos = pos - 64 + (32 - Industry.length());
			Upd.open(overflowDatabase.c_str(), std::ios::in | std::ios::out);
		}
		else {
			cout << "There is no record with that id." << endl;
			return false;
		}
	}
	Din.close();
	Upd.seekp(pos);

	Exp = "-1";
	while (Exp.length() < 11)
		Exp += " ";
	//Upd.write(Exp.c_str(), 11);
	Upd << Exp;
	Married = "null";
	while (Married.length() < 8)
		Married += " ";
	//Upd.write(Married.c_str(), 8);
	Upd << Married;
	Wag = "-99.9";
	while (Wag.length() < 13)
		Wag += " ";
	//Upd.write(Wag.c_str(), 13);
	Upd << Wag;
	Industry = "null";
	while (Industry.length() < 32)
		Industry += " ";
	//Upd.write(Industry.c_str(), 32);
	Upd << Industry;
	
	Upd.close();

	return true;
}

/* Dynamically gets the record count of each opened file in constant time */
void getRecordCount(fstream &file) 
{
	file.seekg(0, ios::end);
	long pos = file.tellg();
	//cout << "position is " << pos << endl;
	NUM_RECORDS = (pos - RECORD_SIZE) / RECORD_SIZE;
	//cout << "NUM_RECORDS is " << NUM_RECORDS << endl;
}

/* Uses Linux command line to merge two files and sort them */
void mergeFiles()
{
	string command1 = "sort -n " + currentDatabase + " " + overflowDatabase + " > temp.txt";
	string command2 = "mv temp.txt " + currentDatabase;
	system(command1.c_str());
	system(command2.c_str());
	NUM_RECORDS += NUM_RECORDS_OVERFLOW;
	NUM_RECORDS_OVERFLOW = 0;
}