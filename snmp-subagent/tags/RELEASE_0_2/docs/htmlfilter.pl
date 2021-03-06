#!/usr/bin/perl
#
# Filter to make documentation look like website
#

my @files = @ARGV;

foreach my $file (@files) {
	system("tidy -c -m $file 2>/dev/null");
}

foreach my $file (@files) {
    local $/ = undef;
    open(IN,"<$file");
    my $contents = <IN>;
    close(IN);

    my $css = '<!--#include virtual="/openhpi.css" -->';
    my $sidebar = '<!--#include virtual="/sidebar.html" -->';
    my $end = '</td></tr></table>';
    # we want to do only one of these
    if($contents =~ s{(</style>\s*)($css)*}{$1$css\n}is) {
        # tidy output
    } elsif ($contents =~ s{($css\s*)*</head}{$css\n</head}is) {
        # not tidy output
    }
    
    $contents =~ s{(<body.*?>\s*).*?(<div)}{$1<table>\n<tr>\n$sidebar\n<td valign="top">\n$2}is;
    $contents =~ s{(</td></tr></table>\s*)*(</body>)}{</td></tr></table></body>}is;

    open(OUT,">$file");
    print OUT $contents;
    close(OUT);
}
