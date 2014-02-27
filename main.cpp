#include "portfolio.h"
#include "printLotSheet.cpp"
#include "printStockSheet.cpp"

int main(){
  portfolio p(getInput());
  
  printLotSheet(&p);
  printStockSheet(&p);
}
