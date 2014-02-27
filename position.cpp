#include "position.h"



// Constructor, takes the first buy order for a position
// and initializes the basis array.
position::position(pTrade input){
  activeBasis.resize(0);
  index = 0;

  sortID = input.sortID;
  symbol = input.symbol;
  
  add(input);
}

// Overloaded constructor, takes an input vector
// of lots and initials a new position.
// Called from exchange and spinoff.
position::position(vector<basis> base, string sort, string sym){
  activeBasis = base;
  index = 0;

  for(int i = 0; i < base.size(); i++){
    base[i].symbol = sym;
  }

  sortID = sort;
  symbol = sym;
}

// Adds a lot to the vector of active lots.
// Takes a buy order input. 
void position::add(pTrade input){
  basis temp;

  temp.symbol = input.symbol;
  temp.dateBought = input.d;
  temp.dateSold.month = 0;
  temp.dateSold.day = 0;
  temp.dateSold.year = 0;
  temp.numShares = input.numShares;
  temp.buyPrice = input.price;
  temp.dollarAmountBuy = input.dAmount;
  temp.benchmark = input.benchPrice;
  temp.sellPrice = 0;
  temp.dollarAmountSell = 0;
  temp.dividend = 0;
  temp.benchRet = -1;
  temp.note = "";

  activeBasis.push_back(temp);
}

// Takes a sell order and sells the appropriate lots a FIFO basis
void position::sell(pTrade input){
  double rShares = input.numShares;
  
  // While there are still shares to sell
  while(rShares > 0){
    if(index >= activeBasis.size()){ // Check if enough shares exist to sell
      if(rShares > .1){
	cout << "Error: no basis for sell order exists: " << input.sortID << ": " << input.symbol << ": " << input.numShares << ": " << rShares << endl;
      }
      return;
    }
    
    // If the last lot contains more more than to be sold,
    // Break it into two lots, the first containing the shares to be sold

    if(activeBasis[index].numShares > rShares + .015){
      basis temp = activeBasis[index];
      temp.dollarAmountBuy *= rShares/temp.numShares;
      temp.dividend *= rShares/temp.numShares;
      temp.numShares = rShares;
      activeBasis.insert(activeBasis.begin()+index, temp);
      activeBasis[index+1].numShares -= rShares;
      activeBasis[index+1].dollarAmountBuy -= temp.dollarAmountBuy;
      activeBasis[index+1].dividend -= temp.dividend;
    }
    
    // Move the index along and retain the information from a sell order

    activeBasis[index].dateSold = input.d;
    activeBasis[index].sellPrice = input.price;
    activeBasis[index].dollarAmountSell = input.dAmount*(activeBasis[index].numShares/input.numShares);
    activeBasis[index].benchRet += input.benchPrice/activeBasis[index].benchmark;

    rShares -= activeBasis[index].numShares;
    index++;
  }
}

// Functions the same as sell only adds a note that indicates
// the lots involved where donated
void position::donate(pTrade input){
  double rShares = input.numShares;
  
  while(rShares > 0){
    if(index >= activeBasis.size()){
      cout << "Error: no basis for sell order exists: " << input.symbol << ": " << input.numShares << endl;
      return;
    }
    
    if(activeBasis[index].numShares > rShares + .015){
      basis temp = activeBasis[index];
      temp.dollarAmountBuy *= rShares/temp.numShares;
      temp.dividend *= rShares/temp.numShares;
      temp.numShares = rShares;
      activeBasis.insert(activeBasis.begin()+index, temp);
      activeBasis[index+1].numShares -= rShares;
      activeBasis[index+1].dollarAmountBuy -= temp.dollarAmountBuy;
      activeBasis[index+1].dividend -= temp.dividend;
    }
    
    activeBasis[index].dateSold = input.d;
    activeBasis[index].sellPrice = input.price;
    activeBasis[index].dollarAmountSell = input.dAmount*(activeBasis[index].numShares/input.numShares);
    activeBasis[index].note = "donation";
    activeBasis[index].benchRet += input.benchPrice/activeBasis[index].benchmark;

    rShares -= activeBasis[index].numShares;
    index++;
  }
}

// Sell all the lots and moves the index to the end
void position::acquistion(pTrade input){
  for(index; index < activeBasis.size(); index++){
    activeBasis[index].dateSold = input.d;
    activeBasis[index].sellPrice = input.price;
    activeBasis[index].dollarAmountSell = input.dAmount*(activeBasis[index].numShares/input.numShares);
    activeBasis[index].benchRet += input.benchPrice/activeBasis[index].benchmark;
    activeBasis[index].note = "acquistion";
  }
}

// Splits all the shares in the lot
// the input contains a numShare value that says how many shares where added
// A positive number indicates we recieved that many shares
// A negative number indicates a reverse split where that many shares 
// were removed
void position::split(pTrade input){
  double ratio = (getActiveShares() + input.numShares)/getActiveShares();

  for(int i = index; i < activeBasis.size(); i++){
    activeBasis[i].numShares *= ratio;
    activeBasis[i].buyPrice /= ratio;
  }

}

// Adds a set of lots to the activeBasis list
void position::exchange(vector<basis> swap){
  for(int i = 0; i < swap.size(); i++)
    activeBasis.push_back(swap[i]);
}

// Returns a new position created in a spinoff
// Currently each spinoff must be hard coded to contain the price
// at spinoff date the the child company.
// The function distributes the cost basis as follows:
// The cost of the parent is the cost at purchase, and the cost
// of the child is the position price at spinoff
// The cost basis are then the appropriate fraction of the
// parent plus child cost at spinoff date.
position* position::spinoff(pTrade input) {
  vector<basis> spinoff = getActiveLots();
  double prevCost = getActiveBasis();
 

  if(input.symbol == "FTR"){
    for(int i = index; i <activeBasis.size(); i++){
      activeBasis[i].dollarAmountBuy *= prevCost/(7.49*input.numShares + prevCost);
      activeBasis[i].buyPrice *= prevCost/(7.49*input.numShares + prevCost);
      activeBasis[i].note = "portion of cost basis distributed to spinoff ";
      activeBasis[i].note += input.symbol;
    }
    for(int i = 0; i < spinoff.size(); i++){
      spinoff[i].dollarAmountBuy *= 1 - prevCost/(7.49*input.numShares + prevCost);
      spinoff[i].numShares *= input.numShares/getActiveShares();  
      spinoff[i].buyPrice *= 1 - prevCost/(7.49*input.numShares + prevCost);
      spinoff[i].note = "cost basis inherited from ";
      spinoff[i].note += input.sortID;
      spinoff[i].symbol = input.symbol;
      spinoff[i].dividend = 0;
    }
  }
  else if(input.symbol == "COV"){
    for(int i = index; i <activeBasis.size(); i++){
      activeBasis[i].dollarAmountBuy *= prevCost/(43.41*input.numShares + prevCost);
      activeBasis[i].buyPrice *= prevCost/(43.41*input.numShares + prevCost);
      activeBasis[i].note = "portion of cost basis distributed to spinoff ";
      activeBasis[i].note += input.symbol;
    }
    for(int i = 0; i < spinoff.size(); i++){
      spinoff[i].dollarAmountBuy *= 1 - prevCost/(43.41*input.numShares + prevCost);
      spinoff[i].numShares *= input.numShares/getActiveShares();
      spinoff[i].buyPrice *= 1 - prevCost/(43.41*input.numShares + prevCost);
      spinoff[i].note = "cost basis inherited from ";
      spinoff[i].note += input.sortID;
      spinoff[i].symbol = input.symbol;
      spinoff[i].dividend = 0;
    }
  }
  else if(input.symbol == "TEL"){
    for(int i = index; i <activeBasis.size(); i++){
      activeBasis[i].dollarAmountBuy *= prevCost/(39.97*input.numShares + prevCost);
      activeBasis[i].buyPrice *= prevCost/(39.97*input.numShares + prevCost);
      activeBasis[i].note = "portion of cost basis distributed to spinoff ";
      activeBasis[i].note += input.symbol;
    }
    for(int i = 0; i < spinoff.size(); i++){
      spinoff[i].dollarAmountBuy *= 1 - prevCost/(39.97*input.numShares + prevCost);
      spinoff[i].numShares *= input.numShares/getActiveShares();
      spinoff[i].buyPrice *= 1 - prevCost/(39.97*input.numShares + prevCost);
      spinoff[i].note = "cost basis inherited from ";
      spinoff[i].note += input.sortID;
      spinoff[i].symbol = input.symbol;
      spinoff[i].dividend = 0;
    }
  }
  else if(input.symbol == "FURX"){
    for(int i = index; i <activeBasis.size(); i++){
      activeBasis[i].dollarAmountBuy *= prevCost/(9.30*input.numShares + prevCost);
      activeBasis[i].buyPrice *= prevCost/(9.30*input.numShares + prevCost);
      activeBasis[i].note = "portion of cost basis distributed to spinoff ";
      activeBasis[i].note += input.symbol;
    }
    for(int i = 0; i < spinoff.size(); i++){
      spinoff[i].dollarAmountBuy *= 1 - prevCost/(9.30*input.numShares + prevCost);
      spinoff[i].numShares *= input.numShares/getActiveShares();
      spinoff[i].buyPrice *= 1 - prevCost/(9.30*input.numShares + prevCost);
      spinoff[i].note = "cost basis inherited from ";
      spinoff[i].note += input.sortID;
      spinoff[i].symbol = input.symbol;
      spinoff[i].dividend = 0;
    } 
  }
  else if(input.symbol == "EXPE"){
    for(int i = index; i <activeBasis.size(); i++){
      activeBasis[i].dollarAmountBuy *= prevCost/(23.7*input.numShares + prevCost);
      activeBasis[i].buyPrice *= prevCost/(23.7*input.numShares + prevCost);
      activeBasis[i].note = "portion of cost basis distributed to spinoff ";
      activeBasis[i].note += input.symbol;
    }
    for(int i = 0; i < spinoff.size(); i++){
      spinoff[i].dollarAmountBuy *= 1 - prevCost/(23.7*input.numShares + prevCost);
      spinoff[i].numShares *= input.numShares/getActiveShares();
      spinoff[i].buyPrice *= 1 - prevCost/(23.7*input.numShares + prevCost);
      spinoff[i].note = "cost basis inherited from ";
      spinoff[i].note += input.sortID;
      spinoff[i].symbol = input.symbol;
      spinoff[i].dividend = 0;
    } 
  }

  return new position(spinoff, input.sortID, input.symbol);
}

// Distributes a dividend around the active lotss,
// by the fraction of shares in the lot.
void position::dividend(pTrade input){
  for(int i = index; i < activeBasis.size(); i++){
    activeBasis[i].dividend += activeBasis[i].numShares*input.dAmount/getActiveShares();
  }
}

// Distributes a dividend over the last group of lots sold
void position::xdividend(pTrade input){
  int i = index - 1;
  double nShares = 0;
  date d = activeBasis[i].dateSold;

  if(cDays(d, input.d) > 60){
    cout << "exdividend failed: more than 60 days: " << input.symbol 
	 << ": " << input.d.month << "/" << input.d.day 
	 << "/" << input.d.year << endl;
  }

  while(i >= 0 && cDays(activeBasis[i].dateSold, d) == 0){
      nShares += activeBasis[i].numShares;
      i--;
  }

  i++;
  
  for(i; i < index; i++){
      activeBasis[i].dividend += activeBasis[i].numShares*input.dAmount/nShares;
  }
}

// Return the amount of shares in active lots
double position::getActiveShares(){
  double counter = 0;

  for(int i = index; i < activeBasis.size(); i++)
    counter += activeBasis[i].numShares;

  return counter;
}

// Returns a vector of lots sold
double position::getInactiveShares(){
  double counter = 0;

  for(int i = 0; i < index; i++)
    counter += activeBasis[i].numShares;

  return counter;
}

// Returns the cost basis of the lots active
double position::getActiveBasis(){
  double temp = 0;

  for(int i = index; i < activeBasis.size(); i++)
    temp += activeBasis[i].dollarAmountBuy;

  return temp;
}

// Returns a csv string with all the relevant information
// of a position that has been closed.
string position::printInactive(){
  string temp  = "";

  temp += symbol;
  temp += ",";
  temp += symbol;
  temp += ",";
  temp += std::to_string(index);
  temp += ",";
  temp += std::to_string(activeBasis[0].dateBought.month);
  temp += "/";
  temp += std::to_string(activeBasis[0].dateBought.day);
  temp += "/";
  temp += std::to_string(activeBasis[0].dateBought.year);
  temp += ",";
  temp += std::to_string(activeBasis[index-1].dateSold.month);
  temp += "/";
  temp += std::to_string(activeBasis[index-1].dateSold.day);
  temp += "/";
  temp += std::to_string(activeBasis[index-1].dateSold.year);
  temp += ",";
  temp += std::to_string(weightedTimeHeld());
  temp += ",";
  temp += std::to_string(totalBasis());
  temp += ",";
  temp += std::to_string(totalSales());
  temp += ",";
  temp += std::to_string(priceReturn()*100);
  temp += "%,";
  temp += std::to_string(dailyReturn(priceReturn(),weightedTimeHeld())*100);
  temp += "%,";
  temp += std::to_string(annReturn(priceReturn(), weightedTimeHeld())*100);
  temp += "%,";
  temp += std::to_string(totalDividends());
  temp += ",";
  temp += std::to_string(totalProceeds());
  temp += ",";
  temp += std::to_string(totalReturn()*100);
  temp += "%,";
  temp += std::to_string(dailyReturn(totalReturn(),weightedTimeHeld())*100);
  temp += "%,";
  temp += std::to_string(annReturn(totalReturn(), weightedTimeHeld())*100);
  temp += "%,";
  temp += std::to_string(benchReturn()*100);
  temp += "%,";
  temp += std::to_string(dailyReturn(benchReturn(),weightedTimeHeld())*100);
  temp += "%,";
  temp += std::to_string(annReturn(benchReturn(), weightedTimeHeld())*100);
  temp += "%,";
  return temp;
}

// Returns the dividends associated with lots sold
double position::totalDividends(){

  double counter = 0;

  for(int i = 0; i < index; i++)
    counter += activeBasis[i].dividend;
  
  return counter;
}

// Returns to total sales proceeds of a position
double position::totalSales(){
  double counter = 0;

  for(int i = 0; i < index; i++)
    counter += activeBasis[i].dollarAmountSell;
  
  return counter;
}

// Returns the cost basis of lots sold
double position::totalBasis(){
    double counter = 0;

  for(int i = 0; i < index; i++)
    counter += activeBasis[i].dollarAmountBuy;
  
  return counter;
}

// Returns an weighted average of time held, weighted by the number
// of shares per lot
double position::weightedTimeHeld(){
  double weightTime = 0;

  for(int i = 0; i < index; i++)
    weightTime += cDays(activeBasis[i].dateBought, activeBasis[i].dateSold)*activeBasis[i].numShares;

  return weightTime/getInactiveShares();
}

// Returns a csv string with relevant information of currently
// active lots of a position.
string position::printActive(){
  string temp  = "";

  temp += symbol;
  temp += ",";
  temp += symbol;
  temp += ",";
  temp += std::to_string(numOpenLots());
  temp += ",";
  temp += std::to_string(getActiveShares());
  temp += ",";
  temp += std::to_string(activeBasis[index].dateBought.month);
  temp += "/";
  temp += std::to_string(activeBasis[index].dateBought.day);
  temp += "/";
  temp += std::to_string(activeBasis[index].dateBought.year);
  temp += ",";
  temp += std::to_string(getActiveBasis());
  temp += ",";
  temp += std::to_string(activeDividends());
  temp += ",";

  return temp;

}

// Calculate a weighted average of how much the benchmark returned over 
// the same trades made in a position.
// Weighted by number of shares per lot.
double position::benchReturn(){
  double counter = 0;

  for(int i = 0; i < activeBasis.size(); i++)
    counter += activeBasis[i].benchRet*activeBasis[i].numShares;

  return counter/getInactiveShares();
}

// Return dividend associated with lots currently active
double position::activeDividends(){
  double temp;
  
  for(int i = index; i < activeBasis.size(); i++)
    temp += activeBasis[i].dividend;

  return temp;
}

// Returns a vector of lots currently active
vector<basis> position::getActiveLots(){
  vector<basis> temp;

  temp.resize(0);

  for(int i = index; i < activeBasis.size(); i++)
    temp.push_back(activeBasis[i]);

  return temp;
}

// Returns a vector of lots currently inactive

vector<basis> position::getClosedLots(){
  vector<basis> temp;

  temp.resize(0);

  for(int i = 0; i < index; i++)
    temp.push_back(activeBasis[i]);

  return temp;
}

