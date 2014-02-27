#include "pTrade.h"

// Returns an exponential daily return rate
double dailyReturn(double input, double daysHeld){ return pow(1+input,1.0/daysHeld)-1; }

// Returns an exponential annualized return rate
double annReturn(double input, double daysHeld){ return pow(1+input,365.0/daysHeld)-1; }

// Converts a gregorian date into a day number since 0000AD
int JCD(date d){
  
  int m = (d.month + 9) % 12;
  int y = d.year - m/10;
  return 365*y + y/4 - y/100 + y/400 + (m*306 + 5)/10 + (d.day - 1 );
}

// Returns the number of days between two dates
int cDays(date d1, date d2){
  return JCD(d2) - JCD(d1);
}

//Parses a date string into a date
date parseDate(string input){
  date ret;
  string mm, dd, yy;
  int i = 0;

  for(i; input[i] != '/'; i++)
    mm += input[i];

  i++;

  for(i; input[i] != '/'; i++)
    dd += input[i];

  i++;

  for(i; i < input.length(); i++)
    yy += input[i];

  ret.month = atoi(mm.c_str());
  ret.day = atoi(dd.c_str());
  ret.year = atoi(yy.c_str());

  if(ret.year < 2000)
    ret.year += 2000;

  return ret;
}

// Parses another date string format into a date
date parseIndexDate(string input){
  date ret;
  string mm, dd, yyyy;
  int i = 0;

  for(i; input[i] != '-'; i++)
    yyyy += input[i];

  i++;

  for(i; input[i] != '-'; i++)
    mm += input[i];

  i++;

  for(i; i < input.length(); i++)
    dd += input[i];

  ret.month = atoi(mm.c_str());
  ret.day = atoi(dd.c_str());
  ret.year = atoi(yyyy.c_str());

  return ret;
}

// Returns a string between two commas and iterates to the next comma
string move(string input, int* i){
  string ret = "";
  int j = *i;

  for(j; input[j] != ',' && j < input.length(); j++)
    ret += input[j];

  *i = j+1;

  return ret;
}

// Parses an index point (index being S&P 500)
indexPoint parseIndex(string input){
  indexPoint temp;
  int i = 0;
 
  temp.d = parseIndexDate(move(input, &i));
  temp.price = atof(move(input, &i).c_str());

  return temp;
}

// Parses a csv trade
pTrade parseTrade(string input){
  pTrade ret;
  string temp = "";
  int i = 0;

  ret.type = move(input, &i);
  ret.sortID = move(input, &i);
  ret.symbol = move(input, &i);
  ret.d = parseDate(move(input, &i));
  ret.numShares = atof(move(input, &i).c_str());

  if(ret.type == "buy" || ret.type == "sell" || ret.type == "donate" 
     || ret.type == "aquisition" || ret.type == "dividend" 
     || ret.type == "Buy" || ret.type == "Sell"){
    ret.dAmount = atof(move(input, &i).c_str());
    ret.price = atof(move(input, &i).c_str());
  }

  if(ret.type == "reverse split"){
    ret.numShares *= -1;
  }

  return ret;
}

// Tests which event occured first
bool dateGreat(pTrade first, pTrade second){
  if(cDays(first.d, second.d) > 0)
    return true;
  else
    return false;
}

// Finds the index of a date in the benchmark vector
int findIndex(vector<indexPoint> input, date d){
  int i = 0;

  while(cDays(input[i].d,d) != 0 && i < input.size()) {
    if(cDays(input[i].d,d) == 0)
      return i;
    i++;
  }

  return i;
}

// Returns a vector of input data`
vector<pTrade> getInput(){
  ifstream infile;
  string temp;
  vector<pTrade> ret;
  vector<indexPoint> benchmark;
  
  ret.resize(0);
  benchmark.resize(0);
  
  infile.open("tradedat.csv");

  while(getline(infile,temp))
    ret.push_back(parseTrade(temp));

  infile.close();

  sort(ret.begin(), ret.end() , dateGreat);

  infile.open("sp.csv");

  while(getline(infile,temp)){
    benchmark.push_back(parseIndex(temp));
  }

  infile.close();


  for(int i = 0; i < ret.size(); i++){
    int j = findIndex(benchmark, ret[i].d);

    if(j == benchmark.size()){
      cout << "index not found for date " << ret[i].d.month << "/" << ret[i].d.day << "/"
	   << ret[i].d.year << "/"<< endl;
    }
    else{
      ret[i].benchPrice = benchmark[j].price;
    }
  }

  return ret;
}
