#!/usr/local/bin/perl -Tw

use strict;
use CGI;

&ReadParse(%in);

$val1 = $in{"val1"};
$val2 = $in{"val2"};
$res = add($val1 , $val2);

sub add
{
    ($x,$y) = @_;
    $res = $x + $y ;
    return $res ;
}

print "Content-type:text/html\r\n\r\n";
print '<html>';
print '<body>';
print '<h1>Result: $val1 + $val2 = $res</h1>';
print '</body>';
print '</html>';

1;
