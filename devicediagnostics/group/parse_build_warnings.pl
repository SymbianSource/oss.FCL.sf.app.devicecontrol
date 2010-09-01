#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#
######################################################################
# ****************************************Script info : See footer.****************************************
######################################################################

use strict;
use Getopt::Long;

my($log_file,$warning_file,$help,$line,%warnings,$comp_path,%type_by_comp);

GetOptions (
            'log_file=s' => \$log_file,
			'warning_file=s' => \$warning_file,
            'help' => \$help
            );

usage() if($help);

if(!$log_file){
    print "ERROR : No log file parse! Exiting...\n";
    usage();
} #if

$warning_file=$log_file.".warnings.csv" if(!$warning_file);

open(IN,$log_file) || die "Cannot open $log_file\n";
while($line=<IN>){
	my $key="";
	my $val="";
	chomp($line);
	if($line =~ /^Chdir\s+(.*)/i){
		$comp_path=$1;
		$comp_path=~ s/\\group$//i;
		#$type_by_comp{$comp_path};
	}#if
	if($line =~ /\bwarning\b/ig){
		if($line =~ /\bwarning\s+(\w+\d+)\:/i || $line =~ /\bwarning\:\s+(\#\d+\-\w+)\:/i || $line =~ /\bwarning\:\s+(\w+\d+\w+)\:/i){
			$key=$1;
			if(exists $warnings{$key}){
				$warnings{$key}++;
			}
			else{
				$warnings{$key}=1;
			}#if
		# }
		# else{
			# push @other_warnings,($val);
		}#if
		if ($key ne ""){
			if(exists $type_by_comp{$comp_path}{$key}){
				$type_by_comp{$comp_path}{$key}++;
			}
			else{
				$type_by_comp{$comp_path}{$key}=1;
			}#if
		}#if
	}#if

}#while
close(IN);

open(OUT,">".$warning_file) || die "Cannot open $warning_file\n";

print OUT "\nCOUNTING WARNINGS BY TYPE\n\n";
print OUT "Warning,Count\n";
for my $w ( sort keys %warnings ) {
	print OUT "$w,$warnings{$w}\n";
}#for
print OUT "\n";

print OUT "\nSHOWING SPECIFIC WARNINGS BY COMPONENT AND TYPE\n\n";
print OUT "Component,Total warnings by component,Warnings\n";

for my $co ( sort keys %type_by_comp ) {
    print OUT "\n$co,";
    my @ctvals=values %{ $type_by_comp{$co} };
    my $ctsum=0;
    foreach my $cval(@ctvals){
    	$ctsum+=$cval;
    }#foreach
    print OUT "$ctsum,";
    for my $ty ( sort keys %{ $type_by_comp{$co} } ) {
         print OUT " $ty=$type_by_comp{$co}{$ty} ";
	}#for
}#for
print OUT "\n\n";

close(OUT);

#--------------------------------------------------------------#

sub usage{
	while(<DATA>){
		print;
	} #while
	exit;
} #sub

__DATA__

#--------------------------------------------------------------#
#-------------------------- USAGE -----------------------------#
#--------------------------------------------------------------#
 Script name:	parse_build_warnings.pl

 Copyright (c) 2007-2008 Nokia. All rights reserved.

 Purpose:

	To parse warning messages from s60 platform build log.

 Revision History:

	v0.1.0 19.09.2007 Qvist Heikki 	Initial version.
	v0.1.1 20.09.2007 Qvist Heikki 	Count warnings.
	v0.1.2 10.10.2007 Qvist Heikki 	Count warnings per a component.
	v0.2.0 10.01.2008 Qvist Heikki 	Clean useless output. Fix counters (0 -> 1).
	v0.3.0 10.01.2008 Qvist Heikki 	Output to CSV-file for Excel reading.
    
    Commandline options:
    
        -h show this info.
        -l log file to parse out.
        -w output file contain warnings.
        	If not given, <logfile>.warnings.txt is generated.
#--------------------------------------------------------------#
