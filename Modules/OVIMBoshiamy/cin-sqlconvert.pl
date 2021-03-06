#!/usr/bin/perl -w
use strict;

die "Please cp UNIX/xcin2.5/liu5.cin from LIU57 CDROM to here\n"
    unless -f $ARGV[0];

binmode(STDOUT, ":utf8");

# Official liu5.cin is in big5
for my $fn (@ARGV) {
    open HNDL, "<:encoding(big5)", $fn;

    my @a=($fn=~/(\w+)\..+/);
    my $tblname=lc $a[0];
    print "insert into tablelist values ('$tblname');\n";
    print "create table $tblname (key, value, ord);\n";
    printf "create index %s_index_key on %s (key);\n", $tblname, $tblname;
    printf "create index %s_index_value on %s (value);\n", $tblname, $tblname;
    print "begin;\n";
    while(<HNDL>) {
        chomp;
        next if (/^#/);
        if (/%chardef/) {
            parse_chardef(*HNDL, $tblname);
        }
        elsif (/%keyname/) {
            parse_keyname(*HNDL, $tblname);
        }
        else {
            parse_property($tblname);
        }
    }
    print "commit;\n";
}

sub parse_property {
    my $tblname=shift;
    my @a=split;
    return if (!scalar(@a));
    if ($a[0] =~ /^%.+/) { $a[0]=substr($a[0], 1, length($a[0])-1);  }
    $a[0] =~ s/\'/\'\'/g;
    if ($a[1]) { $a[1] =~ s/\'/\'\'/g; } else { $a[1]="" };
    printf "insert into %s values ('_property_%s\', '%s\', -1);\n", $tblname, lc $a[0], $a[1];
}

sub parse_keyname {
    my $hndl=shift;
    my $t=shift;
    while(<$hndl>) {
        last if (/%keyname/);
        my @a=split;
        $a[0] =~ s/\'/\'\'/g;
        $a[1] =~ s/\'/\'\'/g;
        printf "insert into %s values ('_key_%s\', '%s', -1);\n", $t, lc $a[0], $a[1];
    }
}

sub parse_chardef {
    my $hndl=shift;
    my $t=shift;
    my ($lastkey, $lastorder)=("", 0);
    while (<$hndl>) {
        chomp;
	next unless $_;
        last if /%chardef/;
        my @a=split;
        if ($a[0] eq $lastkey) {
            $lastorder++;
        }
        else
        {
            $lastorder=0;
            $lastkey=$a[0];
        }
        $a[0] =~ s/\'/\'\'/g;
        $a[1] =~ s/\'/\'\'/g;
        printf "insert into %s values('%s', '%s', %s);\n", $t, lc sprintf("%s", $a[0]), $a[1], $lastorder;
    }
}
