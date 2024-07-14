
#function $betta1 0xa32cc
li r3, 0
li r3, 1
blr

#function $betta2 after $betta1
li r3, 2
li r3, 3
blr

#function $betta3 after $betta2
li r3, 4
li r3, 5
blr

#function $betta4 after $betta2
li r3, 6
li r3, 7
blr
