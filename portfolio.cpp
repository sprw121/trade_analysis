#include "portfolio.h"


// Function to process which action to take with current trade
void portfolio::action(pTrade input){
  //  cout << "Enter action" << endl;
  if(input.sortID == "CD")
    return;

  if(input.type == "buy" || input. type == "Buy")
    buy(input);
  else if (input.type == "sell" || input.type == "Sell")
    sell(input);
  else if (input.type == "donate" || input.type == "Donate")
    donate(input);
  else if (input.type == "aquisition" || input.type == "Aquisition")
    acquistion(input);
  else if(input.type == "spinoff" || input.type == "Spinoff")
    spinoff(input);
  else if(input.type == "reverse split" || input.type == "split")
    split(input);
  else if(input.type == "exchange")
    exchange(input);
  else if(input.type == "dividend")
    dividend(input);

  //  cout << "Exit action" << endl << endl;
}


// Function to process a buy order
void portfolio::buy(pTrade input){
  //  cout << "Enter buy: " << input.sortID << ":  " << input.symbol << endl;

  // Find whether the desired position exists, and if so it's index
  int index = findPosition(input.symbol);

  if(index != -1)   // If position exists add buy order as a basis
    (*active[index]).add(input);
  else{             // Else add a new position
    position* temp = new position(input);  
    active.push_back(temp);
  }

  //  cout << "Exit buy" << endl;
}


// Routine to process a sell order
void portfolio::sell(pTrade input){
  //  cout << "Enter sell" << ":  " << input.sortID << ":  " << input.symbol<< endl;


  // Find the index of the desired position exists
  int index = findPosition(input.symbol);

  if(index == -1)    // Error message if the desired position does not exist
    cout << "Error: failed to find position: " << input.symbol << endl;
  else{            // Otherwise process the sell order 
    (*active[index]).sell(input);

    if((*active[index]).getActiveShares() == 0){
      inactive.push_back(active[index]);    // If the position was closed move to inactive
      active.erase(active.begin() + index);
    }
  }

  //  cout << "Exit sell" << endl;
}


// Routine to process a donation
void portfolio::donate(pTrade input){
  //  cout << "Enter donate" << endl;

  // Find the index of the desired position exists
  int index = findPosition(input.symbol);

  if(index == -1)    // Error message if the desired position does not exist
    cout << "Error: failed to find position: " << input.symbol << endl;
  else{            // Otherwise process the donation
    (*active[index]).donate(input);
    if((*active[index]).getActiveShares() == 0){
      inactive.push_back(active[index]);    // If the position was closed move to inactive
      active.erase(active.begin() + index);
    }
  }

  //  cout << "Exit donate" << endl;
}

void portfolio::acquistion(pTrade input){
  //  cout << "Enter acquistion" << endl;

  // Find the index of the desired position exists
  int index = findPosition(input.symbol);

  if(index == -1)    // Error message if the desired position does not exist
    cout << "Error: failed to find position: " << input.symbol << endl;
  else{            // Otherwise process the acquistion and move position to inactive
    (*active[index]).acquistion(input);
    inactive.push_back(active[index]);    
    active.erase(active.begin() + index);
  }

  //  cout << "Exit acquistion" << endl;
}

void portfolio::split(pTrade input){
  //  cout << "Enter split" << endl;

  // Find the index of the desired position exists
  int index = findPosition(input.symbol);

  if(index == -1)    // Error message if the desired position does not exist
    cout << "Error: failed to find position: " << input.symbol << endl;
  else
    (*active[index]).split(input);

  // cout << "Exit split" << endl;
}

void portfolio::exchange(pTrade input){
  //  cout << "Enter exchange" << endl;

  // Find the index of the desired positions, and if exists
  int index1, index2;

  index2 = findPosition(input.symbol);

  for(int i = 0; i < active.size(); i++){
    if((*active[i]).getSortID() == input.sortID && i != index2)
      index1 = i;
  }

  // Get the lots of the position to be exchanged;
  vector<basis> swap = (*active[index1]).getActiveLots();

  // Modify the price, numeShares, and symbol as appropriate
  for(int i = 0; i < swap.size(); i++){
    swap[i].numShares *= input.numShares/(*active[index1]).getActiveShares();
    swap[i].buyPrice /= input.numShares/(*active[index1]).getActiveShares();
    swap[i].note = "exchanged; originally ";
    swap[i].note += (*active[index1]).getSymbol();
    swap[i].symbol = input.symbol;
  }

  if(index2 == -1){ // If no positions exists, construct a new one
    position* temp = new position(swap, input.sortID, input.symbol);
    active.push_back(temp);
  } 
  else //Else add the shares from the old to the shares of the new
    (*active[index2]).exchange(swap);

  // Delete the old position
  delete active[index1];

  active.erase(active.begin()+index1);

  // cout << "Exit exchange" << endl;
}

void portfolio::spinoff(pTrade input){
  //  cout << "Enter spinoff";

  // Find the index of the desired position exists
  int index = findPosition(input.sortID);

  if(index == -1)
    cout << "Error: failed to find position: " << input.symbol << endl;
  else
    active.push_back((*active[index]).spinoff(input));

  //  cout << "Exit spinoff";
}

void portfolio::dividend(pTrade input){
  int index = findPosition(input.symbol);
  
  //Find the position in the portfolio, 
  //then adds the dividend to that positions;

  if(index == -1){
    index = findInactivePosition(input.symbol);

    (*inactive[index]).xdividend(input);
  }
  else
    (*active[index]).dividend(input);
}


// Returns the index of the active position
int portfolio::findPosition(string input){
  for(int i = 0; i < active.size(); i ++){
    if((*active[i]).getSymbol() == input)
      return i;         
  }

  return -1; // return -1 if failed to find an active position
}

// Return the index of an inactive position
int portfolio::findInactivePosition(string input){
  for(int i = 0; i < inactive.size(); i++){
    if((*inactive[i]).getSymbol() == input)
      return i;         
  }

  return -1; // return -1 if failed to find an active position
}

vector<basis> portfolio::getClosedBases(){
  // Return a vector of all the lots sold

  vector<basis> temp;
  temp.resize(0);

  for(int i = 0; i < active.size(); i ++){
    vector<basis> temp2 = (*active[i]).getClosedLots();
    for(int j = 0; j < temp2.size(); j++)
      temp.push_back(temp2[j]);
  }

  for(int i = 0; i < inactive.size(); i ++){
    vector<basis> temp2 = (*inactive[i]).getClosedLots();
    for(int j = 0; j < temp2.size(); j++)
      temp.push_back(temp2[j]);
  }

  return temp;
}

vector<basis> portfolio::getActiveBases(){
  // Returns a vector of all the lots not yet sold;

  vector<basis> temp;
  temp.resize(0);

  for(int i = 0; i < active.size(); i++){
    vector<basis> temp2 = (*active[i]).getActiveLots();
    for(int j = 0; j < temp2.size(); j++)
      temp.push_back(temp2[j]);
  }
  
  return temp;
}

portfolio::portfolio(){
  active.resize(0);
  inactive.resize(0);
}

portfolio::portfolio(vector<pTrade> input){
  active.resize(0);
  inactive.resize(0);

  for(int i = 0; i < input.size(); i++)
    action(input[i]);
}

portfolio::~portfolio(){
  for(int i = 0; i < active.size(); i ++)
    delete active[i];

  for(int i = 0; i < inactive.size(); i++)
    delete inactive[i];
}
