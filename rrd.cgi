#!/usr/bin/perl
# Copyright (c) 2006, J. A. Zanardo Jr. <zanardo@gmail.com>
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE AUTHOR AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

use strict;
use warnings;

use CGI qw(:standard);
use CGI::Carp qw(fatalsToBrowser);
use RRDs;

my $RRDROOT = '/opt/rrd';
my $VERSION = '0.2';

chdir($RRDROOT) or die $!;

$| = 1; # no output buffering

my $re = '';
if(defined param('re') and param('re') ne '') { $re = param('re') }
my $restr = $re; $restr =~ s/([^A-Za-z0-9])/sprintf("%%%02X", ord($1))/seg;

my $offset = 86400;	  # 1 day
if(defined param('offset') and param('offset') =~ /^[0-9]{1,25}$/) {
	$offset = param('offset') }

my $graph = '';
if(defined param('graph') and param('graph') =~ /^[0-9a-z\-]{1,100}$/) {
	$graph = param('graph') }

my $action = '';
if(defined param('action')) { $action = param('action') }
else { $action = 'list-graphs' }

my @activefilters = ();
if(defined param('filters')) {
	@activefilters = grep {/^[a-z0-9\-]+$/} split(',', param('filters'));
}

my $geometry = '';
if(defined param('geometry')) { $geometry = param('geometry') }

# Default periods in seconds.
my %periods = qw(7200 Hourly 86400 Daily 604800 Weekly 1814400 Montly 21772800 Yearly);

if($action eq 'render-graph') {
	my ($w, $h) = (400, 100); 	# width x height
	if($geometry =~ /^(\d+)x(\d+)$/) { $w = $1; $h = $2 }
	print header(-type=>'image/png', -expires=>'60s');
	my @graphparams = ();
	open(GRAPH, "./conf-graph/$graph") or die $!;
	while(<GRAPH>) { chomp; next if /^#/; push @graphparams, $_ }; close GRAPH;
	chdir('./data') or die $!;
	RRDs::graph( '-',
		"-E",
		"--end=now",
		"--start=end-$offset",
		"--width=$w",
		"--height=$h",
		"--font=AXIS:6",
		"--font=LEGEND:7",
		"--font=TITLE:7",
		"--color=BACK#FFFFFF",
		"--color=SHADEA#FFFFFF",
		"--color=SHADEB#FFFFFF",
		@graphparams );
	if(defined RRDs::error) { die RRDs::error }
}
elsif($action eq 'list-graphs') {
	opendir(GRAPHS, "./conf-graph") or die $!;
	my @graphs = sort grep {/^[a-z0-9\-]{1,100}$/} readdir GRAPHS;
	closedir GRAPHS;
 	print header, start_html(-title => "RRD Graphs", -style => {'src' => '?action=get-css'} );
	opendir(FILTERS, "./conf-graph-filters") or die $!;
	my %filters = ();
	foreach( grep {/^[a-z0-9\-]{1,100}$/} readdir FILTERS ) {
		open(FILTER, "./conf-graph-filters/$_") or die $!;
		$filters{$_} = <FILTER>; chomp $filters{$_}; close FILTER }
	closedir FILTERS;
	print qq{<table width="100%" border="0"><tr>};
	print qq{<td valign="top" width="500px">};
 	foreach(@graphs) {
		my $cgraph = $_; my $show = 0;
		if($re ne '') { if($cgraph =~ /$re/) { $show = 1 } }
		elsif(scalar @activefilters > 0) {
			my $filtersMatch = 0;
			foreach(@activefilters) {
				my $filterreg = $filters{$_};
				next if not defined $filterreg;
				if($cgraph =~ /$filterreg/) { $filtersMatch++ }
			}
			if($filtersMatch == scalar @activefilters) { $show = 1 }
		}
		next if $show == 0;
 		print h3($cgraph);
 		print qq{<a href="?action=list-graph-all-periods&graph=$cgraph"><img src="?action=render-graph&graph=$cgraph&offset=$offset" border=0></a>};
 	}
	gentime();
	print qq{<td valign="top">};
	print h1('RRD Graphs');
    print start_form( -method => 'GET' ),
      textfield( -name => 're', size => 10 ), hidden( 'offset', $offset ),
      hidden( 'action', $action ), submit( 'submit', 'Filter' ), end_form, p;
	print b("Graph Period"), p;
	foreach(sort { $a <=> $b } keys %periods) {
		if($re ne '') {
		 	print qq{<a href="?action=list-graphs&re=$restr&offset=$_">$periods{$_}</a><br>};
		}
		elsif(scalar @activefilters > 0) {
			my $filters = join(',', @activefilters);
		 	print qq{<a href="?action=list-graphs&filters=$filters&offset=$_">$periods{$_}</a><br>};
		}
		else {
		 	print qq{<a href="?action=list-graphs&offset=$_">$periods{$_}</a><br>};
		}
	}
	print p, b('Pre-Defined Filters'), p;
	foreach(sort keys %filters) {
		my $filter = $_;
		if(grep {/^$filter$/} @activefilters) { 
			my $filters = join(',', grep {!/^$filter$/} @activefilters);
			print qq{<b><a href="?action=list-graphs&filters=$filters&offset=$offset">$_</a></b><br>};
		}
		else {
			my $filters = join(',', @activefilters);
			print qq{<a href="?action=list-graphs&filters=$filters,$filter&offset=$offset">$_</a><br>};
		}
	}
	print p, b('All Graphs'), p;
	foreach(@graphs) { print qq{<a href="?action=list-graph-all-periods&graph=$_">$_</a><br>} }
	print qq{</td>};
	print qq{</table>};
}
elsif($action eq 'list-graph-all-periods') {
 	print header, start_html(-title => "RRD Graphs - $graph", -style => {'src' => '?action=get-css'} );
	print h1("RRD Graphs - $graph");
	open(GRAPH, "./conf-graph/$graph") or die $!;
	my $text = '';
	while(<GRAPH>) { 
		chomp;
		if(/^# *includehelp\(([a-z0-9]+)\) *$/) {
			open HELP, "./conf-graph-help/$1" or next;
			$text .= join("\n", <HELP>);
			close HELP;
		}
		elsif(s/^#//) { 
			$text .= $_ 
		} 
	} 
	close GRAPH;
	print $text;
	foreach(sort { $a <=> $b } keys %periods) {
		print h3($periods{$_});
		print qq{<img src="?action=render-graph&graph=$graph&offset=$_&geometry=650x200" border=0><p>};
	}
	gentime();
}
elsif($action eq 'get-css') {
	print header(-type => "text/css", -expires => '+1d');
	while(<DATA>) { print }
}

sub gentime { print p, i(small('Page generated at ', scalar localtime, " by <a href='http://zanardo.org/rrd.html'>rrd $VERSION</a>.")) }

__DATA__
body { margin: 1em 2em 2em 2em; }
h1, h2, h3, h4, h5, h6 {
  font-family: "Gill Sans", "Trebuchet MS", Verdana, sans-serif;
  font-weight: normal; 
}
img { border: none; }
dt { font-weight: bold; }
pre { background-color: #C0C0C0; font-weight: bold; }
a:link { color: #0000EE; }
a:visited { color: #0000EE; }
a:active { color: #0000EE; }
a:hover { color: #0000EE; }
