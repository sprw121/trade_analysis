#include "pTrade.h"

/* Class that containes a position
   Saves lots in a vector called activeBasis
   Along with an index that stores where in the 
   vector the first active lot is.
   Lots before input have been sold aready,
   lots after have not.

   Contains several member functions that return information
   about the position.
*/


// Basis holds all the information on a lot
struct basis {
  string symbol;
  date dateBought;
  date dateSold;
  double numShares;
  double buyPrice;
  double dollarAmountBuy;
  double benchmark;
  double sellPrice;
  double dollarAmountSell;
  double dividend;
  double benchRet;
  string note;
};

class position{
 private:
  string sortID;
  string symbol;
  int index;
  vector<basis> activeBasis;
  
 public:
  position(pTrade input);
  position(vector<basis> base, string sort, string sym);
  void add(pTrade input);
  void sell(pTrade input);
  void donate(pTrade input);
  void acquistion(pTrade input);
  void split(pTrade input);
  void exchange(vector<basis> swap);
  position* spinoff(pTrade input);
  void dividend(pTrade input);
  void xdividend(pTrade input);

  double getActiveShares();
  double getInactiveShares();
  double getActiveBasis();
  string getSymbol(){ return symbol; }
  string getSortID(){ return sortID; }
  string printInactive();
  string printActive();
  vector<basis> getActiveLots();
  vector<basis> getClosedLots();
  double weightedTimeHeld();
  double totalSales();
  double totalDividends();
  double totalProceeds() { return totalSales() + totalDividends(); }
  double totalBasis();
  double priceReturn() { return (totalSales()-totalBasis())/totalBasis();}
  double totalReturn(){  return (totalProceeds()-totalBasis())/totalBasis() ; }
  double benchReturn();
  double activeDividends();
  int numOpenLots() { return activeBasis.size()-index; }
  int numClosedLots() { return index; }
};
