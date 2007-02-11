#!/usr/bin/perl <-- we don't need this line anymore

# This is currently written in perl, just to be quick, and easy to
# experiment with. Once the design is stable, this should be re-written
# in C to be portable to the largest number of platforms.

# NOTE:
# With new Makefile.am layout we do not need to use environment
# for arguments passing, so it might be worth going back to
# an easier-to-use interface for manual runs. 
# Also, it's probably good enough to use a bourne shell rather
# then C for this job.
#
# Mon Jul 10 12:42:30 CEST 2006 --strk;

# $(PERL)

$VERBOSE=1;
$SRCDIR=$ENV{'srcdir'};
$TOP_BUILDDIR=$ENV{'top_builddir'};
$BUILDDIR=$ENV{'builddir'};

sub usage()
{
	#print STDERR "BINDINGS=\"<binding> [<binding> ...]\" Usage: $0 <binding> [<binding> ...]\n";
	print STDERR "Usage: BINDINGS=\"<binding> [<binding> ...]\" $0\n"; #Usage: $0 <binding> [<binding> ...]\n";
	print STDERR " recognized bindings are: C, cxx, PERL, PHP, PYTHON\n";
	exit(1);
}

if ( ! $SRCDIR )
{
	print STDERR "This script is intended for use by automake.\n";
	print STDERR "You must export the 'srcdir' variable in order\n";
	print STDERR "to run it manually.\n";
	exit(1);
}

$do_test_c=0;
$do_test_cxx=0;
$do_test_perl=0;
$do_test_php=0;
$do_test_python=0;

@BINDINGS=split(' ', $ENV{'BINDINGS'});
if ( ! @BINDINGS ) {
	usage();
}
while (@BINDINGS)
{
	$arg = shift(@BINDINGS);

	if ( $arg =~ /^c$/i )
	{
		$do_test_c=1;
	}
	elsif ( $arg =~ /^cxx$/i )
	{
		$do_test_cxx=1;
	}
	elsif ( $arg =~ /^perl$/i )
	{
		$do_test_perl=1;
	}
	elsif ( $arg =~ /^php$/i )
	{
		$do_test_php=1;
	}
	elsif ( $arg =~ /^python$/i )
	{
		$do_test_python=1;
	}
	else
	{
		print STDERR "Unrecognized binding: '$arg'\n";
		usage();
	}

}

if ( $VERBOSE ) {
	print "Test c: ", $do_test_c, "\n";
	print "Test C++: ", $do_test_cxx, "\n";
	print "Test perl: ", $do_test_perl, "\n";
	print "Test python: ", $do_test_python, "\n";
	print "Test php: ", $do_test_php, "\n";
}


$failures=0;

sub doswftest($$$)
{
	local($testswf,$testref,$testbuilder)=@_;

	#$testswf = "$test.swf";
	#$testref = "$test.ref";

	chomp( $pwd = `pwd`);

	# Use absolute paths
	$testbuilder="./".$testbuilder unless ( $testbuilder =~ m@^/@ );
	$testswf=$pwd."/".$testswf unless ( $testswf =~ m@^/@ );

	if( ! -r $testbuilder ) {
		printf  STDERR "$test failed. $testbuilder does not exist\n";
		++$failures;
		return 0;
	}
	if( ! -x $testbuilder ) {
		# This is needed for script objects, as they are present in
		# the repository (not generated at build time) w/out
		# the execute bit. Dunno if it's possible to set the 
		# execute bit automatically.
		print "Setting execute bit on  $testbuilder in $swfdir\n";
		system("chmod +x $testbuilder");
		#printf  STDERR "$test failed. $testbuilder not executable\n";
		#++$failures;
		#return 0;
	}
	if( -r $testswf ) {
		system("rm -f $testswf");
	}

	# Set some vaiable to ensure we pick up the just build library & modules
	if( !$ENV{'USE_INSTALLED'} ) {
		$ENV{'LD_LIBRARY_PATH'}=$TOP_BUILDDIR."/src/.libs";
		$ENV{'PYTHONPATH'}=$TOP_BUILDDIR."/py_ext/build/lib.linux-i686-2.4";
		$ENV{'PERL5LIB'}=$TOP_BUILDDIR."/perl_ext/blib/lib:".$TOP_BUILDDIR."/perl_ext/blib/arch/";
	} else {
		# Try to avoid the autogoo and run the test w/ installed libs
		if( -x ".libs/".$testbuilder ) {
			$testbuilder=".libs/".$testbuilder
		}
	}
	# test will be created in CWD, so let's
	# chdir to target dir
	chomp($cwd = `pwd`);
	chomp($swfdir = `dirname $testswf`);
	chdir($swfdir);
	print "Executing $testbuilder in $swfdir\n";
	system($testbuilder);
	chdir($cwd);


	if( ! -r $testswf ) {
		printf  STDERR "$test failed. $testswf not created\n";
		++$failures;
		return 0;
	}
	if( ! -r $testref ) {
		printf  STDERR "$test failed. $testref does not exist\n";
		++$failures;
		return 0;
	}
	if( system("$TOP_BUILDDIR/util/listswf $testswf | diff - $testref") ) {
		printf  STDERR "$test failed. problem comparing against $testref\n";
		++$failures;
		return 0;
	}
	return 1;
}

sub dotestset($$$)
{
	local($depth,$srcdir,$builddir) = @_;

	local %test;

	local $TESTLIST;

	print "dotestset depth $depth\n";

	open($TESTLIST,"<$srcdir/TestList") || die "Can't find $srcdir/TestList";
	while(<$TESTLIST>) {
		@test = split(':');
		if( $test[1] eq "swf" ) {

			$test = $test[0];
			$testswf = $builddir."/".$test.".swf";
			$testref = $srcdir."/".$test.".ref";

			if ( $do_test_c ) {
				$testbuilder = $test;
				#print "TESTBUILDER: $testbuilder\n";
				doswftest($testswf, $testref, $testbuilder);
			}

			if ( $do_test_cxx ) {
				$testbuilder = $test."-cxx";
				doswftest($testswf, $testref, $testbuilder);
			}

			if ( $do_test_php ) {
				$testbuilder = $srcdir."/".$test.".php";
				doswftest($testswf, $testref, $testbuilder);
			}

			if ( $do_test_perl ) {
				$testbuilder = $srcdir."/".$test.".pl";
				doswftest($testswf, $testref, $testbuilder);
			}

			if ( $do_test_python ) {
				$testbuilder = $srcdir."/".$test.".py";
				doswftest($testswf, $testref, $testbuilder);
			}
		} elsif ( $test[1] eq "dir" ) {
			$subdir=$test[0];
			print "doing tests in ".$subdir."\n";
			#chdir $test[0] || die "Can't cd to ".$test[0]."\n";
			dotestset($depth+1, $srcdir."/".$subdir, $builddir."/".$subdir);
			#chdir ".." || die "Can't cd ..";
			print "done in ".$subdir."\n";
		} else {
			print STDERR "Don't know how to handle test type ".$test[1]."\n";
		}
	}
	print "End of loop for depth $depth\n";
	close($TESTLIST);
}

dotestset(0, $SRCDIR, $BUILDDIR);

exit $failures;
