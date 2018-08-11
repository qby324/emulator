#! /usr/bin/perl

# Sat Dec 29 15:15:15 JST 2007
# Takahiro Shinozaki

# randomize lines

require "getopts.pl";
&Getopts('s:');

if ($opt_s) {
    srand($opt_s);
}

$i = 0;
while (<>) {
    $line[$i] = $_;
    $idx[$i] = $i;
    $rnd[$i] = rand();
    $i++;
}

@idx = sort {$rnd[$a] <=> $rnd[$b]} @idx;
foreach $i (@idx) {
    print $line[$i];
}
