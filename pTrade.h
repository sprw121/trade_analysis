#include <string>
#include <vector>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

struct date{
  int year;
  int month;
  int day;
};

struct pTrade {
  string type;
  string sortID;
  string symbol;
  date d;
  double numShares;
  double dAmount;
  double price;
  double benchPrice;
};

struct indexPoint {
  date d;
  double price;
};

// Returns an exponential daily return rate
double dailyReturn(double input, double daysHeld);

// Returns an exponential annualized return rate
double annReturn(double input, double daysHeld);

// Converts a gregorian date into a day number since 0000AD
int JCD(date d);

// Returns the number of days between two dates
int cDays(date d1, date d2);

//Parses a date string into a date
date parseDate(string input);

// Parses another date string format into a date
date parseIndexDate(string input);

// Returns a string between two commas and iterates to the next comma
string move(string input, int* i);

// Parses an index point (index being S&P 500)
indexPoint parseIndex(string input);

// Parses a csv trade
pTrade parseTrade(string input);

// Tests which event occured first
bool dateGreat(pTrade first, pTrade second);

// Finds the index of a date in the benchmark vector
int findIndex(vector<indexPoint> input, date d);

// Returns a vector of input data`
vector<pTrade> getInput();
