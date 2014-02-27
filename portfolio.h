#include "position.h"

// Class portfolio acts as a container for the positions
// throughout the life of the portfolio.

// It stores the positions currently active
// along with inactive (positions closed)
// for output at the end of the program.

class portfolio{
 private:
  vector<position*> active;
  vector<position*> inactive;
 
 public:

  portfolio ();
  portfolio (vector<pTrade> input);
  ~portfolio ();
  void kill();
  void action(pTrade input);
  void buy(pTrade input);
  void sell(pTrade input);
  void donate(pTrade input);
  void acquistion(pTrade input);
  void spinoff(pTrade input);
  void split(pTrade input);
  void exchange(pTrade input);
  void dividend(pTrade input);

  int findPosition(string input);
  int findInactivePosition(string input);
  vector<basis> getClosedBases();
  vector<basis> getActiveBases();
  vector<position*> getInactiveStocks() { return inactive; }
  vector<position*> getOpenStocks() { return active; }
};
