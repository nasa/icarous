#! /usr/bin/perl

# Copyright (c) 2006-2014, Universities Space Research Association (USRA).
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Universities Space Research Association nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


$fileout = 'RegressionResults';
$file1 = @ARGV[0];
$file2 = @ARGV[1];

open(DIF, ">>$fileout");	# Open for output
$testName = $file1;
$testName =~ s/output\/RUN_(.*)_(.*)\.out/$1 + $2/;


# this is for comparing .out with .valid
# only if a .valid file is passed in
if ($file2){     

open (F1, $file1);
open (F2, $file2);
open(FE, STDERR);
@lines1 = <F1>;
@lines2 = <F2>;
$n = 0;
$m = 0;
foreach $temp (@lines2){
  $line1 = @lines1[$n];
  $line1 =~ s/(0x[0-9a-fA-F]{2,16})//g;
  $temp =~ s/(0x[0-9a-fA-F]{2,16})//g;
 if ($line1 ne $temp){   
      if ($m == 0){
          print DIF "\n";
          print DIF "**** TEST FAILED: ", $testName; 
          print "\nTEST FAILED:  ", $testName;
          print "***\n  ------ .out file differs from .valid file\n";
      }
      $m++;
      print DIF "\nLine: ", $n;
      print DIF "\n";
      print DIF ".out:   ", $line1; 
      print DIF ".valid: ", $temp;  
      print DIF "\n";
 
  }  
$n++  
}
if ($n == 0){
          print DIF "\n";
          print DIF "\n**** TEST FAILED: ", $testName; 
          print DIF "***\n  ------ No .output file was provided\n";
          print "\nTEST FAILED:  ", $testName, "\n";
          print "  ------ No .output file was provided\n";
}elsif ($m == 0){
   print DIF "\nTEST PASSED: ", $testName, "\n"; 
}
close(F2);

}else{
# Check for success of root node
# for files where this enough to show success
    open (F1, $file1);
    
    @lines1 = <F1>;
    $n = 0;
    $k = 0;
    foreach $line1 (@lines1){
        if ($line1 =~ m/Added plan:/){
            if (@lines1[$n+1] =~ m/^(.*)\{$/){
                $rootNode = $1;
                $count = @lines1;
                for ($i = $count-1; $i > 0; $i--){
                    if(@lines1[$i] =~ m/PlexilExec:printPlan/){
                         $k = 1;
                        if (@lines1[$i+1] =~ m/^$rootNode\{$/){
                            if (@lines1[$i+3] =~ m/^ Outcome: ([A-Z]*)$/){
							  $outcome = $1;
                                if ($outcome eq "SUCCESS"){
                                    print DIF "\nTEST PASSED: ", $testName, "\n";
                                    $i = 0;
                                }else{
                                    print DIF "\n";
                                    print DIF "\n*** TEST FAILED:  ", $testName, " ***\n  ------- root node outcome = ", $outcome, "\n";
                                    print "\nTEST FAILED:  ", $testName, "\n  ------- root node outcome = ", $outcome, "\n";
                                    $i = 0;
                                }
                            }else{
                                print DIF "\n";
                                print DIF "*** TEST FAILED:  ", $testName, " ***\n  ------- root node outcome != SUCCESS\n";
                                print "\nTEST FAILED:  ", $testName, "\n  ------- root node outcome != SUCCESS\n";
                                $i = 0;
                            }
                        }
                    }
                }       
            } 
        }
        $n++;
    }
    if ($k == 0){
      print DIF "\n";
      print DIF "*** TEST FAILED:  ", $testName, " ***\n  ------- ERROR in test execution\n";
      print "\nTEST FAILED:  ", $testName, "\n  ------- ERROR in test execution\n";
   }
 }

#this doesn't work (tempName) with libraries yet
$fileErrs = 'tempRegressionResults';
open (F3, $fileErrs);
my $i = 0;
@lines3 = <F3>;
foreach $line3 (@lines3){
    $i++;
   if ($line3 =~ s/^RUN_exec-test-runner_g_rt\.-s\.(.*?)\.xml\.-p\.(.*?)\.xml.*/$2 + $1/)
   {
      chomp($tempName = $line3);
      if ($tempName eq $testName){    
         $line3 = @lines3[$i++];
         while ($line3 && !($line3 =~ m/^RUN_exec-test-runner_g_rt\.-s\.(.*?)\.xml\.-p\.(.*?)\.xml.*/)){
            print DIF $line3;
               $line3 = @lines3[$i++];
          }
       }
  }
}

close(F1);
close(F2);
close(DIF);
