#use: >eustagger_liteunknown proba.txt |extractUnknown.pl
    
my $G=0;
my $forma=0;
while ($l=<STDIN>)
{
    chomp $l;
    if ($l =~ /^\/\</)
    {
	$forma=1;
    }
    elsif ($l =~ /^\%G$/ && $forma)
    {
	$G=1;
	$forma=0;
    }
    elsif ($G && $l =~ /\(\(lema/)
    {
	$l =~ /\(Sarrera ([^\-]+)\-\-/;
	print $1."\n";
	$G=0;
    }
}
