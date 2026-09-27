This directory contains some regression tests that are run with make check (about 1 minute) and some other regression tests that are not run with make check because running them every time would take too long. These tests are
* geometry tests from International Mathematic Olympiad, you can run them all at once with chk_oim
* integration tests from Nasser M. Abbasi
https://www.12000.org/my_notes/CAS_integration_tests/reports/summer_2024/indexchapter3.htm#x188-179860003
Run chkint.sh to run these tests (this will run the tests in parallel, assuming you have enough CPU). The timeout value is set to 5 seconds, and may be changed in chkint.cas
Or run chk_intmit.sh (from MIT bee integration) or chk_intind.sh (independant integration tests) or chk_intheb?.sh (from W. Hebisch, aka [part of?] Axiom/Fricas regression tests) or chk_intblake.sh (from S. Blake).

==============================================================
Howto: add new integration tests from this basis of more than 100 000 integrals
==============================================================
A/ From several maple text input files: 
  copy the maple files inside a subdirectory named dirname
  copy intindep.cas to intdirname.cas and chk_indep to chk_dirname
  replace indep by dirname in these files
B/ From one individual file:
1/ Copy intheb1.cas to intfilename.cas and chk_intheb1.sh to chk_intfilename.sh
2/ In chk_intfilename.sh:
  replace intheb1.cas by intfilename.cas
3/ In intfilename.cas:
  Replace heb1 by filename after testname:=
  Erase from lst:=[ to the closing ]:; at the end of intfilename
  Get the maple text input file from the URL above (choose in the tests, then go into Appendix, link to plain text files and maple), this a raw text file starting with lst:=[[ and ending with ]:
  Copy/paste to the intfilename.cas where you erased lst:=[
  At the end replace : by :;
  Save the file
4/ Now you are ready to run it
  ./chk_intfilename.sh
5/ Optional: to get the tests faster, you can disable some integration or verification step.

If the display stalls too long, look if it's at the integration step or at the checking step (last line starts with integration start or integration done), check the test number. Add 4 to get the corresponding entry line in the text file. If it stalled at integration step, add [fail,x,1,2,fail,1],// at the line begin (this will skip the test and mark it as failed). If it stalled at the antiderivative checking step, search in the line x,1 and replace 1 by "nock" (cf. examples in intheb1).
Before restarting the tests, replace 0 by the test number in the loop from j from 0 to S-1 do at the end of the file, so that you restart where you were. After a few edits you should reach the end of the tests. Reset the loop start to 0 in for j from 0 to S-1 do. Now the full test should run without stalling.

Note that some tests that stall or fail with integrate may sometimes work with risch.
