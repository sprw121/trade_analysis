This program takes two inputs, tradedat.csv and sp.csv, 
respectively the data on trades made and index price
for each day the portfolio was active. To run it in the future,
the additional trades should be added at the bottom of
tradedat.csv and saved in the appropriate csv format. Index prices
available from http://finance.yahoo.com/q/hp?s=VFINX+Historical+Prices should
also be added in sp.csv in the appropriate place, for each day there was
an action on the portfolio.

Any change in trade formatting or index price formatting must
be hard coded in the appropriate parsers in pTrade.cpp.

To compile the program, simply type into a linux or unix command line

make

To run then type

./main 

and the lot and stocksheets should show up in the directory under lotsheet.csv
and stocksheet.csv.

Modifications to the lotsheet should be made in printLotSheet.cpp and to the stocksheet
in position.cpp
