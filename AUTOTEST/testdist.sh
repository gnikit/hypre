#!/bin/sh
#BHEADER**********************************************************************
# Copyright (c) 2008,  Lawrence Livermore National Security, LLC.
# Produced at the Lawrence Livermore National Laboratory.
# This file is part of HYPRE.  See file COPYRIGHT for details.
#
# HYPRE is free software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License (as published by the Free
# Software Foundation) version 2.1 dated February 1999.
#
# $Revision$
#EHEADER**********************************************************************

# Which tests to run?
TEST_ALPHA="-`hostname -a`"
TEST_BETA="$TEST_ALPHA -alc"
TEST_GENERAL="$TEST_BETA -thunder -up -zeus"
TERMCMD=""

while [ "$*" ]
do
   case $1 in
      -h|-help)
         cat <<EOF

   $0 [options] {release}

   where: {release}  is a hypre release tar file (gzipped)

   with options:
      -xterm         run the tests in parallel using multiple xterm windows
      -h|-help       prints this usage information and exits
      -t|-trace      echo each command

   This script unpacks {release} in the parent directory and lists the tests
   needed to verify it. The list depends on the release type (alpha, beta, or
   general). If all required tests have passed, the script will generate a
   verification file containing the logs from the runs. Otherwise, tests that
   have failed or have not been run yet can be started, and the script will
   have to be re-run after their completion to generate the verification file.

   Example usage: $0 /usr/casc/hypre/hypre-2.0.0.tar.gz

EOF
         exit
         ;;
      -t|-trace)
         set -xv
         shift
         ;;
      -xterm)
         # Get the terminal command and make sure it runs bash
         TERMCMD="$TERM -e"; SHELL=/bin/sh
         shift
         ;;
      *)
         break
         ;;
   esac
done

# Setup
testing_dir=`cd ..; pwd`
autotest_dir="$testing_dir/AUTOTEST"
release_file=$1
release_dir=`basename $release_file | awk -F.t '{print $1}'`
release=`echo $release_dir | awk -F- '{print $2}'`
output_dir="$testing_dir/AUTOTEST-hypre-$release"
case `basename $release_file | awk -F. '{print $3}'` in
   *a) NAME="ALPHA"; TESTS=$TEST_ALPHA ;;
   *b) NAME="BETA"; TESTS=$TEST_BETA ;;
   *)  NAME="GENERAL"; TESTS=$TEST_GENERAL ;;
esac

# Extract the release
cd $testing_dir
echo "Checking the distribution file..."
if !(tar -dzf $release_file 2>/dev/null 1>&2) then
   rm -rf $release_dir $output_dir $autotest_dir/autotest-*
   tar -zxf $release_file
fi
echo ""
echo "The followinfg tests are needed to verify this $NAME release: $TESTS"
echo ""

# List the status of the required tests
cd $autotest_dir
NOTRUN=""
FAILED=""
PENDING=""
for test in $TESTS
do
   name=`echo $test | sed 's/[0-9]//g'`
   # Determine failed, pending, passed and tests that have not been run
   if [ -s $output_dir/machine$name.err -o -s autotest$name.err ]; then
      status="[FAILED] "; FAILED="$FAILED $test"
   else
      if [ ! -e autotest$name-start ]; then
         status="[NOT RUN]"; NOTRUN="$NOTRUN $test"
      else
         if [ ! -e autotest$name-done ]; then
            status="[PENDING]"; PENDING="$PENDING $test"
         else
            status="[PASSED] "
         fi
      fi
   fi
   if [ "$TERMCMD" == "" ]; then
      echo "$status ./autotest.sh -dist $release $test"
   else
      echo "$status $TERMCMD ./autotest.sh -dist $release $test &"
   fi
done

# If all tests have passed, return a tarball of the log files
if [ "$NOTRUN$FAILED$PENDING" == "" ]; then
   echo ""; echo "Generating the verification file AUTOTEST-hypre-$release.tgz"
   cd $testing_dir
   mv -f $autotest_dir/autotest-* $output_dir
   tar -zcf $autotest_dir/AUTOTEST-hypre-$release.tgz `basename $output_dir`
   echo "The release is verified!"
   exit
fi

cat <<EOF

The release can not be verified at this time, because not all tests are listed
as [PASSED]. This script can start the remaining tests now. Alternatively, you
can run the above commands manually (or in a cron job). If you do this, make
sure to examine the standart error of the autotest.sh script.

EOF

echo -n "Do you want to start the tests that have failed or have not run yet? (yes,no) : "
read -e RUN
if [ "$RUN" == "yes" ]; then
   for test in $FAILED $NOTRUN
   do
      name=`echo $test | sed 's/[0-9]//g'`
      rm -rf $output_dir/machine$name.??? autotest$name*
      if [ "$TERMCMD" == "" ]; then
         echo "Running test [./autotest.sh -dist $release $test]"
         ./autotest.sh -dist $release $test 2>> autotest$name.err
      else
         echo "Running test [$TERMCMD ./autotest.sh -dist $release $test &]"
         $TERMCMD "./autotest.sh -dist $release $test 2>> autotest$name.err" 2>> autotest$name.err &
      fi
      echo ""
   done
fi
echo ""
echo "Re-run the script after tests have completed to verify the release."
echo ""