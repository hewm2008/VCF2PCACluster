#!/usr/bin/perl -w
use strict;
#explanation:this program is edited to 
#edit by HeWeiMing;   Thu Jan 14 22:26:57 CST 2010
#Version 1.0    hewm@genomics.org.cn 

die  "Version 1.0\t2010-01-14;\nUsage: $0 <Input1><Input2>\n" unless (@ARGV ==3);

#############Befor  Start  , open the files ####################

open IA,"$ARGV[0]"  || die "input file can't open $!" ;
open IB,"$ARGV[1]"  || die "$!";
open OA,">$ARGV[2]" || die "output file can't open $!" ;

################ Do what you want to do #######################
my %hash=();
<IA>;
	while(<IA>) 
	{ 
		chomp ; 
		my @inf=split ;
		$hash{$inf[0]}=$inf[3];  # K=XX  modify inf[XX]
		
	}
close IA;

    while(<IB>)
    {
         chomp ;
         my @inf=split ;
         if( exists  $hash{$inf[0]})
         {
			 $inf[2]=$hash{$inf[0]};
         }
		 $_=join("\t",@inf);
         print OA $_ ,"\n";
    }
close IB ;
close OA ;

######################swimming in the sky and flying in the sea ############################
