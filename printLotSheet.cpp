// Determines whether a lot should be before or after another
// Sorts by date sold - ascending
// then by ticker - ascending
// then by date bought - ascending

bool baseGreat(basis input1, basis input2){
  if(cDays(input1.dateSold, input2.dateSold) < 0)
    return false;
  else if(cDays(input1.dateSold, input2.dateSold) > 0)
    return true;
  else{
    if(input1.symbol > input2.symbol)
      return false;
    else if( input1.symbol < input2.symbol)
      return true;
    else{
      if(cDays(input1.dateBought, input2.dateBought) < 0)
        return false;
      else
        return true;
    }
  }
}

// Returns a csv string with the revelant information for a lot
string printClosedBasis(basis input){
  string temp;
  double daysHeld = double(cDays(input.dateBought, input.dateSold));
  double pRet = input.dollarAmountSell/input.dollarAmountBuy - 1;
  double tRet = (input.dollarAmountSell+input.dividend-input.dollarAmountBuy)/input.dollarAmountBuy;

  temp += input.symbol;
  temp +=  ",";
  temp += input.symbol;
  temp += ",";
  temp += to_string(input.numShares);
  temp += ",";
  temp += std::to_string(input.dateBought.month);
  temp += "/";
  temp += std::to_string(input.dateBought.day);
  temp += "/";
  temp += std::to_string(input.dateBought.year);
  temp += ",";
  temp += std::to_string(input.dollarAmountBuy);
  temp += ",";
  temp += std::to_string(input.buyPrice);
  temp += ",";
  temp += std::to_string(input.dateSold.month);
  temp += "/";
  temp += std::to_string(input.dateSold.day);
  temp += "/";
  temp += std::to_string(input.dateSold.year);
  temp += ",";
  temp += std::to_string(input.dollarAmountSell);
  temp += ",";
  temp += std::to_string(input.sellPrice);
  temp += ",";
  temp += std::to_string(daysHeld);
  temp += ",";
  temp += std::to_string(pRet*100);
  temp += "%,";
  temp += std::to_string(dailyReturn(pRet,daysHeld)*100);
  temp += "%,";
  temp += std::to_string(annReturn(pRet,daysHeld)*100);
  temp += "%,";
  temp += std::to_string(input.dividend);
  temp += ",";
  temp += std::to_string(input.dollarAmountSell + input.dividend);
  temp += ",";
  temp += std::to_string(tRet*100);
  temp += "%,";
  temp += std::to_string(dailyReturn(tRet,daysHeld)*100);
  temp += "%,";
  temp += std::to_string(annReturn(tRet,daysHeld)*100);
  temp += "%,";
  temp += std::to_string(input.benchRet*100);
  temp += "%,";
  temp += std::to_string(dailyReturn(input.benchRet,daysHeld)*100);
  temp += "%,";
  temp += std::to_string(annReturn(input.benchRet,daysHeld)*100);
  temp += "%,";
  temp += input.note;
  
  return temp;
}

// Returns a csv string with the revelant information for a lot
string printActiveBasis(basis input){
  string temp;

  temp += input.symbol;
  temp +=  ",";
  temp += input.symbol;
  temp += ",";
  temp += to_string(input.numShares);
  temp += ",";
  temp += std::to_string(input.dateBought.month);
  temp += "/";
  temp += std::to_string(input.dateBought.day);
  temp += "/";
  temp += std::to_string(input.dateBought.year);
  temp += ",";
  temp += std::to_string(input.dollarAmountBuy);
  temp += ",";
  temp += std::to_string(input.buyPrice);
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += std::to_string(input.dividend);
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += ",";
  temp += input.note;

  return temp;
}

// Function to format and print the lotsheet
void printLotSheet(portfolio* p){
  ofstream outfile;

  outfile.open("lotsheet.csv", ios::trunc);
 
  outfile << "Company," << "Ticker," << "Number of shares," 
	  << "Purchase Date," << "Cost Basis," << "Purchase cost/share," 
	  << "Sale Date," << "Sale Proceeds," << "Sale Price," 
	  << "Holding time," << "Price return," << "Price Return per day," 
	  << "Annualized Price Returns," << "Dividend," << "Total Proceeds,"
	  << "Total Return," << "Total Return per day," 
	  << "Annualized Total Return," << "S&P 500 Total return,"
	  << "S&P 500 Total Return per day," << "Annualized S&P 500 Return,"
	  << "Note," << endl;

  vector<basis> processList = (*p).getClosedBases();
  vector<basis> activeList = (*p).getActiveBases();

  sort(processList.begin(), processList.end(), baseGreat);
  sort(activeList.begin(), activeList.end(), baseGreat);


  for(int i = 0; i < processList.size(); i++){
    outfile << printClosedBasis(processList[i]) << endl;
  }
  
  outfile << endl << "Lots currently active:" << endl << endl;
  
  for(int i = 0; i < activeList.size(); i++){
    outfile << printActiveBasis(activeList[i]) << endl;
  }

  outfile.close();
}