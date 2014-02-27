// Function to format and print the stocksheet
void printStockSheet(portfolio* p){
  ofstream outfile;

  outfile.open("stocksheet.csv",  ios::trunc);

  outfile << "Company," << "Ticker," << "Number of lots," 
	  << "Earliest purchase," << "Latest Sale," 
	  << "Average holding time," << "Total basis," 
	  << "Total sales proceeds," << "Price return,"
	  << "Average daily price return," << "Annualized price return,"
	  << "Total dividends," << "Total proceeds," << "Total return," 
	  << "Average daily total return," << "Annualized total return,"
	  << "S&P 500 Total return," << "Average daily S&P 500 Total return,"
	  << "Annualized S&P 500 Total return,"
	  << endl;

  vector<position*> closed;
  vector<position*> open;

  closed = (*p).getInactiveStocks();
  open = (*p).getOpenStocks();

  for(int i = 0; i < closed.size(); i++){
    outfile << (*closed[i]).printInactive() << endl;
  }

  outfile << endl << "Returns on lots sold in positions currently open:" 
	  << endl << endl;

  outfile << "Company," << "Ticker," << "Number of lots," 
	  << "Earliest purchase," << "Latest Sale," 
	  << "Average holding time," << "Total basis," 
	  << "Total sales proceeds," << "Price return,"
	  << "Average daily price return," << "Annualized price return,"
	  << "Total dividends," << "Total proceeds," << "Total return," 
	  << "Average daily total return," << "Annualized total return,"
	  << "S&P 500 Total return," << "Average daily S&P 500 Total return,"
	  << "Annualized S&P 500 Total return,"
	  << endl;

  for(int i = 0; i < open.size(); i++){
    if((*open[i]).numClosedLots() != 0){
      outfile << (*open[i]).printInactive() << endl;
    }
  }

  outfile << endl << "Summary of positions currently open:" 
	  << endl << endl;

  outfile << "Company," << "Ticker," << "Number of lots open," 
	  << "Number of shares," << "Earliest purchase," << "Total basis," 
	  << "Total dividends," << endl;

  for(int i = 0; i < open.size(); i++)
    outfile << (*open[i]).printActive() << endl;

  outfile.close();
}