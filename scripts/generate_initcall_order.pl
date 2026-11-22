#!/usr/bin/env perl
# SPDX-License-Identifier: GPL-2.0
#
# Generate a linker script with initcalls in object and source order.
#
# Copyright (C) 2019 Google LLC

use strict;
use warnings;

my $nm = $ENV{'NM'} || 'llvm-nm';
my @objects;
my %initcalls;
my $level_pattern = qr/early|rootfs|security|con|[0-7]s?/;

open(my $symbols, '-|', $nm, '--print-file-name', '--just-symbol-name',
	'--defined-only', @ARGV) or die "$0: cannot run $nm: $!\n";

while (my $line = <$symbols>) {
	chomp($line);
	my ($object, $counter, $lineno, $function, $level) = $line =~
		/^(.*):\s+__initcall_(\d+)_(\d+)_(.+)($level_pattern)$/;
	next unless defined($level);

	push(@objects, $object) unless exists($initcalls{$object});
	die "$0: repeated initcall counter $counter in $object\n"
		if exists($initcalls{$object}->{$counter});
	my $name = join('_', $counter, $lineno, $function);
	$initcalls{$object}->{$counter} = [$level, $name];
}

close($symbols) or die "$0: $nm failed\n";
die "$0: no initcalls found\n" unless @objects;

my %sections;
foreach my $object (@objects) {
	my $calls = $initcalls{$object};
	foreach my $counter (sort { $a <=> $b } keys(%$calls)) {
		my ($level, $name) = @{$calls->{$counter}};
		my $section = ($level eq 'con' || $level eq 'security') ?
			".${level}_initcall.init" : ".initcall$level.init";
		push(@{$sections{$section}}, $name);
	}
}

print "SECTIONS {\n";
foreach my $section (sort(keys(%sections))) {
	print "\t$section : {\n";
	foreach my $name (@{$sections{$section}}) {
		print "\t\t*($section..$name) ;\n";
	}
	print "\t}\n";
}
print "}\n";
