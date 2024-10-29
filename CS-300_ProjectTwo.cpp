//==============================================
// Name: CS-300_ProjectTwo.cpp 
// Author: Jeff Fulfer
// Date: 10-21-2024
//==============================================

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>

using namespace std;

// =========================================================
//                    Define Structures 
// =========================================================
struct Course {
	string classID;
	string Title;
	vector<string> preReqs;
	Course() {
		classID = "";
	}
};

struct Node {
	Course course;
	Node* left;
	Node* right;

	Node() {
		course = Course();
		left = nullptr;
		right = nullptr;
	}

	Node(Course newCourse) :
		Node() {
		course = newCourse;
		left = nullptr;
		right = nullptr;
	}
};

// =========================================================
//                 File Check Functions
// =========================================================

// Function to check for valid prereqs
bool validPrereqs(vector<string> prereq) {
	for (string code : prereq) {
		for (char c : code) {
			if (!isalnum(c)) {
				return false;
			}
		}
	}
	return true;
}

bool checkFormatting(string line) {

	stringstream ss(line);      // Stream we'll use to operate in the line input
	string chunk;               // Temp string to hold chunk we'll add to array
	char del = ',';             // Delimeter used to split string

	// Initialize Vector to hold chunks
	vector<string> parts;

	// Initialize Vector to hold prereqs
	vector<string> prereqs;

	// Loop on line until we reach EOL
	while (getline(ss, chunk, del)) {
		// Append chunk to vector
		parts.push_back(chunk);
	}

	// Append only prereqs to vector
	for (int i = 0; i < parts.size(); ++i) {

		// Check if we are prereq pos AND cell is not empty
		if (i >= 2 && parts.at(i) != "") {
			prereqs.push_back(parts.at(i));
		}
	}

	// Check at minimum the Class Code and Name are entered
	if (parts.size() < 2) {
		return false;
	}

	// Check for empty class code or empty description
	if (parts.at(0) == "" || parts.at(1) == "") {
		return false;
	}

	// Check for non-alphanumeric class code
	for (char c : parts.at(0)) {
		if (!isalnum(c)) {
			return false;
		}
	}

	// Check for valid prereq
	if (prereqs.size() > 0) {
		if (!validPrereqs(prereqs)) {
			return false;
		}
	}

	// Return true if all checks succeeded
	return true;
}

// Main file check function - used to invoke other file check functions
bool fileCheck(string const& file) {

	// Attempt to open file
	try {
		ifstream fileName(file);
		// Throw exception with error message if unable to open
		if (!fileName.is_open()) {
			cout << strerror(errno) << endl;
			throw runtime_error("Unable to open the file " + file);
		}

		// Variable to hold each line of file
		string line;

		// Loop each line through checkFormatting function
		while (getline(fileName, line)) {
			if (!checkFormatting(line)) {
				return false;
			}
		}
	}
	// Display error message and return false
	catch (runtime_error& excpt) {
		cout << excpt.what() << endl;
		return false;
	}

	// Return true if all lines return true
	return true;
}

// =========================================================
//                Insert Node Function
// =========================================================

void insertNode(Node* node, Course& course) {

	// If node classID is larger than classID of course, add to the left
	if (node->course.classID.compare(course.classID) > 0) {
		// if left node is empty, add course here
		if (node->left == nullptr) {
			node->left = new Node(course);
		}
		// Otherwise recurse down left node
		else {
			insertNode(node->left, course);
		}
	}
	else {
		// if node classID is less than, check the right node
		if (node->right == nullptr) {
			node->right = new Node(course);
		}
		else {
			// if right node is populated, recurse down the right node
			insertNode(node->right, course);
		}

	}
}

// =========================================================
//                   Create Course Objects
// =========================================================
void createTreeObjects(string const &file, Node *root) {


	// Verify CSV file is formatted correctly and all rows are valid courses
	if (!fileCheck(file)) {
		
		// Output error message and return if file check failed
		cout << "Unable to open file or file has invalid courses" << endl;
		return;
	}
	else {
		// Open file stream
		ifstream fileName(file);
		if (fileName.is_open()) {

			// Initizalize string to use for each line
			string line;

			// Loop over each line in input file stream
			while (getline(fileName, line)) {

				stringstream ss(line);        // Open string stream using the current line
				string cell;                  // Initialize string to hold each cell
				vector<string> cells;         // Vector to manage each cell in row
				vector<string> prereqs;       // Vector to manage prereqs
				char del = ',';               // Delimeter

				// Append all cells into vector
				while (getline(ss, cell, del)) {

					cells.push_back(cell);
				}

				// Append any prereqs to vector
				if (cells.size() > 2) {
					for (int i = 0; i < cells.size(); ++i) {
						if (i >= 2) {
							prereqs.push_back(cells.at(i));
						}
					}
				}

				// Create course struct
				Course currCourse;
				currCourse.classID = cells.at(0);
				currCourse.Title = cells.at(1);
				currCourse.preReqs = prereqs;

				// Insert course in BST
				if (root->course.classID == "") {

					// Insert course into root node if empty
					root->course = currCourse;
				}
				else {
					// If node is populated, insert node using insertNode function
					insertNode(root, currCourse);
				}
			}
		}
	}
}

// =========================================================
//                    Print Ordered Tree
// =========================================================

void printSortTree(Node* node) {

	// if node is not null
	if (node != nullptr) {
		// recurse to the left
		printSortTree(node->left);

		// Print course info
		cout << node->course.classID << ", " << node->course.Title << endl;

		/*
		for (string req : node->course.preReqs) {
			cout << req << " | ";
		}
		cout << endl;
		*/

		// recurse to the right
		printSortTree(node->right);
	}
}
// =========================================================
//                  Search Course
// =========================================================

void searchTree(Node *root, string classID) {

	// Initialize pointer to use for traversal
	Node* currNode = root;

	// Loop while pointer is not null
	while (currNode != nullptr) {
		// if classID is greater than currNode, traverse to the right
		if (currNode->course.classID < classID) {
			currNode = currNode->right;
		}
		// if classID is less than currNode, traverse to the left
		else if (currNode->course.classID > classID) {
			currNode = currNode->left;
		}
		// Node found
		else {
			cout << currNode->course.classID << ", " << currNode->course.Title << endl;
			cout << "Prerequisites: ";
			for (int i = 0; i < currNode->course.preReqs.size(); ++i) {
				if (i == (currNode->course.preReqs.size() - 1)) {
					cout << currNode->course.preReqs.at(i) << endl;
				}
				else {
					cout << currNode->course.preReqs.at(i) << ", ";
				}
			}
			cout << endl;
			return;
		}
	}

	// Class not found
	cout << classID << " not found." << endl;
}


int main(int argc, char* argv[]) {

	string csvfile;
	string classID;

	switch (argc) {

	// Check if file argument is provided
	case 2:
		csvfile = argv[1];
		break;

	// Assign default file
	default:
		csvfile = "CS_300_ABCU_Advising_Program_Input.csv";
	}

	// Initialize tree root node
	Node root = Node();

	// Initial choice value to enter loop
	int choice = 0;
	while (choice != 9) {

		// Display Menu Options
		cout << "Menu Options:" << endl;
		cout << "  1.) Load Courses" << endl;
		cout << "  2.) Print Courses In Alphanumerical Order" << endl;
		cout << "  3.) Search And Print Course Details" << endl;
		cout << "  9.) Exit The Program" << endl << endl; 
		cout << "Enter Choice: ";
		cin >> choice;

		switch (choice) {

		case 1:
			// Call function to load courses
			createTreeObjects(csvfile, &root);
			break;

		case 2:

			cout << "Printing courses in alphanumerical order.." << endl << endl;

			// Call function to print courses
			printSortTree(&root);
			break;

		case 3:
			cout << "Enter Class Course ID: ";
			cin >> classID;

			// Call Search function
			searchTree(&root, classID);
			break;

		default:
			cout << choice << " is not a valid option." << endl;
		}

	}

	cout << "Exiting the program.. " << endl;

	return 0;
}
